// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportImageSequenceSettings.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceSettings"

//---

FExportImageSequenceOptions::FExportImageSequenceOptions()
{
    mPatternKeywords.AddKeyword( EExportImageSequencePatternKeyword::PanelIndex     , "panel-index"     , LOCTEXT( "export-image-sequence-pattern-keyword.panel-index", "the index of the panel in the panel list (1, 2, 3, ...)" ) );
    mPatternKeywords.AddKeyword( EExportImageSequencePatternKeyword::StoryboardName , "root-board-name" , LOCTEXT( "export-image-sequence-pattern-keyword.root-board-name", "the root board name" ) );
    mPatternKeywords.AddKeyword( EExportImageSequencePatternKeyword::BoardIndex     , "board-index"     , LOCTEXT( "export-image-sequence-pattern-keyword.board-index", "the index which represents the board (only in the first level)" ) );
    mPatternKeywords.AddKeyword( EExportImageSequencePatternKeyword::ShotIndex      , "shot-index"      , LOCTEXT( "export-image-sequence-pattern-keyword.shot-index", "the index which represents the shot" ) );
    mPatternKeywords.AddKeyword( EExportImageSequencePatternKeyword::PanelFrame     , "panel-frame"     , LOCTEXT( "export-image-sequence-pattern-keyword.panel-frame", "the frame which represents the panel" ) );

    Pattern = FString::Printf( TEXT( "%s-%s-seq%s-sh%s-p%s" )
                               , *mPatternKeywords.mKeywordList[EExportImageSequencePatternKeyword::PanelIndex].mKeywordWithBraces
                               , *mPatternKeywords.mKeywordList[EExportImageSequencePatternKeyword::StoryboardName].mKeywordWithBraces
                               , *mPatternKeywords.mKeywordList[EExportImageSequencePatternKeyword::BoardIndex].mKeywordWithBraces
                               , *mPatternKeywords.mKeywordList[EExportImageSequencePatternKeyword::ShotIndex].mKeywordWithBraces
                               , *mPatternKeywords.mKeywordList[EExportImageSequencePatternKeyword::PanelFrame].mKeywordWithBraces
                               );
}

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
