// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportImageSequenceSettings.h"

#include "Settings/NamingConventionSettings.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceSettings"

//---

static FPatternKeywordList sgExportImageSequencePatternKeywordList;

const FPatternKeywordList& GetExportImageSequencePatternKeywordList()
{
    if( !sgExportImageSequencePatternKeywordList.mKeywordList.Num() )
    {
        sgExportImageSequencePatternKeywordList.AddKeyword( EExportImageSequencePatternKeyword::PanelFrame     , "panel-frame"     , LOCTEXT( "export-image-sequence-pattern-keyword.panel-frame", "the frame which represents the panel" ) );
        sgExportImageSequencePatternKeywordList.AddKeyword( EExportImageSequencePatternKeyword::PanelIndex     , "panel-index"     , LOCTEXT( "export-image-sequence-pattern-keyword.panel-index", "the index of the panel in the panel list (1, 2, 3, ...)" ) );
        sgExportImageSequencePatternKeywordList.AddKeyword( EExportImageSequencePatternKeyword::StoryboardName , "root-board-name" , LOCTEXT( "export-image-sequence-pattern-keyword.root-board-name", "the root board name" ) );
    }

    return sgExportImageSequencePatternKeywordList;
}

//---

FExportImageSequenceOptions::FExportImageSequenceOptions()
{
    mPatternKeywordLists.AddKeywordList( &GetExportImageSequencePatternKeywordList() );
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionShotPatternKeywordList() );
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionBoardPatternKeywordList() );
    mPatternKeywordLists.AddKeywordList( &GetNamingConventionCommonPatternKeywordList() );

    Pattern = FString::Printf( TEXT( "%s-%s-seq%s-sh%s-p%s" )
                               , *mPatternKeywordLists.GetKeyword( EExportImageSequencePatternKeyword::PanelIndex ).mKeywordWithBraces
                               , *mPatternKeywordLists.GetKeyword( EExportImageSequencePatternKeyword::StoryboardName ).mKeywordWithBraces
                               , *mPatternKeywordLists.GetKeyword( ENamingConventionBoardPatternKeyword::BoardIndex ).mKeywordWithBraces
                               , *mPatternKeywordLists.GetKeyword( ENamingConventionShotPatternKeyword::ShotIndex ).mKeywordWithBraces
                               , *mPatternKeywordLists.GetKeyword( EExportImageSequencePatternKeyword::PanelFrame ).mKeywordWithBraces
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
