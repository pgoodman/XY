/*
 * diagnostic_context.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include <cassert>

#include "xy/include/color.hpp"
#include "xy/include/cstring.hpp"
#include "xy/include/diagnostic_context.hpp"

namespace xy {

    diagnostic_context::diagnostic_context(const char *file_name_) throw()
        : file_name(cstring::copy(file_name_))
        , branch_counts()
        , active_branch()
        , branches()
    {
        speculate(1U);
    }


    diagnostic_context::~diagnostic_context(void) throw() {
        cstring::free(file_name);
        file_name = nullptr;

        for(size_t i(0); i < branches.size(); ++i) {
            delete branches[i];
            branches[i] = nullptr;
        }
    }

    const char *diagnostic_context::file(void) const throw() {
        return file_name;
    }

    void diagnostic_context::print_diagnostics(FILE *fp) const throw() {
        assert(!branches.empty());

        auto begin(branches[0]->begin());
        auto end(branches[0]->end());
        for(; begin != end; ) {
            const io::message *msg(*begin);

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

                begin->print(fp);
            }
        }
    }

    /// split into num_branches separate message queues so that we can explore
    /// different paths of computation without polluting the main line of
    /// messages
    void diagnostic_context::speculate(unsigned num_branches) throw() {
        assert(0 < num_branches);

        branch_counts.push_back(num_branches);
        active_branch.push_back(static_cast<unsigned>(branches.size()));

        for(unsigned i(0); i < num_branches; ++i) {
            branches.push_back(new io::message_queue);
        }
    }

    /// accept one of the speculative branches, i.e. subsume any messages from
    /// only that one branch.
    void diagnostic_context::accept(unsigned which_branch) throw() {
        assert(!branches.empty());

        const size_t num_queues(branches.size());
        const size_t num_leafs(branch_counts.back());
        const size_t good_branch((num_queues - num_leafs) + which_branch);

        io::message_queue *accepted(branches[good_branch]);

        branch_counts.pop_back();
        active_branch.pop_back();

        // kill off the bad branches
        for(size_t i(num_queues - num_leafs); i < num_queues; ++i) {
            if(good_branch != i) {
                delete branches[i];
            }
            branches[i] = nullptr;
        }

        branches.resize(num_queues - num_leafs, nullptr);

        // merge the accepted branch into the now active branch
        assert(!branches.empty());
        branches[active_branch.back()]->subsume(*accepted);

        // kill the accepted branch
        delete accepted;
        accepted = nullptr;
    }

    /// reject all of the speculative branches, i.e. subsume all of them!
    void diagnostic_context::reject(void) throw() {
        const size_t num_queues(branches.size());
        const size_t num_leafs(branch_counts.back());

        assert(0 < num_queues);
        assert(0 < num_leafs);

        branch_counts.pop_back();
        active_branch.pop_back();

        const bool can_subsume(!active_branch.empty());

        io::message_queue *prev_active_branch(nullptr);

        if(can_subsume) {
            prev_active_branch = branches[active_branch.back()];
        }

        for(size_t i(num_queues - num_leafs); i < num_queues; ++i) {
            if(can_subsume) {
                prev_active_branch->subsume(*(branches[i]));
            }

            delete branches[i];
            branches[i] = nullptr;
        }

        branches.resize(num_queues - num_leafs, nullptr);
    }

    /// reject all of the speculative branches, i.e. subsume all of them!
    void diagnostic_context::reset(void) throw() {
        while(!branches.empty()) {
            reject();
        }
        speculate(1);
    }


    /// check if we have any messages (in the top queue)
    bool diagnostic_context::has_message(void) const throw() {
        return branches[active_branch.back()]->has_message();
    }

    /// check if we have any messages of a particular type (in the top queue)
    bool diagnostic_context::has_message(io::message_type type) const throw() {
        return branches[active_branch.back()]->has_message(type);
    }
}
