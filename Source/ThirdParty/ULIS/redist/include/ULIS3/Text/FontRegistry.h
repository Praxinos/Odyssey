// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         FontRegistry.h
* @author       Clement Berthaud
* @brief        This file provides the registry classes for the Font Registry tools.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include <map>
#include <vector>

ULIS3_NAMESPACE_BEGIN
class FFontRegistry;
class FFontFamilyKey;
class FFontStyleKey;

/////////////////////////////////////////////////////
/// @class      FFontStyleKey
/// @brief      The FFontStyleKey class provides a mean of storing and manipulating font families file paths.
class ULIS3_API FFontStyleKey
{
public:
    // Construction / Destruction
    ~FFontStyleKey();
    FFontStyleKey( const std::string& iFamilyName, const std::string& iStyleName, const std::string& iFontPath );

public:
    // Public API
    const std::string& GetFamilyName() const;
    const std::string& GetStyleName() const;
    const std::string& GetFontPath() const;

private:
    // Private Data Members
    std::string mFamilyName;
    std::string mStyleName;
    std::string mFontPath;
};

/////////////////////////////////////////////////////
/// @class      FFontFamilyKey
/// @brief      The FFontFamilyKey class provides a mean of storing and manipulating font families file paths.
class ULIS3_API FFontFamilyKey
{
public:
    // Construction / Destruction
    ~FFontFamilyKey();
    FFontFamilyKey( const std::string& iFamilyName );

public:
    // Public API
    void AddFontStyleKey( const std::string& iStyle, const FFontStyleKey& iFontStyleKey );
    int NumStyles() const;
    const std::map< std::string, FFontStyleKey >& GetStyles() const;
    const std::string& GetFamilyName() const;
    const FFontStyleKey* FuzzyFindFontStyleKey( const std::string& iName ) const;

private:
    // Private Data Members
    std::string mFamilyName;
    std::map< std::string, FFontStyleKey > mStyles;
};

/////////////////////////////////////////////////////
/// @class      FFontRegistry
/// @brief      The FFontRegistry class provides a mean of storing and manipulating font families file paths.
class ULIS3_API FFontRegistry
{
public:
    // Construction / Destruction
    ~FFontRegistry();
    FFontRegistry( const FFontEngine& iFontEngine );

public:
    // Public API
    void AddLookupPath( const std::string& iPath );
    void AddLookupPaths( const std::vector< std::string >& iPaths );
    void Load();
    int NumFamilies() const;
    int NumStyles() const;
    const std::map< std::string, FFontFamilyKey >& GetFamilies() const;
    const std::vector< std::string >& GetLookupPaths() const;
    const FFontFamilyKey* FuzzyFindFontFamily( const std::string& iName ) const;
    const FFontStyleKey* FuzzyFindFontStyleKey( const std::string& iFamily, const std::string& iStyle ) const;
    std::string FuzzyFindFontPath( const std::string& iFamily, const std::string& iStyle ) const;
    const FFontEngine& FontEngine() const;

private:
    // Private Data Members
    std::vector< std::string > mLookupPaths;
    std::map< std::string, FFontFamilyKey > mFamilies;
    const FFontEngine& mFontEngine;
};

ULIS3_NAMESPACE_END

