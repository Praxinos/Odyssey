#include "Import/OdysseyVectorImport.h"
#include "OdysseyVectorPathCubic.h"
#include "Palette/OdysseyPalette.h"
#include "EditorAssetLibrary.h"
#include "Engine/ObjectLibrary.h"

static FOdysseyVectorObject*
CreateObject( uint32 iObjectType )
{
    FOdysseyVectorObject* newObject = nullptr;

    switch ( iObjectType )
    {
        case FOdysseyVectorObject::VECTORROOTTYPE :
            // do nothing, the scene is already created by the vector layer
        break;

        case FOdysseyVectorObject::VECTORPATHCUBICTYPE :
            newObject = new FOdysseyVectorPathCubic( FString("CubicPath"));
        break;

        case FOdysseyVectorObject::VECTORGROUPPAINTTYPE :
            newObject = new FOdysseyVectorGroupPaint( FString("PaintGroup") );
        break;
/*
        case FOdysseyVectorObject::VECTORELLIPSETYPE :
            newObject = new FOdysseyVectorEllipse( FString("Ellipse"), 0.0f, 0.0f, 4.0f );
        break;
*/
        default :
            newObject = new FOdysseyVectorObject( FString("Object") );
        break;
    }

    return newObject;
}

void
FOdysseyVectorImport::ReadObjectsDeclare( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_DECLARE_OBJECT_ENTRY :
                {
                    FOdysseyVectorObject* newObject;
                    uint32 objectType;

                    Ar << objectType;

                    newObject = CreateObject( objectType );

                    if ( newObject )
                    {
                        vectorObjectArray.push_back( newObject );
                    }
                }
                break;

                default :
                    // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}

static void
ReadObjectsDefineObjectTransform( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_TRANSLATION:
                {
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iObject.Translate( translationX, translationY );
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iObject.Rotate( rotation );
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_SCALING:
                {
                    double scalingX;
                    double scalingY;

                    Ar << scalingX;
                    Ar << scalingY;

                    iObject.Scale( scalingX, scalingY );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}

static void
ReadObjectsDefineObjectPaletteEntryDescription(FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive& Ar)
{
    FOdysseyVectorImport::ReadChunks(iChunkEnd
        , Ar
        , [&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive& Ar) -> void
        {
            switch (iChunkID)
            {
            case FOdysseyVectorExport::CHUNK_OBJECT_PALETTEENTRYDESCRIPTION_ENTRYID:
            {
                Ar << iObject.GetPaletteEntryDescription().EntryId;

                if (!(iObject.GetPaletteEntryDescription().EntryId.IsNone()))
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
                            if (entries[j]->GetFName().IsEqual(iObject.GetPaletteEntryDescription().EntryId, ENameCase::CaseSensitive))
                            {
                                iObject.mObjectParam.Entry = entries[j];
                                break;
                            }
                        }
                    }
                }
            }
            break;

            case FOdysseyVectorExport::CHUNK_OBJECT_PALETTEENTRYDESCRIPTION_USEDSET:
            {
                Ar << iObject.GetPaletteEntryDescription().UsedSet;
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
FOdysseyVectorImport::ReadObjectsDefine( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            static uint32 objectID;

            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_DEFINE_OBJECT_ENTRY :
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_ID:
                    Ar << objectID;
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_PARENTID:
                {
                    uint32 parentID;

                    Ar << parentID;

                    // objectID = 0 if we are on the root node. Ignore it.
                    if( objectID )
                    {
                        vectorObjectArray[parentID]->AppendChild( vectorObjectArray[objectID] );
                    }
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM:
                    ReadObjectsDefineObjectTransform( *vectorObjectArray[objectID], Ar.Tell() + iChunkLen, Ar );
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_PALETTEENTRYDESCRIPTION:
                    ReadObjectsDefineObjectPaletteEntryDescription(*vectorObjectArray[objectID], Ar.Tell() + iChunkLen, Ar);
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_FOREGROUNDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    vectorObjectArray[objectID]->SetForegroundColor( R, G, B, A );
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_BACKGROUNDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    vectorObjectArray[objectID]->SetBackgroundColor( R, G, B, A );
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_PATHCUBIC:
                {
                    FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>( vectorObjectArray[objectID] );

                    FOdysseyVectorImport::ReadObjectPathCubic( *cubicPath, Ar.Tell() + iChunkLen, Ar );
                    // immediately update invalidated segments and updates the path's BBox
                    cubicPath->Update( 0 );
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_GROUPPAINT:
                {
                    FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( vectorObjectArray[objectID] );

                    FOdysseyVectorImport::ReadObjectGroupPaint( *paintGroup, Ar.Tell() + iChunkLen, Ar );

                    paintGroup->Invalidate();
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_ELLIPSE:
                {
                    FOdysseyVectorEllipse* circle = static_cast<FOdysseyVectorEllipse*>( vectorObjectArray[objectID] );

                    FOdysseyVectorImport::ReadObjectEllipse( *circle, Ar.Tell() + iChunkLen, Ar );

                    circle->Invalidate();
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
