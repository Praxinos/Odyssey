// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "BlueprintFileUtilitiesBPLibrary.h"

#include "FileUtilities/ZipArchiveReader.h"
#include "ImageCore.h"
#include "ImageUtils.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UBlueprintFileUtilitiesBPLibrary::UBlueprintFileUtilitiesBPLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

//static
bool
UBlueprintFileUtilitiesBPLibrary::Unzip( const FFilePath& iCompressedPathFile, FDirectoryPath& oUncompressedPath )
{
    const FString AbsoluteCompressedPathFile = FPaths::ConvertRelativePathToFull( iCompressedPathFile.FilePath );

    if( !FPaths::FileExists( AbsoluteCompressedPathFile ) )
        return false;

    const FString TempDir = FPlatformProcess::UserTempDir();

    oUncompressedPath.Path = FPaths::Combine( TempDir, FGuid::NewGuid().ToString() );
    FPaths::NormalizeDirectoryName( oUncompressedPath.Path );

    //---

    IFileHandle* FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenRead( *AbsoluteCompressedPathFile );
    if( !FileHandle )
        return false;

    FZipArchiveReader Reader( FileHandle );
    if( !Reader.IsValid() )
        return false;

    for( const FString& EntryName : Reader.GetFileNames() )
    {
        FString RelativeName = EntryName;
        FPaths::NormalizeDirectoryName( RelativeName );

        // Reject absolute paths and traversal attempts.
        if( RelativeName.StartsWith( TEXT( "/" ) )
            || FPaths::IsRelative( RelativeName ) == false
            || RelativeName.Contains( TEXT( "../" ) )
            || RelativeName == TEXT( ".." )
            || RelativeName.Contains( TEXT( ":" ) ) )
        {
            return false;
        }

        const FString OutputPath = FPaths::ConvertRelativePathToFull( FPaths::Combine( oUncompressedPath.Path, RelativeName ) );

        // Ensure the resolved path remains under Root.
        if( !OutputPath.StartsWith( oUncompressedPath.Path + TEXT( "/" ), ESearchCase::IgnoreCase ) )
            return false;

        // Create parent directories and write the entry.
        TArray<uint8> Data;

        if( !Reader.TryReadFile( EntryName, Data ) )
            return false;

        if( !IFileManager::Get().MakeDirectory( *FPaths::GetPath( OutputPath ), true ) )
            return false;

        if( !FFileHelper::SaveArrayToFile( Data, *OutputPath ) )
            return false;
    }

    return true;
}
