/***************************************************************************************************
 * Copyright 2020 NVIDIA Corporation. All rights reserved.
 **************************************************************************************************/
/// \file
/// \brief Writers, used by exporters.

#ifndef MI_NEURAYLIB_IWRITER_H
#define MI_NEURAYLIB_IWRITER_H

#include <mi/neuraylib/ireader_writer_base.h>

namespace mi {

namespace neuraylib {

/** \if MDL_SDK_API \addtogroup mi_neuray_mdl_sdk_misc
    \else \addtogroup mi_neuray_impexp
    \endif
@{
*/

/// A writer supports binary block writes and string-oriented line writes that accept a
/// zero-terminated string as argument.
class IWriter :
    public base::Interface_declare<0x0e6ecfbc,0x78c3,0x4082,0xba,0x51,0xa3,0x60,0xbb,0x1d,0x6f,0xc0,
                                   neuraylib::IReader_writer_base>
{
public:
    /// Writes a number of bytes to the stream.
    ///
    /// \param buffer  The buffer from where to read the data.
    /// \param size    The number of bytes to write.
    /// \return        The number of bytes written, or -1 in case of errors.
    virtual Sint64 write( const char* buffer, Sint64 size) = 0;

    /// Writes a zero-terminated string to the stream.
    ///
    /// \param str     The string to be written. Note that the writer does not add an extra
    ///                newline character at the end.
    /// \return        \c true in case of success, or \c false in case of errors.
    virtual bool writeline( const char* str) = 0;

    /// Flushes all buffered output to the stream.
    /// \return        \c true in case of success, or \c false in case of errors.
    virtual bool flush() = 0;
};

/*@}*/ // end group mi_neuray_impexp

} // namespace neuraylib

} // namespace mi

#endif // MI_NEURAYLIB_IWRITER_H
