// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Import/v2/OdysseyVectorImport.h"
#include "EditorAssetLibrary.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorBrush.h"

void
FOdysseyVectorImportV2::ReadBrush( FOdysseyVectorBrush& iBrush, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [this, &iBrush](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_BRUSH_REVERT:
                {
                    uint32 revert;

                    Ar << revert;

                    iBrush.Revert = static_cast<bool>(revert);
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BRUSH_EXTENSIONMODE:
                {
                    uint32 extensionMode;

                    Ar << extensionMode;

                    //iBrush.ExtendOverPath = static_cast<bool>(extendOverPath);
                    iBrush.ExtensionMode = static_cast<eBrushExtensionMode>(extensionMode);
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BRUSH_COLORFROMBRUSH:
                {
                    uint32 colorFromBrush;

                    Ar << colorFromBrush;

                    iBrush.ColorFromBrush = static_cast<bool>(colorFromBrush);
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BRUSH_TEXTURE:
                {
                    UTexture2D* texture;
                    FString assetName;

                    Ar << assetName;

                    texture = Cast<UTexture2D>(UEditorAssetLibrary::LoadAsset( assetName ));

                    if( texture )
                    {
                        mBrushTextureMultiMap.insert( std::pair<UTexture2D*,FOdysseyVectorBrush*>( texture, &iBrush ) );
                    }
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
