/***************************************************************************************************
 * Copyright 2020 NVIDIA Corporation. All rights reserved.
 **************************************************************************************************/
/// \file
/// \brief      API component that gives access to the MDL compatibility API.

#ifndef MI_NEURAYLIB_IMDL_COMPATIBILITY_API_H
#define MI_NEURAYLIB_IMDL_COMPATIBILITY_API_H

#include <mi/base/interface_declare.h>
#include <mi/base/enums.h>

namespace mi {

class IArray;

namespace neuraylib {

class IMdl_execution_context;

/** \addtogroup mi_neuray_mdl_types
@{
*/

/// Provides access to various functions to inspect the compatibility between MDL modules
/// and archives.
class IMdl_compatibility_api : public
    base::Interface_declare<0x5f094722,0xfe19,0x4372,0x92,0xcf,0x5d,0x9f,0x57,0xa7,0x6,0x98>
{
public:
    /// Compares one original module with an replacement module for compatibility.
    ///
    /// \param[in]  module_name     The absolute name of the original module.
    /// \param[in]  repl_file_name  Path to the replacement module.
    /// \param[in]  search_paths    An optional array of additional search paths to consider for
    ///                             repl_file_name.
    /// \param[out] context         An execution context. If non-null, the context can be queried
    ///                             for messages after the comparison operation has finished.
    ///
    /// \return
    ///                             -  0: The modules are compatible.
    ///                             - -1: Invalid parameters (NULL pointer).
    ///                             - -2: An error occurred during module comparison.
    ///                                   Please check the context for details.
    virtual Sint32 compare_modules(
        const char*             module_name,
        const char*             repl_file_name,
        const IArray*           search_paths,
        IMdl_execution_context* context) const = 0;
        
    /// Compares two archives for compatibility.
    ///
    /// \param[in]  archive_fname1  Path to first archive.
    /// \param[in]  archive_fname2  Path to second archive.
    /// \param[in]  search_paths    An optional array of additional search paths to consider for
    ///                             archive_fname2.
    /// \param[out] context         An execution context. If non-null, the context can be queried
    ///                             for messages after the comparison operation has finished.
    ///
    /// \return
    ///                             -  0: The archives are compatible.
    ///                             - -1: Invalid parameters (NULL pointer).
    ///                             - -2: An error occurred during archive comparison.
    ///                                   Please check the context for details.
    virtual Sint32 compare_archives(
        const char*             archive_fname1,
        const char*             archive_fname2,
        const IArray*           search_paths,
        IMdl_execution_context* context) const = 0;
};

/*@}*/ // end group mi_neuray_mdl_types

} // namespace neuraylib
} // namespace mi

#endif // MI_NEURAYLIB_IMDL_COMPATIBILITY_API_H
