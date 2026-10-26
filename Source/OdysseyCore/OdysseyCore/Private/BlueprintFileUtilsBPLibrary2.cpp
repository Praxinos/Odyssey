// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "BlueprintFileUtilsBPLibrary2.h"

#include "ImageCore.h"
#include "ImageUtils.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UBlueprintFileUtilsBPLibrary2::UBlueprintFileUtilsBPLibrary2(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

//static
bool
UBlueprintFileUtilsBPLibrary2::ToFile( const FString& iContent, const FFilePath& iPathFile )
{
    return ToTextFile( iContent, iPathFile );
}

//static
bool
UBlueprintFileUtilsBPLibrary2::ToTextFile( const FString& iContent, const FFilePath& iPathFile )
{
    const bool& bResult = FFileHelper::SaveStringToFile( iContent, *iPathFile.FilePath );
    if( !bResult )
    {
        FFrame::KismetExecutionMessage( *FString::Printf( TEXT( "Failed to save data to file. %s" ), *iPathFile.FilePath ), ELogVerbosity::Error );
    }

    return bResult;
}

//static
bool
UBlueprintFileUtilsBPLibrary2::FromTextFile( FString& oContent, const FFilePath& iPathFile )
{
    const bool& bResult = FFileHelper::LoadFileToString( oContent, *iPathFile.FilePath );
    if( !bResult )
    {
        FFrame::KismetExecutionMessage( *FString::Printf( TEXT( "Failed to load data from file. %s" ), *iPathFile.FilePath ), ELogVerbosity::Error );
    }

    return bResult;
}

//static
bool
UBlueprintFileUtilsBPLibrary2::ConvertRawRGBAToPNG( const FFilePath& iRawPathFile, int32 iWidth, int32 iHeight, FFilePath& oPNGPathFile )
{
    if( iWidth <= 0 || iHeight <= 0 )
        return false;

    TArray64<uint8> RawData;
    if( !FFileHelper::LoadFileToArray( RawData, *iRawPathFile.FilePath ) )
        return false;

    const int32 BytesPerPixel = 4;
    const int32 PaddingBytes = ( RawData.Num() - int64( iWidth ) * iHeight * BytesPerPixel ) / iHeight;
    const int32 StrideBytes = iWidth * BytesPerPixel + PaddingBytes;

    FImage Image( iWidth, iHeight, ERawImageFormat::BGRA8, EGammaSpace::sRGB );

    // Swap R and B as ERawImageFormat::RGBA8 doesn't exist
    for( int64 i = 0; i < RawData.Num(); i += 4 )
        Swap( RawData[i], RawData[i + 2] );

    // Fill image data manually as there is a padding/stride
    //Image.RawData = MoveTemp( RawData );
    Image.RawData.SetNumUninitialized( int64( iWidth ) * iHeight * BytesPerPixel );
    for( int32 y = 0; y < iHeight; y++ )
    {
        FMemory::Memcpy(
            Image.RawData.GetData() + int64( y ) * iWidth * BytesPerPixel,
            RawData.GetData() + int64( y ) * StrideBytes,
            iWidth * BytesPerPixel );
    }

    const FString TempDir = FPlatformProcess::UserTempDir();
    //IFileManager::Get().MakeDirectory( *TempDir, true );

    oPNGPathFile.FilePath = FPaths::Combine( TempDir, FGuid::NewGuid().ToString() + TEXT( ".png" ) );

    return FImageUtils::SaveImageByExtension( *oPNGPathFile.FilePath, FImageView( Image ), 100 );
}
