// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/NamingConventionSettings.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettings"

#define ADD_PATTERN_KEYWORD( ioMap, iKey, iTextKeyPrefix, iText )       \
{                                                                       \
    FString key = TEXT( iKey );                                         \
                                                                        \
    FNamingConventionPatternKeyword value;                              \
    value.mKeywordWithBraces = FString::Printf( TEXT( "{%s}" ), *key ); \
    value.mHelp = LOCTEXT( iTextKeyPrefix "." iKey, iText );            \
    ioMap.Add( key, value );                                            \
}

//---

FNamingConventionPlane::FNamingConventionPlane()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, "plane-index",    "plane-pattern-keyword", "an incremental index" );
    //ADD_PATTERN_KEYWORD( PatternKeywords, "camera-name",    "plane-pattern-keyword", "the name of the camera (won't update if camera name changes)" );
    //ADD_PATTERN_KEYWORD( PatternKeywords, "shot-name",      "plane-pattern-keyword", "the name of the shot (won't update if shot name changes)" );
}

FNamingConventionCamera::FNamingConventionCamera()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, "camera-index",   "camera-pattern-keyword", "an incremental index" );
    //ADD_PATTERN_KEYWORD( PatternKeywords, "shot-name",      "camera-pattern-keyword", "the name of the shot (won't update if shot name changes)" );
}

FNamingConventionShot::FNamingConventionShot()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, "shot-index",             "shot-pattern-keyword", "an incremental index" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "take-index",             "shot-pattern-keyword", "an incremental index for take (not used)" );

    ADD_PATTERN_KEYWORD( PatternKeywords, "studio-name",            "shot-pattern-keyword", "the full studio name" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "studio-acronym",         "shot-pattern-keyword", "the studio name acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "license-name",           "shot-pattern-keyword", "the full license name" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "license-acronym",        "shot-pattern-keyword", "the license name acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-name",        "shot-pattern-keyword", "the full production title" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-acronym",     "shot-pattern-keyword", "the production title acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "season",                 "shot-pattern-keyword", "the season number" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "episode",                "shot-pattern-keyword", "the episode number" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "part",                   "shot-pattern-keyword", "the part (A, B, C, ...)" );
    //ADD_PATTERN_KEYWORD( PatternKeywords, "department-name",        "shot-pattern-keyword", "the full department name" );
    //ADD_PATTERN_KEYWORD( PatternKeywords, "department-acronym",     "shot-pattern-keyword", "the department name acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "initials",               "shot-pattern-keyword", "some initials" );
}

FNamingConventionBoard::FNamingConventionBoard()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, "board-index",            "board-pattern-keyword", "an incremental index" );

    ADD_PATTERN_KEYWORD( PatternKeywords, "studio-name",            "board-pattern-keyword", "the full studio name" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "studio-acronym",         "board-pattern-keyword", "the studio name acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "license-name",           "board-pattern-keyword", "the full license name" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "license-acronym",        "board-pattern-keyword", "the license name acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-name",        "board-pattern-keyword", "the full production title" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-acronym",     "board-pattern-keyword", "the production title acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "season",                 "board-pattern-keyword", "the season number" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "episode",                "board-pattern-keyword", "the episode number" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "part",                   "board-pattern-keyword", "the part (A, B, C, ...)" );
    //ADD_PATTERN_KEYWORD( PatternKeywords, "department-name",        "board-pattern-keyword", "the full department name" );
    //ADD_PATTERN_KEYWORD( PatternKeywords, "department-acronym",     "board-pattern-keyword", "the department name acronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "initials",               "board-pattern-keyword", "some initials" );
}

//---

FName
UNamingConventionSettings::GetContainerName() const
{
    return TEXT( "Editor" );
}

FName
UNamingConventionSettings::GetCategoryName() const
{
    return TEXT( "Plugins" );
}

#undef LOCTEXT_NAMESPACE
