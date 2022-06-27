// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NamingFormatter.h"

#include "Board/BoardSequence.h"
#include "Settings/NamingConventionSettings.h"
#include "Shot/ShotSequence.h"

//---

FString
UDefaultNamingFormatterBoard::FormatName( const UObject* iContext )
{
    const UBoardSequence* const_board_sequence = Cast<UBoardSequence>( iContext );
    UBoardSequence* board_sequence = const_cast<UBoardSequence*>( const_board_sequence );

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionGlobal global_settings = settings->GlobalNaming;
    FNamingConventionBoard board_settings = settings->BoardNaming;

    const FBoardNameElements& name_elements = board_sequence->NameElements;
    if( !name_elements.IsValid() )
        return board_sequence->GetName();

    //---

    FString parsed_string = board_settings.Pattern;

    {
        auto ReplaceKeywordInt        = [&]( ENamingConventionBoardPatternKeyword iKeywordId, int iValue, int32 iNumDigits )    -> FString  { return parsed_string.Replace( *board_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *FString::Printf( TEXT( "%0*d" ), iNumDigits, iValue ) ); };
        auto ReplaceKeywordString     = [&]( ENamingConventionBoardPatternKeyword iKeywordId, FString iValue )                  -> FString  { return parsed_string.Replace( *board_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *iValue ); };

        //-

        parsed_string = ReplaceKeywordInt( ENamingConventionBoardPatternKeyword::BoardIndex, name_elements.Index, board_settings.IndexFormat.NumDigits );
    }

    //---

    {
        auto ReplaceKeywordInt        = [&]( ENamingConventionCommonPatternKeyword iKeywordId, int iValue, int32 iNumDigits )   -> FString  { return parsed_string.Replace( *board_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *FString::Printf( TEXT( "%0*d" ), iNumDigits, iValue ) ); };
        auto ReplaceKeywordString     = [&]( ENamingConventionCommonPatternKeyword iKeywordId, FString iValue )                 -> FString  { return parsed_string.Replace( *board_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *iValue ); };

        //-

        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::StudioName          , name_elements.StudioName );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::StudioAcronym       , name_elements.StudioAcronym );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::LicenseName         , name_elements.LicenseName );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::LicenseAcronym      , name_elements.LicenseAcronym );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::ProductionName      , name_elements.ProductionName );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::ProductionAcronym   , name_elements.ProductionAcronym );
        parsed_string = name_elements.IsSerie ? ReplaceKeywordInt( ENamingConventionCommonPatternKeyword::Season     , name_elements.Season, global_settings.SeasonNumDigits ) : parsed_string;
        parsed_string = name_elements.IsSerie ? ReplaceKeywordInt( ENamingConventionCommonPatternKeyword::Episode    , name_elements.Episode, global_settings.EpisodeNumDigits ) : parsed_string;
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::Part                , name_elements.Part );
        //parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::DepartmentName      , name_elements.DepartmentName );
        //parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::DepartmentAcronym   , name_elements.DepartmentAcronym );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::Initials            , name_elements.Initials );
    }

    return parsed_string;
}

//---

FString
UDefaultNamingFormatterShot::FormatName( const UObject* iContext )
{
    const UShotSequence* shot_sequence = Cast<UShotSequence>( iContext );

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionGlobal global_settings = settings->GlobalNaming;
    FNamingConventionShot shot_settings = settings->ShotNaming;

    const FShotNameElements& name_elements = shot_sequence->NameElements;
    if( !name_elements.IsValid() )
        return shot_sequence->GetName();

    //---

    FString parsed_string = shot_settings.Pattern;

    {
        auto ReplaceKeywordInt        = [&]( ENamingConventionShotPatternKeyword iKeywordId, int iValue, int32 iNumDigits )     -> FString  { return parsed_string.Replace( *shot_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *FString::Printf( TEXT( "%0*d" ), iNumDigits, iValue ) ); };
        auto ReplaceKeywordString     = [&]( ENamingConventionShotPatternKeyword iKeywordId, FString iValue )                   -> FString  { return parsed_string.Replace( *shot_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *iValue ); };

        //-

        parsed_string = ReplaceKeywordInt( ENamingConventionShotPatternKeyword::ShotIndex, name_elements.Index, shot_settings.IndexFormat.NumDigits );
        parsed_string = ReplaceKeywordInt( ENamingConventionShotPatternKeyword::TakeIndex, name_elements.TakeIndex, shot_settings.TakeFormat.NumDigits );
        //PATCH: error in default value for take index in NamingConventionSettings.h
        parsed_string = parsed_string.Replace( TEXT( "{take_index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.TakeFormat.NumDigits, name_elements.TakeIndex ) );
    }

    //---

    {
        auto ReplaceKeywordInt        = [&]( ENamingConventionCommonPatternKeyword iKeywordId, int iValue, int32 iNumDigits )   -> FString  { return parsed_string.Replace( *shot_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *FString::Printf( TEXT( "%0*d" ), iNumDigits, iValue ) ); };
        auto ReplaceKeywordString     = [&]( ENamingConventionCommonPatternKeyword iKeywordId, FString iValue )                 -> FString  { return parsed_string.Replace( *shot_settings.mPatternKeywordLists.GetKeyword( iKeywordId ).mKeywordWithBraces, *iValue ); };

        //-

        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::StudioName          , name_elements.StudioName );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::StudioAcronym       , name_elements.StudioAcronym );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::LicenseName         , name_elements.LicenseName );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::LicenseAcronym      , name_elements.LicenseAcronym );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::ProductionName      , name_elements.ProductionName );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::ProductionAcronym   , name_elements.ProductionAcronym );
        parsed_string = name_elements.IsSerie ? ReplaceKeywordInt( ENamingConventionCommonPatternKeyword::Season     , name_elements.Season, global_settings.SeasonNumDigits ) : parsed_string;
        parsed_string = name_elements.IsSerie ? ReplaceKeywordInt( ENamingConventionCommonPatternKeyword::Episode    , name_elements.Episode, global_settings.EpisodeNumDigits ) : parsed_string;
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::Part                , name_elements.Part );
        //parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::DepartmentName      , name_elements.DepartmentName );
        //parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::DepartmentAcronym   , name_elements.DepartmentAcronym );
        parsed_string = ReplaceKeywordString( ENamingConventionCommonPatternKeyword::Initials            , name_elements.Initials );
    }

    return parsed_string;
}
