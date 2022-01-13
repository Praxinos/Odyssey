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

    const UBoardAssetUserData* data = board_sequence->GetAssetUserData<UBoardAssetUserData>();
    if( !data )
        return board_sequence->GetName();

    const FString name = board_settings.Pattern.Replace( TEXT( "{board-index}" ), *FString::Printf( TEXT("%0*d"), board_settings.NumDigits, data->Index ) )
                                                              .Replace( TEXT( "{studio-name}" ), *data->StudioName )
                                                              .Replace( TEXT( "{studio-accronym}" ), *data->StudioAccronym )
                                                              .Replace( TEXT( "{production-name}" ), *data->ProductionName )
                                                              .Replace( TEXT( "{production-accronym}" ), *data->ProductionAccronym );

    return name;
    //return board_sequence->GetName();
}

//---

FString
UDefaultNamingFormatterShot::FormatName( const UObject* iContext )
{
    const UShotSequence* shot = Cast<UShotSequence>( iContext );

    return shot->GetName();
}
