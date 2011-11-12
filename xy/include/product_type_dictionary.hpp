/*
 * product_type_dictionary.hpp
 *
 *  Created on: Aug 18, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_PRODUCT_TYPE_DICTIONARY_HPP_
#define XY_PRODUCT_TYPE_DICTIONARY_HPP_

#include "xy/include/type.hpp"

namespace xy {

    /// represents a dictionary for looking up product types (tuples, records).
    /// this type of lookup is needed because product types being built can be
    /// structurally equivalent to other types, but this structural equivalence
    /// is complicated by recursive types.
    ///
    /// e.g.
    ///         let Foo := record {
    ///             &Foo foo;
    ///             Int32 id;
    ///         }
    ///
    ///         let Bar := record {
    ///             &Bar bar;
    ///             Int32 id;
    ///         }
    ///
    ///         let Baz := record {
    ///             &Foo baz;
    ///             Int32 id;
    ///         }
    ///
    /// are all structurally equivalent.
    ///
    /// this is further complicated when we have substructures, e.g.
    ///
    ///         let Foo := record {
    ///             record {
    ///                 &Foo foo;
    ///             } foo;
    ///             Int32 id;
    ///         }
    ///
    ///         let Bar := record {
    ///             record {
    ///                 &Bar bar;
    ///             } bar;
    ///             Int32 id;
    ///         }
    ///
    /// and even more complicated by mutually recursive structures, e.g.
    ///
    ///        let Foo, Bar := record {
    ///             record {
    ///                 &Bar foo;
    ///             } foo;
    ///             Int32 id;
    ///         }, record {
    ///             record {
    ///                 &Foo bar;
    ///             } bar;
    ///             Int32 id;
    ///         }
    ///
    class product_type_dictionary {
    private:



    public:

    };

}


#endif /* XY_PRODUCT_TYPE_DICTIONARY_HPP_ */
