// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         FontEngine.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FFontEngine class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include <map>
#include <list>

ULIS_NAMESPACE_BEGIN
#pragma warning(push)
#pragma warning(disable : 4251) // Shut warning C4251 dll export of stl classes
/////////////////////////////////////////////////////
/// @class FFontStyleEntry
/// @brief The FFontStyleEntry class provides a mean of storing and manipulating font files entries along with their family, style and path.
class ULIS_API FFontStyleEntry
{
public:
    // Construction / Destruction
    ~FFontStyleEntry();
    FFontStyleEntry( const std::string& iFamily, const std::string& iStyle, const std::string& iPath );

public:
    // Public API
    const std::string& Family() const;
    const std::string& Style() const;
    const std::string& Path() const;

private:
    // Private Data Members
    std::string mFamily;
    std::string mStyle;
    std::string mPath;
};

/////////////////////////////////////////////////////
/// @class FFontFamilyEntry
/// @brief The FFontFamilyEntry class provides a mean of storing and manipulating font family entries.
class ULIS_API FFontFamilyEntry
{

public:
    // Construction / Destruction
    ~FFontFamilyEntry();
    FFontFamilyEntry( const std::string& iName );

public:
    // Public API
    void AddFontStyleKey( const std::string& iStyle, const FFontStyleEntry& iFontStyleKey );
    int StyleCount() const;
    const std::map< std::string, FFontStyleEntry >& Styles() const;
    const std::string& Family() const;
    const FFontStyleEntry* FuzzyFindFontStyleKey( const std::string& iStyle ) const;

private:
    // Private Data Members
    std::string mFamily;
    std::map< std::string, FFontStyleEntry > mStyles;
};

/////////////////////////////////////////////////////
/// @class      FFontEngine
/// @brief      The FFontEngine class provides a thin wrapper around the freetype
///             library. It also provides a registry to store and retrieve
///             discoverable font entries on the system.
class ULIS_API FFontEngine
{
public:
    // Construction / Destruction
    ~FFontEngine();
    FFontEngine();
    FFontEngine( const FFontEngine& ) = delete;
    FFontEngine( FFontEngine&& ) = delete;
    FFontEngine& operator=( const FFontEngine& ) = delete;
    FFontEngine& operator=( FFontEngine&& iOther ) = delete;

public:
    // Public API
    void* LibraryHandle() const;
    void AddLookupPath( const std::string& iPath );
    void AddLookupPaths( const std::list< std::string >& iPaths );
    void Refresh();
    int FamilyCount() const;
    const std::map< std::string, FFontFamilyEntry >& Records() const;
    const std::list< std::string >& LookupPaths() const;
    const FFontFamilyEntry* FuzzyFindFontFamily( const std::string& iName ) const;
    const FFontStyleEntry* FuzzyFindFontStyle( const std::string& iFamily, const std::string& iStyle ) const;
    std::string FuzzyFindFontPath( const std::string& iFamily, const std::string& iStyle ) const;

    static const FFontEngine GFontEngine;

private:
    // Private Data Members
    void* mLibraryHandle;
    std::list< std::string > mLookupPaths;
    std::map< std::string, FFontFamilyEntry > mRecords;
};
#pragma warning(pop)

ULIS_NAMESPACE_END

