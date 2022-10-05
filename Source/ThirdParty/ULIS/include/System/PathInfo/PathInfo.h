// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         PathInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FPathInfo tools.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "String/String.h"

ULIS_NAMESPACE_BEGIN
/// @class      FPathInfo
/// @brief      The FPathInfo class provides a mean of fetching informations
///             about the environment paths, cross platform
class ULIS_API FPathInfo
{
private:
    ~FPathInfo() = delete;
    FPathInfo() = delete;
    FPathInfo( const FPathInfo& ) = delete;
    FPathInfo( FPathInfo&& ) = delete;

public:
    static void FontPaths( TArray< FString >& oPaths );
    static FString AppDataPath();
    static void DiskPaths( TArray< FString >& oPaths );
    static FString HomePath();
};

ULIS_NAMESPACE_END

