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

    /// stores context-specific diagnostic information.
    class diagnostic_context {
    private:

        const char *file_name;

    public:

        io::message_queue diag;

        diagnostic_context(const char *file_name_) throw();
        ~diagnostic_context(void) throw();

        const char *file(void) const throw();

        void print_diagnostics(FILE *fp) const throw();
    };

}

#endif /* XY_DIAGNOSTIC_CONTEXT_HPP_ */
