
#include <cstring>
#include <cassert>

#include "xy/include/array.hpp"
#include "xy/include/utf8/decoder.hpp"
#include "xy/include/utf8/codepoint.hpp"


// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

namespace {

    static const uint8_t utf8d[] = {
        // The first part of the table maps bytes to character classes that
        // to reduce the size of the transition table and create bitmasks.
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

        // The second part is a transition table that maps a combination
        // of a state of the automaton and a character class to a state.
        0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
        12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
        12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
        12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
        12,36,12,12,12,12,12,12,12,12,12,12,
    };

    enum {
        ACCEPT_STATE = 0,
        SINK_STATE = 12
    };

    static uint8_t REPLACEMENT_CHARACTER[]{0xFF,0xFD,0,0,0};
}

namespace xy { namespace utf8 {

    decoder::decoder(void) throw() {
        state = ACCEPT_STATE;
        seen = 0U;
        found_error_ = false;
        memset(chars, 0, array::size(chars));
    }

    bool decoder::next_state(uint8_t byte, codepoint &codepoint) throw() {
        const uint32_t type = utf8d[byte];
        state = utf8d[256 + state + type];
        chars[seen] = byte;
        ++seen;
        if(ACCEPT_STATE == state) {
            codepoint.init_from_byte_array(chars);
            memset(chars, 0, array::size(chars));
            seen = 0;
            found_error_ = false;
            return true;
        } else if(SINK_STATE == state) {
            codepoint.init_from_byte_array(REPLACEMENT_CHARACTER);
            memset(chars, 0, array::size(chars));
            seen = 0;
            found_error_ = true;
            state = ACCEPT_STATE;
            return true;
        }

        return false;
    }

    bool decoder::is_in_use(void) const throw() {
        return 0 != seen;
    }

    bool decoder::found_error(void) const throw() {
        return found_error_;
    }

    void decoder::reset(void) throw() {
        state = ACCEPT_STATE;
        memset(chars, 0, array::size(chars));
    }

    size_t decoder::length(const char *str) throw() {
        decoder d;
        codepoint chr;
        size_t j(0);

        if(nullptr == str) {
            return j;
        }

        for(; '\0' != *str; ) {
            if(!d.next_state(*str++, chr)) {
                continue;
            }
            ++j;
        }
        return j;
    }
}}
