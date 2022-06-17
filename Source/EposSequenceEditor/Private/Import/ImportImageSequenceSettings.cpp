// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImportImageSequenceSettings.h"

#define LOCTEXT_NAMESPACE "ImportImageSequenceSettings"

//---

#define ADD_PATTERN_KEYWORD( ioMap, iId, iKey, iTextKeyPrefix, iText )  \
{                                                                       \
    FString key = TEXT( iKey );                                         \
                                                                        \
    FImportImageSequencePatternKeyword value;                           \
    value.mKeywordId = iId;                                             \
    value.mKeywordWithBraces = FString::Printf( TEXT( "{%s}" ), *key ); \
    value.mHelp = LOCTEXT( iTextKeyPrefix "." iKey, iText );            \
                                                                        \
    ioMap.Add( iId, value );                                            \
}

//---

FImportImageSequenceOptions::FImportImageSequenceOptions()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, EImportImageSequencePatternKeyword::BoardId, "sequence", "image-sequence-pattern-keyword", "the id which represents the board" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EImportImageSequencePatternKeyword::ShotId, "shot", "image-sequence-pattern-keyword", "the id which represents the shot" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EImportImageSequencePatternKeyword::FrameId, "panel", "image-sequence-pattern-keyword", "the id which represents the frame (drawing)" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EImportImageSequencePatternKeyword::Duration, "duration", "image-sequence-pattern-keyword", "the number which represents the duration of the frame (drawing)" );

    // No default pattern value, it's too file dependent
}

#undef ADD_PATTERN_KEYWORD

//---

#undef LOCTEXT_NAMESPACE
