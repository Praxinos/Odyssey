// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/EposSequenceEditorSettings.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorSettings"

//---

#define ADD_PATTERN_KEYWORD( ioMap, iId, iKey, iTextKeyPrefix, iText )  \
{                                                                       \
    FString key = TEXT( iKey );                                         \
                                                                        \
    FInfoBarPatternKeyword value;                                       \
    value.mKeywordId = iId;                                             \
    value.mKeywordWithBraces = FString::Printf( TEXT( "{%s}" ), *key ); \
    value.mHelp = LOCTEXT( iTextKeyPrefix "." iKey, iText );      \
                                                                        \
    ioMap.Add( iId, value );                                            \
}

//---

FInfoBarSettings::FInfoBarSettings()
{
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::CurrentFrame_InStoryboard,                "current-frame.in-storyboard",                  "infobar-pattern-keyword", "the current frame in the storyboard reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::CurrentFrame_InSequence,                  "current-frame.in-sequence",                    "infobar-pattern-keyword", "the current frame in the current sequence reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::CurrentFrame_InSubsequence,               "current-frame.in-subsequence",                 "infobar-pattern-keyword", "the current frame in the current subsequence reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StartFrameOfStoryboard_InStoryboard,      "start-frame-of-storyboard.in-storyboard",      "infobar-pattern-keyword", "the start frame of the storyboard in the storyboard reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StopFrameOfStoryboard_InStoryboard,       "stop-frame-of-storyboard.in-storyboard",       "infobar-pattern-keyword", "the stop frame of the storyboard in the storyboard reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StartFrameOfSequence_InStoryboard,        "start-frame-of-sequence.in-storyboard",        "infobar-pattern-keyword", "the start frame of the current sequence in the storyboard reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StartFrameOfSequence_InSequence,          "start-frame-of-sequence.in-sequence",          "infobar-pattern-keyword", "the start frame of the current sequence in the current sequence reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StopFrameOfSequence_InStoryboard,         "stop-frame-of-sequence.in-storyboard",         "infobar-pattern-keyword", "the stop frame of the current sequence in the storyboard reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StopFrameOfSequence_InSequence,           "stop-frame-of-sequence.in-sequence",           "infobar-pattern-keyword", "the stop frame of the current sequence in the current sequence reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StartFrameOfSubsequence_InStoryboard,     "start-frame-of-subsequence.in-storyboard",     "infobar-pattern-keyword", "the start frame of the current subsequence in the storyboard reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StartFrameOfSubsequence_InSequence,       "start-frame-of-subsequence.in-sequence",       "infobar-pattern-keyword", "the start frame of the current subsequence in the current sequence reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StartFrameOfSubsequence_InSubSequence,    "start-frame-of-subsequence.in-subsequence",    "infobar-pattern-keyword", "the start frame of the current subsequence in the current subsequence reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StopFrameOfSubsequence_InStoryboard,      "stop-frame-of-subsequence.in-storyboard",      "infobar-pattern-keyword", "the stop frame of the current subsequence in the storyboard reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StopFrameOfSubsequence_InSequence,        "stop-frame-of-subsequence.in-sequence",        "infobar-pattern-keyword", "the stop frame of the current subsequence in the current sequence reference" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::StopFrameOfSubsequence_InSubSequence,     "stop-frame-of-subsequence.in-subsequence",     "infobar-pattern-keyword", "the stop frame of the current subsequence in the current subsequence reference" );

    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Storyboard_Duration,              "storyboard.duration",              "infobar-pattern-keyword", "the total duration of the storyboard" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Storyboard_TotalSequences,        "storyboard.total-of-sequences",    "infobar-pattern-keyword", "the total number of boards/shots in the storyboard" );

    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Sequence_Duration,                "sequence.duration",                "infobar-pattern-keyword", "the duration of the current sequence" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Sequence_NumberOfSubsequences,    "sequence.number-of-subsequence",   "infobar-pattern-keyword", "the number of subsequence of the current sequence" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Sequence_Index,                   "sequence.index",                   "infobar-pattern-keyword", "the index of the current sequence" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Sequence_Name,                    "sequence.name",                    "infobar-pattern-keyword", "the name of the current sequence" );

    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Subsequence_Duration,             "subsequence.duration",             "infobar-pattern-keyword", "the duration of the current subsequence" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Subsequence_Index,                "subsequence.index",                "infobar-pattern-keyword", "the index of the current subsequence" );
    ADD_PATTERN_KEYWORD( PatternKeywords, EInfoBarPatternKeyword::Subsequence_Name,                 "subsequence.name",                 "infobar-pattern-keyword", "the name of the current subsequence" );

    // Default value of Pattern
    Pattern = FString::Printf( TEXT( "%s" ), *PatternKeywords[EInfoBarPatternKeyword::CurrentFrame_InStoryboard].mKeywordWithBraces );
    Pattern += TEXT( "\n" );
    Pattern += FString::Printf( TEXT( "%s (%s)" ), *PatternKeywords[EInfoBarPatternKeyword::Sequence_Name].mKeywordWithBraces
                                               , *PatternKeywords[EInfoBarPatternKeyword::CurrentFrame_InSequence].mKeywordWithBraces );
    Pattern += TEXT( "\n" );
    Pattern += FString::Printf( TEXT( "%s (%s) [%s-%s]" ), *PatternKeywords[EInfoBarPatternKeyword::Subsequence_Name].mKeywordWithBraces
                                                         , *PatternKeywords[EInfoBarPatternKeyword::CurrentFrame_InSubsequence].mKeywordWithBraces
                                                         , *PatternKeywords[EInfoBarPatternKeyword::StartFrameOfSubsequence_InSequence].mKeywordWithBraces
                                                         , *PatternKeywords[EInfoBarPatternKeyword::StopFrameOfSubsequence_InSequence].mKeywordWithBraces );
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
