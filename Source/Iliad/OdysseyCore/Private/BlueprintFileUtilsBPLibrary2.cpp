// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "BlueprintFileUtilsBPLibrary2.h"
#include "HAL/FileManager.h"

UBlueprintFileUtilsBPLibrary2::UBlueprintFileUtilsBPLibrary2(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UBlueprintFileUtilsBPLibrary2::ToFile( const FString& iContent, const FFilePath& iPathFile )
{
    const bool& bResult = FFileHelper::SaveStringToFile( iContent, *iPathFile.FilePath );
    if( !bResult )
    {
        FFrame::KismetExecutionMessage( *FString::Printf( TEXT( "Failed to save data to file. %s" ), *iPathFile.FilePath ), ELogVerbosity::Error );
    }

    return bResult;
}
