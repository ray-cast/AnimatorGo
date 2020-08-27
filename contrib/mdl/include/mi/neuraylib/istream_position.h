/***************************************************************************************************
 * Copyright 2020 NVIDIA Corporation. All rights reserved.
 **************************************************************************************************/
/// \file
/// \brief Position in a data stream.

#ifndef MI_NEURAYLIB_ISTREAM_POSITION_H
#define MI_NEURAYLIB_ISTREAM_POSITION_H

#include <mi/base/interface_declare.h>

namespace mi {

namespace neuraylib {

/** \if MDL_SDK_API \addtogroup mi_neuray_mdl_sdk_misc
    \else \addtogroup mi_neuray_impexp
    \endif
@{
*/

/// Represents the position in a data stream.
///
/// \see #mi::neuraylib::IReader_writer_base
class IStream_position :
    public base::Interface_declare<0xdbd2d643,0x7788,0x41fb,0xad,0xcd,0xad,0xbc,0x52,0x3f,0xf2,0x9f>
{
public:
    /// Indicates whether the stream position is valid.
    ///
    /// \return \c true, if the stream position is valid and can be used with
    ///         #mi::neuraylib::IReader_writer_base::seek_position(), and \c false otherwise.
    virtual bool is_valid() const = 0;
};

/*@}*/ // end group mi_neuray_impexp

} // namespace neuraylib

} // namespace mi

#endif // MI_NEURAYLIB_ISTREAM_POSITION_H
