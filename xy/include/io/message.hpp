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
#include <tr1/utility>
#include <tr1/type_traits>
#include <cstdio>
#include <cstring>
#include <iterator>

#include "xy/include/cstring.hpp"

#include "xy/include/mpl/bool.hpp"
#include "xy/include/mpl/equal.hpp"
#include "xy/include/mpl/remove_const.hpp"

#include "xy/include/support/unsafe_cast.hpp"

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

        sentinel_type = 6
    } message_type;

    /// represents a single diagnostic message template. Each message has a
    /// unique identifier, a type (e.g. error, warning), and a format string.
    /// The format string uses C printf formatting rules. Diagnostic message
    /// templates are "instantiated" by means of the message class, where the
    /// parameters to be substituted into the string and stored.
    struct message_template {
    public:
        const message_id id;
        const message_type type;
        const char * const message;
    };

    // forward declarations
    class message_queue;
    class message_iterator;

    /// represents an instantiated message template with its formal parameters.
    /// the parameters are stored in a tuple (which is defined in the derived
    /// template classes).
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

#define XY_MAX_POD_OBJS(F)                                      \
    F(char)                                                     \
    F(unsigned char)                                            \
    F(short)                                                    \
    F(unsigned short)                                           \
    F(int)                                                      \
    F(unsigned)                                                 \
    F(long)                                                     \
    F(unsigned long)                                            \
    F(long long)                                                \
    F(unsigned long long)                                       \
    F(float)                                                    \
    F(double)

#define XY_MAKE_POD_CONV(T)                                     \
    template <>                                                 \
    class as_pod<T> {                                           \
    public:                                                     \
        typedef const mpl::remove_const<T>::type type; \
        static type convert(type &v) throw() {                  \
            return v;                                           \
        }                                                       \
    };

        template <typename T>
        class as_pod;

        template <typename T, bool>
        class as_pod_impl;

        /// convert some type that is integral, and so is indeed a base type
        template <typename T>
        class as_pod_impl<T, true> {
        public:
            typedef int type;
            static int convert(const typename mpl::remove_const<T>::type &v) throw() {
                return static_cast<int>(v);
            }
        };

        /// comvert some type that is not integral
        template <typename T>
        class as_pod_impl<T, false> {
        public:
            typedef const char *type;
            static const char *convert(const typename mpl::remove_const<T>::type &v) throw() {
                return v.operator const char *();
            }
        };

        template <typename T>
        class as_pod {
        public:
            typedef as_pod_impl<T, std::tr1::is_enum<T>::value> converter;
            typedef typename converter::type type;
            static type convert(
                const typename mpl::remove_const<T>::type &v
            ) throw() {
                return converter::convert(v);
            }
        };

        template <typename T>
        class as_pod<const T> : public as_pod<T> { };

        XY_MAX_POD_OBJS(XY_MAKE_POD_CONV)
        XY_MAKE_POD_CONV(char *)
        XY_MAKE_POD_CONV(const char *)

        template <typename T>
        typename as_pod<T>::type to_pod(const T &v) throw() {
            return as_pod<T>::convert(v);
        }

        /// send the tuple parameters of an instantiated message template
        /// as arguments to C printf

        template <const int len, typename tuple_type>
        class print_tuple {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &);
        };

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
                fprintf(fp, fmt, to_pod(std::tr1::get<0>(t)));
            }
        };

        template <typename tuple_type>
        class print_tuple<2, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt, to_pod(std::tr1::get<0>(t)), to_pod(std::tr1::get<1>(t)));
            }
        };

        template <typename tuple_type>
        class print_tuple<3, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    to_pod(std::tr1::get<0>(t)), to_pod(std::tr1::get<1>(t)), to_pod(std::tr1::get<2>(t))
                );
            }
        };

        template <typename tuple_type>
        class print_tuple<4, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    to_pod(std::tr1::get<0>(t)), to_pod(std::tr1::get<1>(t)), to_pod(std::tr1::get<2>(t)),
                    to_pod(std::tr1::get<3>(t))
                );
            }
        };

        template <typename tuple_type>
        class print_tuple<5, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    to_pod(std::tr1::get<0>(t)), to_pod(std::tr1::get<1>(t)), to_pod(std::tr1::get<2>(t)),
                    to_pod(std::tr1::get<3>(t)), to_pod(std::tr1::get<4>(t))
                );
            }
        };

        template <typename tuple_type>
        class print_tuple<6, tuple_type> {
        public:
            static void print(FILE *fp, const char *fmt, const tuple_type &t) {
                fprintf(fp, fmt,
                    to_pod(std::tr1::get<0>(t)), to_pod(std::tr1::get<1>(t)), to_pod(std::tr1::get<2>(t)),
                    to_pod(std::tr1::get<3>(t)), to_pod(std::tr1::get<4>(t)), to_pod(std::tr1::get<5>(t))
                );
            }
        };

        /// recursively duplicate and replace parameter C strings stored in the
        /// message tuple. This is so that parameter strings can be stored in
        /// reusable buffers and so that those buffers can be safely passed as
        /// arguments to the messages because the string contents of the
        /// buffers will be duplicated.

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

        /// recursively free any C strings stored in the tuple of arguments in
        /// the instantiated message template

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

        /// implementation of a given instantiated message template

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

    /// represents a simple queue of messages, where each message is represented
    /// by a message id and the arguments to substitute into the message.
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

        /// push a message into the queue.
        template <typename ...arg_types>
        void push(message_id id, arg_types... args) throw() {
            message *msg(new message_impl<arg_types...>(id, args...));
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

        /// check if we have any messages
        bool has_message(void) const throw();

        /// check if we have any messages of a particular type
        bool has_message(message_type) const throw();

        /// take control of all of the messages of another queue
        void subsume(message_queue &other) throw();
    };

}}

#endif /* XY_MESSAGE_HPP_ */
