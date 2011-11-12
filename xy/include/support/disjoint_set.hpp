/*
 * disjoint_set.hpp
 *
 *  Created on: Oct 13, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_DISJOINT_SET_HPP_
#define XY_DISJOINT_SET_HPP_

#include <stdint.h>

namespace xy { namespace support {

    template <typename tag>
    class disjoint_set {
    private:

        typedef disjoint_set<tag> self_type;

        uint16_t rank;
        self_type *parent;

    public:

        disjoint_set(void) throw()
            : rank(0)
            , parent(this)
        { }

        ~disjoint_set(void) throw() {
            parent = nullptr;
        }

        self_type *find(void) throw() {
            if(parent != this) {
                parent = parent->find();
            }
            return parent;
        }

        void join(self_type *that) throw() {
            self_type *me(find());
            self_type *you(that->find());

            if(rank < that->rank) {
                me->parent = you;
            } else if(rank > that->rank) {
                you->parent = me;
            } else {
                you->parent = me;
                me->rank += 1;
            }
        }
    };

}}

#endif /* XY_DISJOINT_SET_HPP_ */
