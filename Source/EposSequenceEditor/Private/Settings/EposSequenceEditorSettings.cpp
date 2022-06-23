// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/EposSequenceEditorSettings.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorSettings"

//---

FInfoBarSettings::FInfoBarSettings()
{
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::CurrentFrame_InStoryboard              , "current-frame.in-storyboard"                 , LOCTEXT( "infobar-pattern-keyword.current-frame.in-storyboard", "the current frame in the storyboard reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::CurrentFrame_InSequence                , "current-frame.in-sequence"                   , LOCTEXT( "infobar-pattern-keyword.current-frame.in-sequence", "the current frame in the current sequence reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::CurrentFrame_InSubsequence             , "current-frame.in-subsequence"                , LOCTEXT( "infobar-pattern-keyword.current-frame.in-subsequence", "the current frame in the current subsequence reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StartFrameOfStoryboard_InStoryboard    , "start-frame-of-storyboard.in-storyboard"     , LOCTEXT( "infobar-pattern-keyword.start-frame-of-storyboard.in-storyboard", "the start frame of the storyboard in the storyboard reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StopFrameOfStoryboard_InStoryboard     , "stop-frame-of-storyboard.in-storyboard"      , LOCTEXT( "infobar-pattern-keyword.stop-frame-of-storyboard.in-storyboard", "the stop frame of the storyboard in the storyboard reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StartFrameOfSequence_InStoryboard      , "start-frame-of-sequence.in-storyboard"       , LOCTEXT( "infobar-pattern-keyword.start-frame-of-sequence.in-storyboard", "the start frame of the current sequence in the storyboard reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StartFrameOfSequence_InSequence        , "start-frame-of-sequence.in-sequence"         , LOCTEXT( "infobar-pattern-keyword.start-frame-of-sequence.in-sequence", "the start frame of the current sequence in the current sequence reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StopFrameOfSequence_InStoryboard       , "stop-frame-of-sequence.in-storyboard"        , LOCTEXT( "infobar-pattern-keyword.stop-frame-of-sequence.in-storyboard", "the stop frame of the current sequence in the storyboard reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StopFrameOfSequence_InSequence         , "stop-frame-of-sequence.in-sequence"          , LOCTEXT( "infobar-pattern-keyword.stop-frame-of-sequence.in-sequence", "the stop frame of the current sequence in the current sequence reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StartFrameOfSubsequence_InStoryboard   , "start-frame-of-subsequence.in-storyboard"    , LOCTEXT( "infobar-pattern-keyword.start-frame-of-subsequence.in-storyboard", "the start frame of the current subsequence in the storyboard reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StartFrameOfSubsequence_InSequence     , "start-frame-of-subsequence.in-sequence"      , LOCTEXT( "infobar-pattern-keyword.start-frame-of-subsequence.in-sequence", "the start frame of the current subsequence in the current sequence reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StartFrameOfSubsequence_InSubSequence  , "start-frame-of-subsequence.in-subsequence"   , LOCTEXT( "infobar-pattern-keyword.start-frame-of-subsequence.in-subsequence", "the start frame of the current subsequence in the current subsequence reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StopFrameOfSubsequence_InStoryboard    , "stop-frame-of-subsequence.in-storyboard"     , LOCTEXT( "infobar-pattern-keyword.stop-frame-of-subsequence.in-storyboard", "the stop frame of the current subsequence in the storyboard reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StopFrameOfSubsequence_InSequence      , "stop-frame-of-subsequence.in-sequence"       , LOCTEXT( "infobar-pattern-keyword.stop-frame-of-subsequence.in-sequence", "the stop frame of the current subsequence in the current sequence reference" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::StopFrameOfSubsequence_InSubSequence   , "stop-frame-of-subsequence.in-subsequence"    , LOCTEXT( "infobar-pattern-keyword.stop-frame-of-subsequence.in-subsequence", "the stop frame of the current subsequence in the current subsequence reference" ) );

    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Storyboard_Duration                    , "storyboard.duration"                         , LOCTEXT( "infobar-pattern-keyword.storyboard.duration", "the total duration of the storyboard" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Storyboard_TotalSequences              , "storyboard.total-of-sequences"               , LOCTEXT( "infobar-pattern-keyword.storyboard.total-of-sequences", "the total number of boards/shots in the storyboard" ) );

    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Sequence_Duration                      , "sequence.duration"                           , LOCTEXT( "infobar-pattern-keyword.sequence.duration", "the duration of the current sequence" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Sequence_NumberOfSubsequences          , "sequence.number-of-subsequence"              , LOCTEXT( "infobar-pattern-keyword.sequence.number-of-subsequence", "the number of subsequence of the current sequence" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Sequence_Index                         , "sequence.index"                              , LOCTEXT( "infobar-pattern-keyword.sequence.index", "the index of the current sequence" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Sequence_Name                          , "sequence.name"                               , LOCTEXT( "infobar-pattern-keyword.sequence.name", "the name of the current sequence" ) );

    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Subsequence_Duration                   , "subsequence.duration"                        , LOCTEXT( "infobar-pattern-keyword.subsequence.duration", "the duration of the current subsequence" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Subsequence_Index                      , "subsequence.index"                           , LOCTEXT( "infobar-pattern-keyword.subsequence.index", "the index of the current subsequence" ) );
    mPatternKeywords.AddKeyword( EInfoBarPatternKeyword::Subsequence_Name                       , "subsequence.name"                            , LOCTEXT( "infobar-pattern-keyword.subsequence.name", "the name of the current subsequence" ) );

    // Default value of Pattern
    Pattern = FString::Printf( TEXT( "%s" ), *mPatternKeywords.mKeywordList[EInfoBarPatternKeyword::CurrentFrame_InStoryboard].mKeywordWithBraces );
    Pattern += TEXT( "\n" );
    Pattern += FString::Printf( TEXT( "%s (%s)" ), *mPatternKeywords.mKeywordList[EInfoBarPatternKeyword::Sequence_Name].mKeywordWithBraces
                                                 , *mPatternKeywords.mKeywordList[EInfoBarPatternKeyword::CurrentFrame_InSequence].mKeywordWithBraces );
    Pattern += TEXT( "\n" );
    Pattern += FString::Printf( TEXT( "%s (%s) [%s-%s]" ), *mPatternKeywords.mKeywordList[EInfoBarPatternKeyword::Subsequence_Name].mKeywordWithBraces
                                                         , *mPatternKeywords.mKeywordList[EInfoBarPatternKeyword::CurrentFrame_InSubsequence].mKeywordWithBraces
                                                         , *mPatternKeywords.mKeywordList[EInfoBarPatternKeyword::StartFrameOfSubsequence_InSequence].mKeywordWithBraces
                                                         , *mPatternKeywords.mKeywordList[EInfoBarPatternKeyword::StopFrameOfSubsequence_InSequence].mKeywordWithBraces );
}

//---

FName
UEposSequenceEditorSettings::GetContainerName() const
{
    return TEXT( "Editor" );
}

FName
UEposSequenceEditorSettings::GetCategoryName() const
{
    return TEXT( "Plugins" );
}

#undef LOCTEXT_NAMESPACE
