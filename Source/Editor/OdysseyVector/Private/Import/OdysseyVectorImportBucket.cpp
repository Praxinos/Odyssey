#include "Import/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "EditorAssetLibrary.h"
#include "Engine/ObjectLibrary.h"

static void
ReadBucketDefineObjectPaletteEntryDescription( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive& Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
        , Ar
        , [&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive& Ar) -> void
        {
            switch (iChunkID)
            {
            case FOdysseyVectorExport::CHUNK_BUCKET_PALETTEENTRYDESCRIPTION_ENTRYID:
            {
                Ar << iBucket.GetPaletteEntryDescription().EntryId;

                if (!(iBucket.GetPaletteEntryDescription().EntryId.IsNone()))
                {
                    //If palettes aren't loaded, they don't have their entries. By doing this, we force them to be valid while we check for entries
                    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
                    TArray<FAssetData> AssetData;
                    AssetRegistryModule.Get().GetAssetsByClass(FName("OdysseyPalette"), AssetData, true);

                    UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UOdysseyPalette::StaticClass(), false, GIsEditor);
                    
                    for (int i = 0; i < AssetData.Num(); i++)
                    {
                        UObject* object = AssetData[i].GetAsset();
                        UOdysseyPalette* LoadedObject = Cast<UOdysseyPalette>(StaticLoadObject(UOdysseyPalette::StaticClass(), nullptr, *(AssetData[i].GetObjectPathString())));
                        FString pathName = LoadedObject->PaletteRoot->GetPathName();
                        ObjectLibrary->LoadAssetDataFromPath(pathName);

                        //UPackage* FoundPackage = LoadPackage(nullptr, *(AssetData[i].GetObjectPathString()), LOAD_None, nullptr, nullptr);
                    //    int j = 0;
                    }
                    ObjectLibrary->LoadAssetsFromAssetData();

                    //AssetRegistryModule.Get().GetAssetsByClass(FName("OdysseyPaletteEntry"), AssetData, true);

                    for (int i = 0; i < AssetData.Num(); i++)
                    {
                        UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData[i].GetAsset());
                        TArray<UOdysseyPaletteEntry*> entries = palette->GetEntries();
                        for (int j = 0; j < entries.Num(); j++)
                        {
                            UE_LOG(LogTemp, Display, TEXT("%s"), *(entries[j]->GetFName().ToString()));
                            if (entries[j]->GetFName().IsEqual(iBucket.GetPaletteEntryDescription().EntryId, ENameCase::CaseSensitive))
                            {
                                iBucket.SetPaletteEntry( entries[j] );
                                break;
                            }
                        }
                    }
                }
            }
            break;

            case FOdysseyVectorExport::CHUNK_BUCKET_PALETTEENTRYDESCRIPTION_USEDSET:
            {
                Ar << iBucket.GetPaletteEntryDescription().UsedSet;
            }
            break;

            default:
                // Mandatory
                Ar.Seek(Ar.Tell() + iChunkLen);
                break;
            }
        });

    //TODOD: Get Referenced Objects

    /*UObject* object = Cast<UObject>(SelectorItem);
    UPackage* Package = object->GetPackage();
    if( Package )
        UE_LOG(LogTemp, Display, TEXT("%s, %s, %s"), *(object->GetFName().ToString()), *(Package->GetFName().ToString()), *(Package->GetPersistentGuid().ToString()));*/
}

void
FOdysseyVectorImport::ReadBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_BUCKET_PALETTEENTRYDESCRIPTION :
                {
                    ReadBucketDefineObjectPaletteEntryDescription( iBucket, Ar.Tell() + iChunkLen, Ar );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_PROPAGATED:
                {
                    uint32 propagated;

                    Ar << propagated;

                    iBucket.SetPropagated( propagated ? true : false );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iBucket.SetRotation( rotation );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_POSITION:
                {
                    double x;
                    double y;

                    Ar << x;
                    Ar << y;

                    iBucket.SetCoords( x, y );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_SOLIDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    iBucket.SetGradient(false);
                    iBucket.SetSolidColor( R, G, B, A );
                }
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT: // container
                    iBucket.SetGradient( true );
                break;

                case FOdysseyVectorExport::CHUNK_BUCKET_GRADIENT_STOP:
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
