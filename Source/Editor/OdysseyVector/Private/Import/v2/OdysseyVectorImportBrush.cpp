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
                            , [&iBrush](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
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
                    FString assetName;

                    Ar << assetName;

                    iBrush.SetTexture( Cast<UTexture2D>(UEditorAssetLibrary::LoadAsset( assetName )) );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
