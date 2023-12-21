// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "PatternKeywordList.h"

//---

TArray<FPatternKeyword>
FPatternKeywordList::GetKeywordList() const
{
    TArray<FPatternKeyword> keywords;
    mKeywordList.GenerateValueArray( keywords );

    return keywords;
}

void
FPatternKeywordList::AddKeyword( uint32 iId, const FString& iKeyWithoutBraces, const FText& iHelpText )
{
    FPatternKeyword keyword;
    keyword.mKeywordId = iId;
    keyword.mKeyword = iKeyWithoutBraces;
    keyword.mKeywordWithBraces = FString::Printf( TEXT( "{%s}" ), *iKeyWithoutBraces );
    keyword.mHelp = iHelpText;

    mKeywordList.Add( iId, keyword );
}

bool
FPatternKeywordList::IsValidPattern( const FString& iPattern )
{
    FRegexPattern key_pattern( TEXT( "\\{[^}]*\\}" ) ); // Mandatory as FRegexMatcher() takes a const reference
    FRegexMatcher matcher( key_pattern, iPattern );

    // This loop is to validate all {...} patterns
    while( matcher.FindNext() )
    {
        int32 full_begin = matcher.GetMatchBeginning();
        int32 full_end = matcher.GetMatchEnding();
        FTextRange full_range( full_begin, full_end );
        FString full_string = iPattern.Mid( full_range.BeginIndex, full_range.Len() );

        auto FindKeyword = [keyword = full_string]( const TTuple<uint32, FPatternKeyword>& iKeyword )
        {
            return iKeyword.Value.mKeywordWithBraces == keyword;
        };
        if( !mKeywordList.FilterByPredicate( FindKeyword ).Num() )
            return false;
    }

    // This loop is to check if a valid pattern appears ONLY 1 time
    for( auto pair : mKeywordList )
    {
        FString valid_pattern = pair.Value.mKeywordWithBraces;

        int32 start_index = iPattern.Find( valid_pattern );
        // If the current valid pattern is NOT found, that's ok and let's check the next pattern
        if( start_index == INDEX_NONE )
            continue;

        // Here we find the first occurance of the current valid pattern

        // Try to find the same pattern another time
        start_index = iPattern.Find( valid_pattern, ESearchCase::IgnoreCase, ESearchDir::FromStart, start_index + 1 );
        // If the current valid pattern is found again, it's wrong because a valid pattern should only appear 1 time, so return false
        if( start_index != INDEX_NONE )
            return false;
    }

    return true;
};

//---

void
FPatternKeywordLists::AddKeywordList( const FPatternKeywordList* iPatternKeywordList )
{
    mKeywordLists.Add( iPatternKeywordList );
}

const FPatternKeyword&
FPatternKeywordLists::GetKeyword( uint32 iId ) const
{
    for( auto keyword_list : mKeywordLists )
    {
        if( !keyword_list->mKeywordList.Contains( iId ) )
            continue;

        return keyword_list->mKeywordList[iId];
    }

    static FPatternKeyword sKeyword;
    return sKeyword;
}

bool
FPatternKeywordLists::IsValidPattern( const FString& iPattern )
{
    FRegexPattern key_pattern( TEXT( "\\{[^}]*\\}" ) ); // Mandatory as FRegexMatcher() takes a const reference
    FRegexMatcher matcher( key_pattern, iPattern );

    // This loop is to validate all {...} patterns
    while( matcher.FindNext() )
    {
        int32 full_begin = matcher.GetMatchBeginning();
        int32 full_end = matcher.GetMatchEnding();
        FTextRange full_range( full_begin, full_end );
        FString full_string = iPattern.Mid( full_range.BeginIndex, full_range.Len() );
        FString keyword = full_string;

        bool keyword_found = false;
        for( auto keyword_list : mKeywordLists )
        {
            auto FindKeywordInList = [keyword]( const TTuple<uint32, FPatternKeyword>& iKeyword )
            {
                return iKeyword.Value.mKeywordWithBraces == keyword;
            };

            if( keyword_list->mKeywordList.FilterByPredicate( FindKeywordInList ).Num() )
            {
                keyword_found = true;
                break;
            }
        }

        if( !keyword_found )
            return false;
    }

    //// This loop is to check if a valid pattern appears ONLY 1 time
    //for( auto keyword_list : mKeywordLists )
    //{
    //    for( auto pair : keyword_list->mKeywordList )
    //    {
    //        FString valid_pattern = pair.Value.mKeywordWithBraces;

    //        int32 start_index = iPattern.Find( valid_pattern );
    //        // If the current valid pattern is NOT found, that's ok and let's check the next pattern
    //        if( start_index == INDEX_NONE )
    //            continue;

    //        // Here we find the first occurance of the current valid pattern

    //        // Try to find the same pattern another time
    //        start_index = iPattern.Find( valid_pattern, ESearchCase::IgnoreCase, ESearchDir::FromStart, start_index + 1 );
    //        // If the current valid pattern is found again, it's wrong because a valid pattern should only appear 1 time, so return false
    //        if( start_index != INDEX_NONE )
    //            return false;
    //    }
    //}

    return true;
};
