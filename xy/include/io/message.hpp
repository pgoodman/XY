/*
 * message.hpp
 *
 *  Created on: Jun 28, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_MESSAGE_HPP_
#define XY_MESSAGE_HPP_

#include <stdint.h>
#include <tr1/tuple>
#include <utility>
#include <cstdio>
#include <cstring>
#include <iterator>

#include "xy/include/cstring.hpp"

#include "xy/include/mpl/bool.hpp"
#include "xy/include/mpl/equal.hpp"

namespace xy { namespace io {

#ifdef XY_MSG
#undef XY_MSG
#endif

#   define XY_MSG(key, type, message) key,

    typedef enum : uint16_t {
#       include "xy/messages/messages.inc"
        sentinel_id
    } message_id;

#   undef XY_MSG

    typedef enum : uint8_t {
        recoverable_error = 0,
        error = 1,
        warning = 2,
        note = 3,
        failed_assertion = 4,
        special = 5,

        sentinel_type = 6,
    } message_type;

    struct message_template {
    public:
        const message_id id;
        const message_type type;
        const char * const message;
    };

    // forward declarations
    class message_queue;
    class message_iterator;

    class message {
    private:

        friend class message_queue;
        friend class message_iterator;

        message *next;

    protected:

        static const message_template MESSAGE_STRINGS[];
        static const char * const MESSAGE_TYPES[];

    public:

        const message_id id;
        const message_type type;

        message(message_id) throw();

        virtual ~message(void) throw();

        virtual void print(FILE *) const throw() = 0;
    };

    /// iterator for messages in a message queue
    class message_iterator : public std::iterator<std::input_iterator_tag, const message *> {
    private:
        friend class message_queue;

        const message *ptr;

        message_iterator(void) throw();
        message_iterator(const message *) throw();

    public:

        ~message_iterator(void) throw();
        message_iterator(const message_iterator &) throw();
        message_iterator &operator=(const message_iterator &) throw();

        bool operator==(const message_iterator &) const throw();
        bool operator!=(const message_iterator &) const throw();

        value_type operator*(void) const throw();
        value_type operator->(void) const throw();

        message_iterator &operator++(void) throw();
        message_iterator operator++(int) const throw();
    };

    namespace {

        template <const int len, typename tuple_type>
        class print_tuple;

        template <typename tuple_type>
        class print_tuple<0, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &) {
                fprintf(fp, fmt);
            }
        };

        template <typename tuple_type>
        class print_tuple<1, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt, std::tr1::get<0>(t));
            }
        };

        template <typename tuple_type>
        class print_tuple<2, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt, std::tr1::get<0>(t), std::tr1::get<1>(t));
            }
        };

        template <typename tuple_type>
        class print_tuple<3, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    std::tr1::get<0>(t), std::tr1::get<1>(t), std::tr1::get<2>(t)
                );
            }
        };

        template <typename tuple_type>
        class print_tuple<4, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    std::tr1::get<0>(t), std::tr1::get<1>(t), std::tr1::get<2>(t),
                    std::tr1::get<3>(t)
                );
            }
        };

        template <typename tuple_type>
        class print_tuple<5, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    std::tr1::get<0>(t), std::tr1::get<1>(t), std::tr1::get<2>(t),
                    std::tr1::get<3>(t), std::tr1::get<4>(t)
                );
            }
        };

        template <typename tuple_type>
        class print_tuple<6, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    std::tr1::get<0>(t), std::tr1::get<1>(t), std::tr1::get<2>(t),
                    std::tr1::get<3>(t), std::tr1::get<4>(t), std::tr1::get<5>(t)
                );
            }
        };

        template <typename result, const int i, typename tuple_type>
        class allocate_string;

        template <const int i, typename tuple_type>
        class allocate_string<mpl::false_tag, i, tuple_type> {
        public:
            static void allocate(tuple_type &) { }
        };

        template <const int i, typename tuple_type>
        class allocate_string<mpl::true_tag, i, tuple_type> {
        public:
            static void allocate(tuple_type &t) {
                std::tr1::get<i>(t) = cstring::copy(std::tr1::get<i>(t));
            }
        };

        // inductive step
        template <const int i, const int len, typename tuple_type>
        class allocate_strings {
        public:
            static void allocate(tuple_type &t) throw() {
                allocate_string<
                    typename mpl::equal<
                        char *,
                        typename std::tr1::tuple_element<i,tuple_type>::type
                    >::result,
                    i,
                    tuple_type
                >::allocate(t);
                allocate_strings<i + 1, len, tuple_type>::allocate(t);
            }
        };

        // base case
        template <const int len, typename tuple_type>
        class allocate_strings<len, len, tuple_type> {
        public:
            static void allocate(tuple_type &) throw() { }
        };

        template <typename result, const int i, typename tuple_type>
        class free_string;

        template <const int i, typename tuple_type>
        class free_string<mpl::false_tag, i, tuple_type> {
        public:
            static void free(tuple_type &) { }
        };

        template <const int i, typename tuple_type>
        class free_string<mpl::true_tag, i, tuple_type> {
        public:
            static void free(tuple_type &t) {
                cstring::free(std::tr1::get<i>(t));
                std::tr1::get<i>(t) = nullptr;
            }
        };

        // inductive step
        template <const int i, const int len, typename tuple_type>
        class free_strings {
        public:
            static void free(tuple_type &t) throw() {
                free_string<
                    typename mpl::equal<
                        char *,
                        typename std::tr1::tuple_element<i,tuple_type>::type
                    >::result,
                    i,
                    tuple_type
                >::free(t);
                free_strings<i + 1, len, tuple_type>::free(t);
            }
        };

        // base case
        template <const int len, typename tuple_type>
        class free_strings<len, len, tuple_type> {
        public:
            static void free(tuple_type &) throw() { }
        };

        template <typename ...arg_types>
        class message_impl : public message {
        public:
            typedef std::tr1::tuple<arg_types...> tuple_type;
            enum {
                TUPLE_SIZE = std::tr1::tuple_size<tuple_type>::value
            };


            tuple_type values;

            message_impl(message_id id_, arg_types... values_) throw()
                : message(id_)
                , values(values_...)
            {
                allocate_strings<0, TUPLE_SIZE, tuple_type>::allocate(values);
            }

            virtual ~message_impl(void) throw() {
                free_strings<0, TUPLE_SIZE, tuple_type>::free(values);
            }

            virtual void print(FILE *fp) const throw() {
                fprintf(fp, "%s" XY_F_BOLD, message::MESSAGE_TYPES[this->type]);
                print_tuple<
                    TUPLE_SIZE,
                    tuple_type
                >::print(fp, message::MESSAGE_STRINGS[id].message, values);
            }
        };

        template <>
        class message_impl<> : public message {
        public:

            message_impl(message_id id_) throw()
                : message(id_)
            { }

            virtual ~message_impl(void) throw() { }

            virtual void print(FILE *fp) const throw() {
                fprintf(fp, "%s" XY_F_BOLD, message::MESSAGE_TYPES[this->type]);
                fprintf(fp, message::MESSAGE_STRINGS[id].message);
            }
        };
    }

    class message_queue {
    private:

        friend class message;
        friend class message_iterator;

        message *first;
        message *last;

        unsigned num_messages;
        bool seen[static_cast<size_t>(sentinel_type) + 1U];

        class message_end : public message {
        public:
            message_end(void) throw();
            virtual void print(FILE *) const throw();
        };

        static message_end LAST_MESSAGE;

    public:

        message_queue(void) throw();

        ~message_queue(void) throw();

        template <typename ...arg_types>
        void push(message_id id, arg_types... args) throw() {
            message *msg{new message_impl<arg_types...>(id, args...)};
            seen[msg->type] = true;
            if(&LAST_MESSAGE == first) {
                first = msg;
                last = msg;
            } else {
                last->next = msg;
                last = msg;
            }
            last->next = &LAST_MESSAGE;
            ++num_messages;
        }

        message_iterator begin(void) const throw();
        message_iterator end(void) const throw();

        bool has_message(void) const throw();

        bool has_message(message_type) const throw();
    };

}}

#endif /* XY_MESSAGE_HPP_ */
