/*
 * message.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include "xy/include/color.hpp"
#include "xy/include/array.hpp"
#include "xy/include/io/message.hpp"

namespace xy { namespace io {

    message_queue::message_end::message_end(void) throw()
        : message(sentinel_id)
    { }

    void message_queue::message_end::print(FILE *) const throw() { }

    message_queue::message_end message_queue::LAST_MESSAGE;

    message::message(message_id id_) throw()
        : next(&(message_queue::LAST_MESSAGE))
        , id(id_)
        , type(message::MESSAGE_STRINGS[id_].type)
    { }

    message::~message(void) throw() {
        next = nullptr;
    }

#   define XY_MSG(key, type, message) {key, type, XY_F_DEF message},
    const message_template message::MESSAGE_STRINGS[]{
#       include "xy/messages/messages.inc"
        {sentinel_id, sentinel_type, nullptr}
    };
#   undef XY_MSG

    const char * const message::MESSAGE_TYPES[] = {
        XY_F_BOLD XY_F_RED "error: " XY_F_DEF,
        XY_F_BOLD XY_F_RED "error: " XY_F_DEF,
        XY_F_BOLD XY_F_BLUE "warning: " XY_F_DEF,
        XY_F_BOLD XY_F_GREEN "note: " XY_F_DEF,
        XY_F_BOLD XY_F_RED "failed assertion: " XY_F_DEF,
        "",
        ""
    };

    message_iterator::message_iterator(void) throw() {
        ptr = &(message_queue::LAST_MESSAGE);
    }
    message_iterator::~message_iterator(void) throw() {
        ptr = &(message_queue::LAST_MESSAGE);
    }
    message_iterator::message_iterator(const message *msg) throw() {
        ptr = msg;
    }

    message_iterator::message_iterator(const message_iterator &o) throw() {
        ptr = o.ptr;
    }
    message_iterator &message_iterator::operator=(const message_iterator &o) throw() {
        ptr = o.ptr;
        return *this;
    }

    bool message_iterator::operator==(const message_iterator &o) const throw() {
        return ptr == o.ptr;
    }
    bool message_iterator::operator!=(const message_iterator &o) const throw() {
        return ptr != o.ptr;
    }

    message_iterator::value_type message_iterator::operator*(void) const throw() {
        return ptr;
    }
    message_iterator::value_type message_iterator::operator->(void) const throw() {
        return ptr;
    }

    message_iterator &message_iterator::operator++(void) throw() {
        ptr = ptr->next;
        return *this;
    }

    message_iterator message_iterator::operator++(int) const throw() {
        return message_iterator(ptr->next);
    }

    message_queue::message_queue(void) throw()
        : first(&LAST_MESSAGE)
        , last(&LAST_MESSAGE)
        , num_messages(0)
    {
        memset(seen, 0, array::size(seen));
    }

    message_queue::~message_queue(void) throw() {
        for(message *msg{first}, *next{&LAST_MESSAGE}; &LAST_MESSAGE != msg; msg = next) {
            next = msg->next;
            delete msg;
        }
    }

    message_iterator message_queue::begin(void) const throw() {
        return message_iterator(first);
    }

    message_iterator message_queue::end(void) const throw() {
        return message_iterator(&LAST_MESSAGE);
    }

    bool message_queue::has_message(void) const throw() {
        return 0U < num_messages;
    }

    bool message_queue::has_message(message_type type) const throw() {
        return seen[type];
    }
}}
