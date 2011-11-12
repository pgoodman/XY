/*
 * refine_equiv_relation.hpp
 *
 *  Created on: Oct 13, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_REFINE_EQUIV_RELATION_HPP_
#define XY_REFINE_EQUIV_RELATION_HPP_

#include <cassert>

namespace xy { namespace support {

    template <typename T, typename Init, typename Equiv>
    class refine_equiv_relation;

    /// gets the status of something in the equivalence relation
    template <typename T>
    class refine_get {
    private:

        template <typename, typename, typename>
        friend class refine_equiv_relation;

        T *items_x;
        T *items_y;
        bool *matrix;
        size_t num_x;

    public:

        refine_get(void) throw()
            : items_x(nullptr)
            , items_y(nullptr)
            , matrix(nullptr)
            , num_x(0)
        { }

        ~refine_get(void) throw() {
            items_x = nullptr;
            items_y = nullptr;
            matrix = nullptr;
        }

        bool &operator()(T &item_x, T &item_y) throw() {
            const size_t x(static_cast<size_t>(&item_x - items_x));
            const size_t y(static_cast<size_t>(&item_y - items_y));
            return matrix[(y * num_x) + x];
        }
    };

    /// refine an equivalence relation
    template <typename T, typename Init, typename Equiv>
    class refine_equiv_relation {
    private:

        bool *matrix;
        size_t num_allocated_slots;
        Init &initialize;
        Equiv &equal;
        refine_get<T> getter;

    public:

        refine_equiv_relation(Init &in, Equiv &eq) throw()
            : matrix(nullptr)
            , num_allocated_slots(0)
            , initialize(in)
            , equal(eq)
        { }

        ~refine_equiv_relation(void) throw() {
            if(nullptr != matrix) {
                delete [] matrix;
                matrix = nullptr;
            }
        }

        /// refine the equivalence relation
        refine_get<T> &refine(T *items_x, T *items_y, size_t num_x, size_t num_y) throw() {
            assert(num_x > 0);
            assert(num_y > 0);

            size_t max_dim(num_x < num_y ? num_y : num_x);
            size_t num_to_allocate(max_dim * max_dim);
            size_t num_to_set(num_x * num_y);
            T *item_x(items_x);
            const T *max_item_x(items_x + num_x);
            T *item_y(nullptr);
            const T *max_item_y(items_y + num_y);

            // see if we can re-use our existing matrix
            if(nullptr != matrix) {
                if(num_to_allocate > num_allocated_slots) {
                    num_allocated_slots = num_to_allocate;
                    delete [] matrix;
                    matrix = new bool[num_to_allocate];

                    assert(nullptr != matrix);
                }
            }

            // initialize the getter
            getter.items_x = items_x;
            getter.items_y = items_y;
            getter.matrix = matrix;
            getter.num_x = num_x;

            // default to nothing being equivalent
            memset(matrix, 0, num_to_set);

            for(; item_x < max_item_x; ++item_x) {
                item_y = items_y;
                for(; item_y < max_item_y; ++item_y) {
                    initialize(*item_x, *item_y, getter);
                }
            }

            bool made_progress(true);

            while(made_progress) {
                made_progress = false;
                item_x = items_x;
                for(size_t x(0); x < num_x; ++item_x, ++x) {

                    item_y = items_y;
                    for(size_t y(0); y < num_y; ++item_y, ++y) {
                        const size_t index = (y * num_x) + x;

                        if(matrix[index] && !equal(*item_x, *item_y, getter)) {
                            matrix[index] = false;
                            made_progress = true;
                        }
                    }
                }
            }

            return getter;
        }
    };

}}


#endif /* XY_REFINE_EQUIV_RELATION_HPP_ */
