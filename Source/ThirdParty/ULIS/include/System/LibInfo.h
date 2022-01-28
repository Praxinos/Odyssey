// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LibInfo.h
* @author       Clement Berthaud
* @brief        This file provides declarations for API and ABI infos.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "String/String.h"

ULIS_NAMESPACE_BEGIN
/// @class      FLibInfo
/// @brief      The FLibInfo class provides a mean to access information
///             about the library.
/// @details    FLibInfo allows the user to query informations such as
///             the version of the library, the branch or commit it was
///             compiled from, as well as other informations such as the
///             compiler that was used and its version.
class ULIS_API FLibInfo
{
private:
    ~FLibInfo() = delete;
    FLibInfo() = delete;
    FLibInfo( const FLibInfo& ) = delete;
    FLibInfo( FLibInfo&& ) = delete;

public:
    /*! Retrieve the full version string "major.minor.patch". */
    static FString VersionString();

    /*! Retrieve the major version string. */
    static FString VersionMajorString();

    /*! Retrieve the minor version string. */
    static FString VersionMinorString();

    /*! Retrieve the patch version string. */
    static FString VersionPatchString();

    /*! Retrieve the major version number. */
    static uint64 VersionMajor();

    /*! Retrieve the minor version number. */
    static uint64 VersionMinor();

    /*! Retrieve the patch version number. */
    static uint64 VersionPatch();

    /*! Retrieve the built configuration ( Debug, Release, RelWithDebInfo ). */
    static FString ConfigurationString();

    /*! Retrieve the compilation date and time e.g: "Aug 15 2020, 15:12:04". */
    static FString CompilationTimeString();

    /*! Retrieve the compiler name string */
    static FString CompilerNameString();

    /*! Retrieve the compiler version string */
    static FString CompilerVersionString();

    /*! Retrieve the full information string */
    static FString CompilerInformationString();

    /*! Retrieve wether the library was compiled for x64 target */
    static bool CompiledForx64();

    /*! Retrieve wether the library was compiled with AVX2 support */
    static bool CompiledWithAVX2();

    /*! Retrieve wether the library was compiled with SSE42 support */
    static bool CompiledWithSSE42();

    /*! Retrieve wether the library was built as a shared library ( true: shared, false: static ). */
    static bool BuiltAsSharedLibrary();

    /*! Retrieve the git branch from where it was compiled if available */
    static FString BranchNameString();

    /*! Retrieve the git commit hash from where it was compiled if available */
    static FString CommitHashString();

    /*! Retrieve the git commit hash from where it was compiled if available */
    static FString CommitAbbreviatedHashString();

    /*! Retrieve wether the library was built with MT. */
    static bool CompiledWithMT();

    /*! Retrieve the full library identification string */
    static FString LibraryInformationString();
};

ULIS_NAMESPACE_END

