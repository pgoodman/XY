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

#define D(x)

namespace xy { namespace repl {

    static unsigned NUM_WAIT_REQUESTS(0);
    static bool AT_END_OF_INPUT(false);

    void wait(void) throw() {
        D( printf("REPL: incrementing %u\n", NUM_WAIT_REQUESTS + 1); )
        ++NUM_WAIT_REQUESTS;
        AT_END_OF_INPUT = 0U == NUM_WAIT_REQUESTS;
    }

    void accept(void) throw() {
        D( printf("REPL: decrementing %u\n", NUM_WAIT_REQUESTS - 1); )
        --NUM_WAIT_REQUESTS;
        AT_END_OF_INPUT = 0U == NUM_WAIT_REQUESTS;
    }

    bool can_accept(void) throw() {
        return AT_END_OF_INPUT;
    }
}}


