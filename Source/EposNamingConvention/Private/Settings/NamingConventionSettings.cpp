// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Settings/NamingConventionSettings.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettings"

#define ADD_PATTERN_KEYWORD( ioMap, iKey, iTextKeyPrefix, iText )        \
{                                                                       \
    FString key = TEXT( iKey );                                         \
                                                                        \
    FNamingConventionPatternKeyword value;                              \
    value.mKeywordWithBraces = FString::Printf( TEXT( "{%s}" ), *key ); \
    value.mHelp = LOCTEXT( iTextKeyPrefix ## "." ## iKey, iText );      \
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
    ADD_PATTERN_KEYWORD( PatternKeywords, "studio-accronym",        "shot-pattern-keyword", "the studio name accronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-name",        "shot-pattern-keyword", "the full production title" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-accronym",    "shot-pattern-keyword", "the production title accronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "initials",               "shot-pattern-keyword", "some initials" );

}

FNamingConventionBoard::FNamingConventionBoard()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, "board-index",            "board-pattern-keyword", "an incremental index" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "studio-name",            "board-pattern-keyword", "the full studio name" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "studio-accronym",        "board-pattern-keyword", "the studio name accronym" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-name",        "board-pattern-keyword", "the full production title" );
    ADD_PATTERN_KEYWORD( PatternKeywords, "production-accronym",    "board-pattern-keyword", "the production title accronym" );
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
