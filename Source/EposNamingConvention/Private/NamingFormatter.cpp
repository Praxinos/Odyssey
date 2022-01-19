// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
    FNamingConventionBoard board_settings = settings->BoardNaming;

    const FBoardNameElements& name_elements = board_sequence->NameElements;
    if( !name_elements.IsValid() )
        return board_sequence->GetName();

    const FString name = board_settings.Pattern.Replace( TEXT( "{board-index}" ), *FString::Printf( TEXT( "%0*d" ), board_settings.IndexFormat.NumDigits, name_elements.Index ) )

                                               .Replace( TEXT( "{studio-name}" ),           *name_elements.StudioName )
                                               .Replace( TEXT( "{studio-accronym}" ),       *name_elements.StudioAccronym )
                                               .Replace( TEXT( "{license-name}" ),          *name_elements.LicenseName )
                                               .Replace( TEXT( "{license-accronym}" ),      *name_elements.LicenseAccronym )
                                               .Replace( TEXT( "{production-name}" ),       *name_elements.ProductionName )
                                               .Replace( TEXT( "{production-accronym}" ),   *name_elements.ProductionAccronym )
                                               .Replace( TEXT( "{season}" ),                name_elements.IsSerie ? *FString::FromInt( name_elements.Season ) : TEXT("") )
                                               .Replace( TEXT( "{episode}" ),               name_elements.IsSerie ? *FString::FromInt( name_elements.Episode ) : TEXT( "" ) )
                                               .Replace( TEXT( "{part}" ),                  *name_elements.Part )
                                               .Replace( TEXT( "{department-name}" ),       *name_elements.DepartmentName )
                                               .Replace( TEXT( "{department-accronym}" ),   *name_elements.DepartmentAccronym )
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
    FNamingConventionShot shot_settings = settings->ShotNaming;

    const FShotNameElements& name_elements = shot_sequence->NameElements;
    if( !name_elements.IsValid() )
        return shot_sequence->GetName();

    const FString name = shot_settings.Pattern.Replace( TEXT( "{shot-index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.IndexFormat.NumDigits, name_elements.Index ) )
                                              .Replace( TEXT( "{take-index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.TakeFormat.NumDigits, name_elements.TakeIndex ) )

                                              .Replace( TEXT( "{studio-name}" ),            *name_elements.StudioName )
                                              .Replace( TEXT( "{studio-accronym}" ),        *name_elements.StudioAccronym )
                                              .Replace( TEXT( "{license-name}" ),           *name_elements.LicenseName )
                                              .Replace( TEXT( "{license-accronym}" ),       *name_elements.LicenseAccronym )
                                              .Replace( TEXT( "{production-name}" ),        *name_elements.ProductionName )
                                              .Replace( TEXT( "{production-accronym}" ),    *name_elements.ProductionAccronym )
                                              .Replace( TEXT( "{season}" ),                 name_elements.IsSerie ? *FString::FromInt( name_elements.Season ) : TEXT("") )
                                              .Replace( TEXT( "{episode}" ),                name_elements.IsSerie ? *FString::FromInt( name_elements.Episode ) : TEXT( "" ) )
                                              .Replace( TEXT( "{part}" ),                   *name_elements.Part )
                                              .Replace( TEXT( "{department-name}" ),        *name_elements.DepartmentName )
                                              .Replace( TEXT( "{department-accronym}" ),    *name_elements.DepartmentAccronym )
                                              .Replace( TEXT( "{initials}" ),               *name_elements.Initials )
                                              ;

    return name;
}
