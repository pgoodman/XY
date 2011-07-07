/*
 * diagnostic_context.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include "xy/include/color.hpp"
#include "xy/include/cstring.hpp"
#include "xy/include/diagnostic_context.hpp"

namespace xy {

    diagnostic_context::diagnostic_context(void) throw()
        : file_names()
        , diag()
    {
        file_names.reserve(20);
    }

    diagnostic_context::~diagnostic_context(void) throw() {
        for(char *file_name : file_names) {
            cstring::free(file_name);
        }
    }

    void diagnostic_context::push_file(const char *file_name) throw() {
        file_names.push_back(cstring::copy(file_name));
    }

    void diagnostic_context::pop_file(void) throw() {
        const char *file_name(file_names.back());
        cstring::free(file_name);
        file_names.pop_back();
    }

    const char *diagnostic_context::top_file(void) const throw() {
        return file_names.back();
    }

    void diagnostic_context::print_diagnostics(FILE *fp) const throw() {
        auto begin(diag.begin());
        auto end(diag.end());
        for(; begin != end; ) {
            const io::message *msg{*begin};

            if(++begin != end && io::c_file_line_col == begin->id) {
                begin->print(fp);
                ++begin;
            }

            msg->print(fp);
            fprintf(fp, "\n\n");

            for(; begin != end; ++begin) {
                if(io::special != begin->type) {
                    break;
                }

                // todo
            }
        }
    }

}
