// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Font.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FFont class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
#pragma warning(push)
#pragma warning(disable : 4251) // Shut warning C4251 dll export of stl classes
/////////////////////////////////////////////////////
/// @class      FFont
/// @brief      The FFont class provides a thin wrapper around the freetype face type.
class ULIS_API FFont
{
public:
    // Construction / Destruction
    ~FFont();
    FFont( FFont&& ) = delete;
    FFont& operator=( FFont&& iOther ) = delete;
    FFont(
          const FFontEngine& iFontEngine
        , const std::string& iRequestedFamily
        , const std::string& iRequestedStyle
    );

    FFont(
          const FFontEngine& iFontEngine
        , const FFontStyleEntry* iRequestedEntry
    );

    FFont(
          const FFontEngine& iFontEngine
        , const std::string& iRequestedPath
    );

    FFont( const FFont& );

    FFont& operator=( const FFont& );

public:
    // Public API
    void* FontHandle() const;
    const FFontEngine& FontEngine() const;
    const std::string& Family() const;
    const std::string& Style() const;

public:
    static const FFont DefaultFont;

private:
    // Private Data Members
    void* mFontHandle;
    const FFontEngine& mFontEngine;
    std::string mFamily;
    std::string mStyle;
};
#pragma warning(pop)

ULIS_NAMESPACE_END

