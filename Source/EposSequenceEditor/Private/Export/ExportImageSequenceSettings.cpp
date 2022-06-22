// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportImageSequenceSettings.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceSettings"

//---

#define ADD_PATTERN_KEYWORD( ioMap, iId, iKey, iTextKeyPrefix, iText )  \
{                                                                       \
    FString key = TEXT( iKey );                                         \
                                                                        \
    FExportImageSequencePatternKeyword value;                           \
    value.mKeywordId = iId;                                             \
    value.mKeywordWithBraces = FString::Printf( TEXT( "{%s}" ), *key ); \
    value.mHelp = LOCTEXT( iTextKeyPrefix "." iKey, iText );            \
                                                                        \
    ioMap.Add( iId, value );                                            \
}

//---

FExportImageSequenceOptions::FExportImageSequenceOptions()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, EExportImageSequencePatternKeyword::PanelIndex, "panel-index", "export-image-sequence-pattern-keyword", "the index of the panel in the panel list (1, 2, 3, ...)" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EExportImageSequencePatternKeyword::StoryboardName, "root-board-name", "export-image-sequence-pattern-keyword", "the root board name" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EExportImageSequencePatternKeyword::BoardIndex, "board-index", "export-image-sequence-pattern-keyword", "the index which represents the board (only in the first level)" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EExportImageSequencePatternKeyword::ShotIndex, "shot-index", "export-image-sequence-pattern-keyword", "the index which represents the shot" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EExportImageSequencePatternKeyword::PanelFrame, "panel-frame", "export-image-sequence-pattern-keyword", "the frame which represents the panel" );

    Pattern = FString::Printf( TEXT( "%s-%s-seq%s-sh%s-p%s" )
                               , *PatternKeywords[EExportImageSequencePatternKeyword::PanelIndex].mKeywordWithBraces
                               , *PatternKeywords[EExportImageSequencePatternKeyword::StoryboardName].mKeywordWithBraces
                               , *PatternKeywords[EExportImageSequencePatternKeyword::BoardIndex].mKeywordWithBraces
                               , *PatternKeywords[EExportImageSequencePatternKeyword::ShotIndex].mKeywordWithBraces
                               , *PatternKeywords[EExportImageSequencePatternKeyword::PanelFrame].mKeywordWithBraces
                               );
}

#undef ADD_PATTERN_KEYWORD

//---
//---
//---

float
UExportImageSequenceUISettings::GetThumbnailScaleMultiplier() const
{
    return mThumbnailScaleMultiplier;
}

void
UExportImageSequenceUISettings::SetThumbnailScaleMultiplier( float iMultiplier )
{
    mThumbnailScaleMultiplier = iMultiplier;
    SaveConfig();
}

//---

#undef LOCTEXT_NAMESPACE
