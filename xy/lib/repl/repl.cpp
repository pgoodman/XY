/*
 * repl.cpp
 *
 *  Created on: Nov 29, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <pthread.h>
#include <unistd.h>

#include "xy/include/repl/repl.hpp"

#include "xy/deps/linenoise/linenoise.h"

#include "xy/include/array.hpp"
#include "xy/include/cstring.hpp"

#include "xy/include/support/unsafe_cast.hpp"

#define D(x) x

namespace xy { namespace repl {

    static bool IN_REPL(false);
    static pthread_t READ_THREAD;
    static char REPL_BUFFER[8193];

    // keep track of if we can accept
    static unsigned NUM_WAIT_REQUESTS(0);
    static bool AT_END_OF_INPUT(true);

    // owned by whoever is currently running
    static pthread_mutex_t REPL_EXECUTION_LOCK;
    static bool READ_THREAD_GETS_LOCK(false);

    /// set auto-completion things for linenoise
    static void completion(const char *buff, linenoiseCompletions *lc) {
        switch(buff[0]) {
        case 'l': linenoiseAddCompletion(lc,"let"); break;
        case 'f': linenoiseAddCompletion(lc,"function"); break;
        case 'r':
            linenoiseAddCompletion(lc,"record");
            linenoiseAddCompletion(lc,"return");
            break;
        case 'u': linenoiseAddCompletion(lc,"union"); break;
        case '-': linenoiseAddCompletion(lc,"->"); break;
        case ':': linenoiseAddCompletion(lc,":="); break;
        case 'y': linenoiseAddCompletion(lc,"yield"); break;
        case '=': linenoiseAddCompletion(lc,"=>"); break;
        default: break;
        }
    }

    static char *append_line(char *cursor, const char *end, char *line, size_t line_len) throw() {
        if((cursor + line_len + 1) > end) {
            free(line);
            fprintf(stderr, "Error: REPL buffer full.\n");
            ::exit(EXIT_FAILURE);
        }

        memcpy(cursor, line, line_len);
        cursor[line_len] = '\n';
        free(line);

        return cursor + line_len + 1;
    }

    /// start up the reader thread
    static void *read_thread(void *) throw() {

        const char *end_of_buffer(&array::last(REPL_BUFFER) - 1);
        char *cursor(nullptr);
        char *line(nullptr);
        size_t line_len(0);

        D( printf("read thread is %lu\n", support::unsafe_cast<size_t>(pthread_self())); )

        if(!READ_THREAD_GETS_LOCK) {
            eval::yield();
        }

        for(; IN_REPL && READ_THREAD_GETS_LOCK; ) {

            memset(REPL_BUFFER, 0, array::size(REPL_BUFFER));
            cursor = &(REPL_BUFFER[0]);

            // reset for each new statement
            AT_END_OF_INPUT = true;
            NUM_WAIT_REQUESTS = 0U;
            line = linenoise(">>> ");

            if(nullptr == line) {
                repl::exit();
                break;
            }

            if(0 == strcmp("exit", line)) {
                D( printf("read thread is exiting\n"); )
                repl::exit();
                break;
            }

            D( printf("got '%s'\n", line); )
            do {
                line_len = cstring::byte_length(line);
                cursor = append_line(cursor, end_of_buffer, line, line_len);
                line = nullptr;

                D( printf("buffer is '%s'\n", REPL_BUFFER); )

                eval::yield();

                if(AT_END_OF_INPUT) {
                    break;
                }

                // continue getting input
                line = linenoise("... ");
                if(nullptr == line) {
                    repl::exit();
                    break;
                }

            } while(IN_REPL);
        }

        return nullptr;
    }

    /// intialize the REPL. The evaluator/parser thread is the main thread, and
    /// the reader thread is created to handle reading.
    char *init(void) throw() {
        if(IN_REPL) {
            return &(REPL_BUFFER[0]);
        }

        linenoiseSetCompletionCallback(completion);

        IN_REPL = true;

        if(0 != pthread_mutex_init(&REPL_EXECUTION_LOCK, nullptr)) {
            ::exit(EXIT_FAILURE);
        }

        D( printf("main thread is %lu\n", support::unsafe_cast<size_t>(pthread_self())); )

        // presumption: reader has the lock
        READ_THREAD_GETS_LOCK = true;
        pthread_mutex_lock(&REPL_EXECUTION_LOCK);

        // create another thread
        if(0 != pthread_create(&READ_THREAD, nullptr, &read_thread, nullptr)) {
            ::exit(EXIT_FAILURE);
        }

        return &(REPL_BUFFER[0]);
    }

    /// exit the current thread and turn off the REPL if it's on
    void exit(void) throw() {
        if(IN_REPL) {
            IN_REPL = false;
            READ_THREAD_GETS_LOCK = !READ_THREAD_GETS_LOCK;
            pthread_mutex_unlock(&REPL_EXECUTION_LOCK);
        } else {
            pthread_mutex_destroy(&REPL_EXECUTION_LOCK);
        }

        D( printf("thread %lu is exiting\n", support::unsafe_cast<size_t>(pthread_self())); )

        pthread_exit(nullptr);
    }

    /// check if we can keep going
    bool check(void) throw() {
        return IN_REPL;
    }

    void wait(void) throw() {
        NUM_WAIT_REQUESTS += 1;
        AT_END_OF_INPUT = 0U == NUM_WAIT_REQUESTS;
    }

    void accept(void) throw() {
        NUM_WAIT_REQUESTS -= 1;
        AT_END_OF_INPUT = 0U == NUM_WAIT_REQUESTS;
    }

    bool should_wait(void) throw() {
        return !AT_END_OF_INPUT;
    }

    namespace eval {

        /// yield the executing to the eval thread; returns when the read
        /// thread is given the lock back.
        void yield(void) throw() {
            if(!IN_REPL) {
                return;
            }

            D( printf("yielding to eval thread from %lu\n", support::unsafe_cast<size_t>(pthread_self())); )

            // pre-condition:
            // - we are in the read thread
            // - we have the lock; we want to give it away to the eval thread,
            //   and only return when we should get the lock back.

            READ_THREAD_GETS_LOCK = false;
            pthread_mutex_unlock(&REPL_EXECUTION_LOCK);

            for(; IN_REPL; ) {
                if(0 != pthread_mutex_lock(&REPL_EXECUTION_LOCK)) {
                    ::exit(EXIT_FAILURE);
                }

                if(!READ_THREAD_GETS_LOCK) {
                    pthread_mutex_unlock(&REPL_EXECUTION_LOCK);
                    D( sleep(1U); )
                    continue;
                }

                break;
            }
        }
    }

    namespace read {

        /// yield execution to the read thread; called from the eval thread.
        /// this will return when an eval::yield has been called.
        void yield(void) throw() {
            if(!IN_REPL) {
                return;
            }

            D( printf("yielding to read thread from %lu\n", support::unsafe_cast<size_t>(pthread_self())); )

            // pre-condition:
            // - we are in the eval thread
            // - we have the lock; we want to give it away to the read thread,
            //   and only return when we should get the lock back.

            READ_THREAD_GETS_LOCK = true;
            pthread_mutex_unlock(&REPL_EXECUTION_LOCK);

            for(; IN_REPL; ) {
                if(0 != pthread_mutex_lock(&REPL_EXECUTION_LOCK)) {
                    ::exit(EXIT_FAILURE);
                }

                if(READ_THREAD_GETS_LOCK) {
                    pthread_mutex_unlock(&REPL_EXECUTION_LOCK);
                    D( sleep(1U); )
                    continue;
                }

                break;
            }
        }
    }

}}


