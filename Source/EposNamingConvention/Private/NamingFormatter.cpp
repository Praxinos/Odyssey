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

    const FBoardNamingElements& naming_elements = board_sequence->GetNamingElements();
    if( !naming_elements.IsValid() )
        return board_sequence->GetName();

    const FString name = board_settings.Pattern.Replace( TEXT( "{board-index}" ), *FString::Printf( TEXT("%0*d"), board_settings.IndexFormat.NumDigits, naming_elements.Index ) )
                                               .Replace( TEXT( "{studio-name}" ), *naming_elements.StudioName )
                                               .Replace( TEXT( "{studio-accronym}" ), *naming_elements.StudioAccronym )
                                               .Replace( TEXT( "{production-name}" ), *naming_elements.ProductionName )
                                               .Replace( TEXT( "{production-accronym}" ), *naming_elements.ProductionAccronym )
                                               .Replace( TEXT( "{initials}" ), *naming_elements.Initials );

    return name;
    //return board_sequence->GetName();
}

//---

FString
UDefaultNamingFormatterShot::FormatName( const UObject* iContext )
{
    const UShotSequence* shot_sequence = Cast<UShotSequence>( iContext );

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionShot shot_settings = settings->ShotNaming;

    const FShotNamingElements& naming_elements = shot_sequence->GetNamingElements();
    if( !naming_elements.IsValid() )
        return shot_sequence->GetName();

    const FString name = shot_settings.Pattern.Replace( TEXT( "{shot-index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.IndexFormat.NumDigits, naming_elements.Index ) )
                                              .Replace( TEXT( "{take-index}" ), *FString::Printf( TEXT( "%0*d" ), shot_settings.TakeFormat.NumDigits, naming_elements.TakeIndex ) )
                                              .Replace( TEXT( "{studio-name}" ), *naming_elements.StudioName )
                                              .Replace( TEXT( "{studio-accronym}" ), *naming_elements.StudioAccronym )
                                              .Replace( TEXT( "{production-name}" ), *naming_elements.ProductionName )
                                              .Replace( TEXT( "{production-accronym}" ), *naming_elements.ProductionAccronym )
                                              .Replace( TEXT( "{initials}" ), *naming_elements.Initials );

    return name;
    //return shot->GetName();
}
