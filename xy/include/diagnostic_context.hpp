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
#include "xy/include/io/line_highlight.hpp"
#include "xy/include/token.hpp"

namespace xy {

    /// stores context-specific diagnostic information.
    class diagnostic_context {
    private:

        const char *file_name;
        //io::message_queue diag;

        std::vector<unsigned> branch_counts;
        std::vector<unsigned> active_branch;
        std::vector<io::message_queue *> branches;

    public:

        template <typename ...arg_types>
        void report(io::message_id id, arg_types... args) throw() {
            (void) args;
            branches[active_branch.back()]->push(id, args...);
        }

        template <typename ...arg_types>
        void report_here(const token &here, io::message_id id, arg_types... args) throw() {
            (void) args;
            io::message_queue *queue(branches[active_branch.back()]);
            queue->push(id, args...);
            queue->push(io::c_file_line_col, file(), here.line(), here.column());
            queue->push(io::c_highlight, io::highlight_line(
                file(), here.line(), here.column(), here.end_column()
            ));
        }

        diagnostic_context(const char *file_name_) throw();
        ~diagnostic_context(void) throw();

        const char *file(void) const throw();

        void print_diagnostics(FILE *fp) const throw();

        void speculate(unsigned num_branches) throw();
        void accept(unsigned which_branch) throw();
        void reject(void) throw();

        /// check if we have any messages (in the top queue)
        bool has_message(void) const throw();

        /// check if we have any messages of a particular type (in the top queue)
        bool has_message(io::message_type) const throw();
    };

}

#endif /* XY_DIAGNOSTIC_CONTEXT_HPP_ */
