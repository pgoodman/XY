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

    void wait(void) throw();
    void accept(void) throw();
    bool can_accept(void) throw();

}}

#endif /* XY_REPL_HPP_ */
