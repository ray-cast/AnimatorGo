/***************************************************************************************************
 * Copyright 2020 NVIDIA Corporation. All rights reserved.
 **************************************************************************************************/
/// \file
/// \brief      Scene element Attribute_container

#ifndef MI_NEURAYLIB_IATTRIBUTE_CONTAINER_H
#define MI_NEURAYLIB_IATTRIBUTE_CONTAINER_H

#include <mi/neuraylib/iscene_element.h>

namespace mi {

namespace neuraylib {

/** \addtogroup mi_neuray_misc
@{
*/

/// An attribute container is a database element that stores attributes
/// and no other data.
///
/// \see #mi::neuraylib::IAttribute_set
class IAttribute_container :
    public base::Interface_declare<0x2afa6f8b,0x40a7,0x415f,0xac,0x7e,0x4d,0x13,0x2f,0x66,0x7e,0x66,
                                   neuraylib::IScene_element>
{
public:
};

/*@}*/ // end group mi_neuray_misc

} // namespace neuraylib

} // namespace mi

#endif // MI_NEURAYLIB_IATTRIBUTE_CONTAINER_H
