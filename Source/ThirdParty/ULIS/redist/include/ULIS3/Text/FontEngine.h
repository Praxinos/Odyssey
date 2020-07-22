// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         FontEngine.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FFontEngine class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FFontEngine
/// @brief      The FFontEngine class provides a thin wrapper around the freetype library.
class ULIS3_API FFontEngine
{
public:
    // Construction / Destruction
    ~FFontEngine();
    FFontEngine();
    FFontEngine( const FFontEngine& ) = delete;

public:
    // Public API
    void* Handle() const;

private:
    // Private Data Members
    void* mHandle;
};
ULIS3_NAMESPACE_END

