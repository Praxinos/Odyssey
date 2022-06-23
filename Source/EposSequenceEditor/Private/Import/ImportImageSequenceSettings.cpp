// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImportImageSequenceSettings.h"

#define LOCTEXT_NAMESPACE "ImportImageSequenceSettings"

//---

FImportImageSequenceOptions::FImportImageSequenceOptions()
{
    mPatternKeywords.AddKeyword( EImportImageSequencePatternKeyword::BoardId    , "board"       , LOCTEXT( "image-sequence-pattern-keyword.board", "the id which represents the board" ) );
    mPatternKeywords.AddKeyword( EImportImageSequencePatternKeyword::ShotId     , "shot"        , LOCTEXT( "image-sequence-pattern-keyword.shot", "the id which represents the board" ) );
    mPatternKeywords.AddKeyword( EImportImageSequencePatternKeyword::PanelId    , "panel"       , LOCTEXT( "image-sequence-pattern-keyword.panel", "the id which represents the board" ) );
    mPatternKeywords.AddKeyword( EImportImageSequencePatternKeyword::Duration   , "duration"    , LOCTEXT( "image-sequence-pattern-keyword.duration", "the id which represents the board" ) );

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
