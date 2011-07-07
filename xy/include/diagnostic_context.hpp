/*
 * diagnostic_context.hpp
 *
 *  Created on: Jul 3, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_DIAGNOSTIC_CONTEXT_HPP_
#define XY_DIAGNOSTIC_CONTEXT_HPP_

#include <cstdio>
#include <vector>

#include "xy/include/io/message.hpp"

namespace xy {

    class diagnostic_context {
    private:

        std::vector<char *> file_names;

    public:

        io::message_queue diag;

        diagnostic_context(void) throw();
        ~diagnostic_context(void) throw();

        void push_file(const char *file_name) throw();

        void pop_file(void) throw();

        const char *top_file(void) const throw();

        void print_diagnostics(FILE *fp) const throw();
    };

}

#endif /* XY_DIAGNOSTIC_CONTEXT_HPP_ */
