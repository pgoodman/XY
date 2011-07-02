/*
 * message.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include "xy/include/array.hpp"
#include "xy/include/io/message.hpp"

namespace xy { namespace io {

    message::message(void) throw()
        : next(nullptr)
    { }

    message::~message(void) throw() {
        next = nullptr;
    }

#   define XY_MSG(key, type, message) {key, type, message},
    const message_template message::MESSAGE_STRINGS[]{
#       include "xy/messages/messages.inc"
        {sentinel_id, sentinel_type, nullptr}
    };
#   undef XY_MSG

    message_queue::message_queue(void) throw()
        : first(nullptr)
        , last(nullptr)
    {
        memset(seen, 0, array::size(seen));
    }

    message_queue::~message_queue(void) throw() {
        for(message *msg{first}, *next{nullptr}; nullptr != msg; msg = next) {
            next = msg->next;
            delete msg;
        }
    }

    void message_queue::print_all(FILE *fp) const throw() {
        for(message *msg{first}; nullptr != msg; msg = msg->next) {
            msg->print(fp);
        }
    }
}}
