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

    const FString name = board_settings.Pattern.Replace( TEXT( "{board-index}" ), *FString::Printf( TEXT( "%0*d" ), board_settings.IndexFormat.NumDigits, name_elements.Index ) )

                                               .Replace( TEXT( "{studio-name}" ),           *name_elements.StudioName )
                                               .Replace( TEXT( "{studio-acronym}" ),        *name_elements.StudioAcronym )
                                               .Replace( TEXT( "{license-name}" ),          *name_elements.LicenseName )
                                               .Replace( TEXT( "{license-acronym}" ),       *name_elements.LicenseAcronym )
                                               .Replace( TEXT( "{production-name}" ),       *name_elements.ProductionName )
                                               .Replace( TEXT( "{production-acronym}" ),    *name_elements.ProductionAcronym )
                                               .Replace( TEXT( "{season}" ),                 name_elements.IsSerie ? *FString::Printf( TEXT( "%0*d" ), global_settings.SeasonNumDigits, name_elements.Season ) : TEXT("") )
                                               .Replace( TEXT( "{episode}" ),                name_elements.IsSerie ? *FString::Printf( TEXT( "%0*d" ), global_settings.EpisodeNumDigits, name_elements.Episode ) : TEXT( "" ) )
                                               .Replace( TEXT( "{part}" ),                  *name_elements.Part )
                                               //.Replace( TEXT( "{department-name}" ),       *name_elements.DepartmentName )
                                               //.Replace( TEXT( "{department-acronym}" ),    *name_elements.DepartmentAcronym )
                                               .Replace( TEXT( "{initials}" ),              *name_elements.Initials )
                                               ;

    return name;
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

    const FString name = shot_settings.Pattern.Replace( TEXT( "{shot-index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.IndexFormat.NumDigits, name_elements.Index ) )
                                              .Replace( TEXT( "{take-index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.TakeFormat.NumDigits, name_elements.TakeIndex ) )
                                              //PATCH: error in default value for take index in NamingConventionSettings.h
                                              .Replace( TEXT( "{take_index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.TakeFormat.NumDigits, name_elements.TakeIndex ) )

                                              .Replace( TEXT( "{studio-name}" ),            *name_elements.StudioName )
                                              .Replace( TEXT( "{studio-acronym}" ),         *name_elements.StudioAcronym )
                                              .Replace( TEXT( "{license-name}" ),           *name_elements.LicenseName )
                                              .Replace( TEXT( "{license-acronym}" ),        *name_elements.LicenseAcronym )
                                              .Replace( TEXT( "{production-name}" ),        *name_elements.ProductionName )
                                              .Replace( TEXT( "{production-acronym}" ),     *name_elements.ProductionAcronym )
                                              .Replace( TEXT( "{season}" ),                 name_elements.IsSerie ? *FString::Printf( TEXT( "%0*d" ), global_settings.SeasonNumDigits, name_elements.Season ) : TEXT("") )
                                              .Replace( TEXT( "{episode}" ),                name_elements.IsSerie ? *FString::Printf( TEXT( "%0*d" ), global_settings.EpisodeNumDigits, name_elements.Episode ) : TEXT( "" ) )
                                              .Replace( TEXT( "{part}" ),                   *name_elements.Part )
                                              //.Replace( TEXT( "{department-name}" ),        *name_elements.DepartmentName )
                                              //.Replace( TEXT( "{department-acronym}" ),     *name_elements.DepartmentAcronym )
                                              .Replace( TEXT( "{initials}" ),               *name_elements.Initials )
                                              ;

    return name;
}
