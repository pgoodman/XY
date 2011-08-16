/*
 * type_template.hpp
 *
 *  Created on: Aug 14, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_TYPE_TEMPLATE_HPP_
#define XY_TYPE_TEMPLATE_HPP_

namespace xy {

    /// represents a template for some type
    struct template_type : public type {
    public:

        enum {
            MAX_NUM_TYPE_PARAMS = 8U
        };

        type *param_types[MAX_NUM_TYPE_PARAMS];
        support::mapped_name param_names[MAX_NUM_TYPE_PARAMS];
        type *template_type;

        virtual ~template_type(void) throw();

        bool add_param(type *) throw();
        bool add_param(support::mapped_name) throw();
    };


}


#endif /* XY_TYPE_TEMPLATE_HPP_ */
