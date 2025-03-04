// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/v2/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "Engine/ObjectLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorBucket.h"

void
FOdysseyVectorImportV2::ReadBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_BUCKET_COLORMODE:
                {
                    uint32 colorMode;

                    Ar << colorMode;

                    iBucket.SetColorMode( static_cast<eBucketColorMode>(colorMode) );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_PALETTESET:
                {
                    int paletteSet;

                    Ar << paletteSet;

                    iBucket.SetPaletteSet( paletteSet );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_PALETTEENTRY_MK2:
                {
                    FString fullpath;

                    Ar << fullpath;

                    FSoftObjectPath objectPath = FSoftObjectPath( fullpath );
                    UOdysseyPaletteEntry* paletteEntry = Cast<UOdysseyPaletteEntry>(objectPath.TryLoad());

                    iBucket.SetPaletteEntry( paletteEntry );
                }
                break;

                // LEGACY (non-functionnal)
                case FOdysseyFile::VectorV2::CHUNK_BUCKET_PALETTEENTRY_MK1:
                {
                    FName nameEntry;
                    Ar << nameEntry;

                    if (!(nameEntry.IsNone()))
                    {
                        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
                        TArray<FAssetData> AssetData;
                        AssetRegistryModule.Get().GetAssetsByClass(UOdysseyPalette::StaticClass()->GetClassPathName(), AssetData, true);

                        for (int i = 0; i < AssetData.Num(); i++)
                        {
                            UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData[i].GetAsset());
                            TArray<UOdysseyPaletteEntry*> entries = palette->GetEntries();
                            for (int j = 0; j < entries.Num(); j++)
                            {
                                if (entries[j]->GetFName().IsEqual(nameEntry, ENameCase::CaseSensitive))
                                {
                                    iBucket.SetPaletteEntry(entries[j]);
                                    iBucket.SetPaletteSet(palette->UsedSet_DEPRECATED);
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
/*
                case FOdysseyFile::VectorV2::CHUNK_BUCKET_SPREADING:
                {
                    uint32 spreadingPolicy;

                    Ar << spreadingPolicy;

                    iBucket.SetSpreadingPolicy(static_cast<eBucketSpreadingPolicy>(spreadingPolicy));
                }
                break;
*/
                case FOdysseyFile::VectorV2::CHUNK_BUCKET_PROPAGATED:
                {
                    uint32 propagated;

                    Ar << propagated;

                    iBucket.SetPropagated( propagated ? true : false );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iBucket.SetRotation( rotation );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_POSITION:
                {
                    double x;
                    double y;

                    Ar << x;
                    Ar << y;

                    iBucket.Set( x, y );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_SOLIDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    iBucket.SetSolidColor( R, G, B, A );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT: // container
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT_STOP:
                {
                    uint8 R,G,B,A;
                    double stopAt;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    Ar << stopAt;

                    if( stopAt == 0.0f ) iBucket.SetGradientColor0( R, G, B, A );
                    if( stopAt == 1.0f ) iBucket.SetGradientColor1( R, G, B, A );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT_RADIALRADIUS:
                {
                    double radialRadius;

                    Ar << radialRadius;

                    iBucket.SetRadialRadius( radialRadius );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BUCKET_GRADIENT_RADIALOFFSET:
                {
                    double offsetX;
                    double offsetY;

                    Ar << offsetX;
                    Ar << offsetY;

                    iBucket.SetRadialOffset( ::ULIS::FVec2D( offsetX, offsetY ) );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
