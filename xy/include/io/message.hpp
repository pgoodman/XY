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

namespace xy { namespace io {

    template <typename MessageType>
    class MessageTemplate;

    class file_message_tag { };
    class arg_message_tag { };

    template <typename MessageType>
    class Message {

    };

    template <>
    class MessageTemplate<file_message_tag> {

    };

    template <>
    class MessageTemplate<arg_message_tag> {

    };
}}

#endif /* XY_MESSAGE_HPP_ */
