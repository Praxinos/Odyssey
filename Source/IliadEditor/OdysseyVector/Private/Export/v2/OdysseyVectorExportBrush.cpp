// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorBrush.h"

void
FOdysseyVectorExportV2::WriteBrushColorFromBrush( const FOdysseyVectorBrush& iBrush, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BRUSH_COLORFROMBRUSH
                            , Ar
                            , [&iBrush](FArchive &Ar) -> void
    {
        uint32 colorFromBrush = (uint32) iBrush.ColorFromBrush;

        Ar << colorFromBrush;
    } );
}

void
FOdysseyVectorExportV2::WriteBrushExtendOverPath( const FOdysseyVectorBrush& iBrush, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BRUSH_EXTENSIONMODE
                            , Ar
                            , [&iBrush](FArchive &Ar) -> void
    {
        uint32 extensionMode = (uint32) iBrush.ExtensionMode;

        Ar << extensionMode;
    } );
}

void
FOdysseyVectorExportV2::WriteBrushRevert( const FOdysseyVectorBrush& iBrush, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BRUSH_REVERT
                            , Ar
                            , [&iBrush](FArchive &Ar) -> void
    {
        uint32 revert = (uint32) iBrush.Revert;

        Ar << revert;
    } );
}

void
FOdysseyVectorExportV2::WriteBrushTexture( const FOdysseyVectorBrush& iBrush, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BRUSH_TEXTURE
                            , Ar
                            , [&iBrush](FArchive &Ar) -> void
    {
        FString assetName = FSoftObjectPath( iBrush.GetTexture() ).ToString();

        Ar << assetName;
    } );
}

void
FOdysseyVectorExportV2::WriteBrush( const FOdysseyVectorBrush& iBrush, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BRUSH
                            , Ar
                            , [&iBrush](FArchive &Ar) -> void
    {
        if( iBrush.GetTexture() )
        {
            WriteBrushTexture( iBrush, Ar );
        }

        WriteBrushExtendOverPath( iBrush, Ar );
        WriteBrushColorFromBrush( iBrush, Ar );
        WriteBrushRevert( iBrush, Ar );
    } );
}
