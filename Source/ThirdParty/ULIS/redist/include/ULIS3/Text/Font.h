// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Font.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FFont class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FFont
/// @brief      The FFont class provides a thin wrapper around the freetype face type.
class ULIS3_API FFont
{
public:
    // Construction / Destruction
    ~FFont();
    FFont( const FFontRegistry& iFontRegistry, const std::string& iFamily = "Arial", const std::string& iStyle = "Regular" );
    FFont( FFont& iOther );

public:
    // Public API
    void* Handle() const;
    const FFontEngine& FontEngine() const;
    const FFontRegistry& FontRegistry() const;
private:
    // Private Data Members
    void* mHandle;
    const FFontEngine& mFontEngine;
    const FFontRegistry& mFontRegistry;
};
ULIS3_NAMESPACE_END

