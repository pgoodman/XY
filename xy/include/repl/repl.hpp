/*
 * repl.hpp
 *
 *  Created on: Nov 29, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_REPL_HPP_
#define XY_REPL_HPP_

namespace xy { namespace repl {

    char *init(void) throw();
    void exit(void) throw();
    bool check(void) throw();

    void wait(void) throw();
    void accept(void) throw();
    bool should_wait(void) throw();

    namespace read {
        /// return back to the REPL
        void yield(void) throw();
    }

    namespace eval {
        /// return back to the REPL
        void yield(void) throw();
    }

}}

#endif /* XY_REPL_HPP_ */
