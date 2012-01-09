/*
 * resolve_name.hpp
 *
 *  Created on: Jan 2, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_RESOLVE_NAMES_HPP_
#define XY_RESOLVE_NAMES_HPP_

#include "xy/include/front_end.hpp"

namespace xy { namespace pass {

    void resolve_names(diagnostic_context &, symbol_table &, type_system &, ast *) throw();

}}

#endif /* XY_RESOLVE_NAMES_HPP_ */
