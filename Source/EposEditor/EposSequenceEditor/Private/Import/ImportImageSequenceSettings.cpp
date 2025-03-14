// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImportImageSequenceSettings.h"

#define LOCTEXT_NAMESPACE "ImportImageSequenceSettings"

//---

static FPatternKeywordList sgImportImageSequencePatternKeywordList;

const FPatternKeywordList& GetImportImageSequencePatternKeywordList()
{
    if( !sgImportImageSequencePatternKeywordList.mKeywordList.Num() )
    {
        sgImportImageSequencePatternKeywordList.AddKeyword( EImportImageSequencePatternKeyword::BoardId    , "board"       , LOCTEXT( "image-sequence-pattern-keyword.board", "the id which represents the board" ) );
        sgImportImageSequencePatternKeywordList.AddKeyword( EImportImageSequencePatternKeyword::ShotId     , "shot"        , LOCTEXT( "image-sequence-pattern-keyword.shot", "the id which represents the shot" ) );
        sgImportImageSequencePatternKeywordList.AddKeyword( EImportImageSequencePatternKeyword::PanelId    , "panel"       , LOCTEXT( "image-sequence-pattern-keyword.panel", "the id which represents the panel" ) );
        sgImportImageSequencePatternKeywordList.AddKeyword( EImportImageSequencePatternKeyword::Duration   , "duration"    , LOCTEXT( "image-sequence-pattern-keyword.duration", "the duration of the panel" ) );
    }

    return sgImportImageSequencePatternKeywordList;
}

//---

FImportImageSequenceOptions::FImportImageSequenceOptions()
{
    mPatternKeywordLists.AddKeywordList( &GetImportImageSequencePatternKeywordList() );

    // No default pattern value, it's too file dependent
}

//---
//---
//---

float
UImportImageSequenceUISettings::GetThumbnailScaleMultiplier() const
{
    return mThumbnailScaleMultiplier;
}

void
UImportImageSequenceUISettings::SetThumbnailScaleMultiplier( float iMultiplier )
{
    mThumbnailScaleMultiplier = iMultiplier;
    SaveConfig();
}

//---

#undef LOCTEXT_NAMESPACE
