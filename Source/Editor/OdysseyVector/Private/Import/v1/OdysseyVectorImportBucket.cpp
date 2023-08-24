#include "Import/v1/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "Engine/ObjectLibrary.h"

void
FOdysseyVectorImportV1::ReadBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImportV1::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExportV1::CHUNK_BUCKET_PALETTEENTRY:
                {
                    FName nameEntry;
                    Ar << nameEntry;

                    if (!(nameEntry.IsNone()))
                    {
                        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
                        TArray<FAssetData> AssetData;
                        AssetRegistryModule.Get().GetAssetsByClass(FName("OdysseyPalette"), AssetData, true);

                        for (int i = 0; i < AssetData.Num(); i++)
                        {
                            UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData[i].GetAsset());
                            TArray<UOdysseyPaletteEntry*> entries = palette->GetEntries();
                            for (int j = 0; j < entries.Num(); j++)
                            {
                                if (entries[j]->GetFName().IsEqual(nameEntry, ENameCase::CaseSensitive))
                                {
                                    iBucket.SetColorMode( eBucketColorMode::Palette );
                                    iBucket.SetPaletteEntry(entries[j]);
                                    break;
                                }
                            }
                        }
                    }
                }
                break;

                case FOdysseyVectorExportV1::CHUNK_BUCKET_SPREADING:
                {
                    uint32 spreadingPolicy;

                    Ar << spreadingPolicy;

                    iBucket.SetSpreadingPolicy(static_cast<eBucketSpreadingPolicy>(spreadingPolicy));
                }
                break;

                case FOdysseyVectorExportV1::CHUNK_BUCKET_PROPAGATED:
                {
                    uint32 propagated;

                    Ar << propagated;

                    iBucket.SetPropagated( propagated ? true : false );
                }
                break;

                case FOdysseyVectorExportV1::CHUNK_BUCKET_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iBucket.SetRotation( rotation );
                }
                break;

                case FOdysseyVectorExportV1::CHUNK_BUCKET_POSITION:
                {
                    double x;
                    double y;

                    Ar << x;
                    Ar << y;

                    iBucket.Set( x, y );
                }
                break;

                case FOdysseyVectorExportV1::CHUNK_BUCKET_SOLIDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    iBucket.SetColorMode( eBucketColorMode::SolidColor );
                    iBucket.SetSolidColor( R, G, B, A );
                }
                break;

                case FOdysseyVectorExportV1::CHUNK_BUCKET_GRADIENT: // container
                    iBucket.SetColorMode( eBucketColorMode::LinearGradient );
                break;

                case FOdysseyVectorExportV1::CHUNK_BUCKET_GRADIENT_STOP:
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

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
