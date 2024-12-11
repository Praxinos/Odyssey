// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NamingFormatter.h"

#include "Board/BoardSequence.h"
#include "Settings/NamingConventionSettings.h"
#include "Shot/ShotSequence.h"

//---

bool
UDefaultNamingFormatterBoard::FormatName( const UObject* iContext, FString& oPatternFormatted )
{
    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionBoard board_settings = settings->BoardNaming;

    return FormatName( iContext, board_settings.Pattern, oPatternFormatted );
}

bool
UDefaultNamingFormatterBoard::FormatName( const UObject* iContext, const FString& iPatternToFormat, FString& oPatternFormatted )
{
    const UBoardSequence* board_sequence = Cast<UBoardSequence>( iContext );
    if( !board_sequence )
        return false;

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionGlobal global_settings = settings->GlobalNaming;
    FNamingConventionBoard board_settings = settings->BoardNaming;

    const FBoardNameElements& name_elements = board_sequence->NameElements;
    if( !name_elements.IsValid() )
        return false;

    //---

    FString parsed_string = iPatternToFormat;

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

    // oPatternFormatted MUST only be set before a return
    // It's to manage the case when iPatternToFormat == oPatternFormatted
    oPatternFormatted = parsed_string;

    return true;
}

//---

bool
UDefaultNamingFormatterShot::FormatName( const UObject* iContext, FString& oPatternFormatted )
{
    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionShot shot_settings = settings->ShotNaming;

    return FormatName( iContext, shot_settings.Pattern, oPatternFormatted );
}

bool
UDefaultNamingFormatterShot::FormatName( const UObject* iContext, const FString& iPatternToFormat, FString& oPatternFormatted )
{
    const UShotSequence* shot_sequence = Cast<UShotSequence>( iContext );
    if( !shot_sequence )
        return false;

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionGlobal global_settings = settings->GlobalNaming;
    FNamingConventionShot shot_settings = settings->ShotNaming;

    const FShotNameElements& name_elements = shot_sequence->NameElements;
    if( !name_elements.IsValid() )
        return false;

    //---

    FString parsed_string = iPatternToFormat;

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

    // oPatternFormatted MUST only be set before a return
    // It's to manage the case when iPatternToFormat == oPatternFormatted
    oPatternFormatted = parsed_string;

    return true;
}
