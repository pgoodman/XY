/*
 * codepoint.hpp
 *
 *  Created on: Jun 14, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_CODEPOINT_HPP_
#define XY_CODEPOINT_HPP_

namespace xy { namespace utf8 {

    class decoder;

    class codepoint {
    private:

        friend class decoder;

        char data[5];

    public:

        codepoint(void);
        codepoint(const char);
        codepoint(const codepoint &);

        friend bool operator==(const char, const codepoint &) throw();

        bool operator==(const codepoint &) const throw();

        codepoint &operator=(const char) throw();
        codepoint &operator=(const codepoint &) throw();
    };

    bool operator==(const char, const codepoint &) throw();
}}

#endif /* XY_CODEPOINT_HPP_ */
