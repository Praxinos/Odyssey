// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/v2/OdysseyVectorImport.h"
#include "AssetRegistry/AssetRegistryModule.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorImportV2::~FOdysseyVectorImportV2()
{
}

FOdysseyVectorImportV2::FOdysseyVectorImportV2()
    : mScene( nullptr )
{
}

void
FOdysseyVectorImportV2::Reset()
{
    mScene = nullptr;
    mObjectArray.clear();
    mBrushTextureMultiMap.clear();
}

void
FOdysseyVectorImportV2::PostLoadTextures()
{
    for( std::pair<UTexture2D*,FOdysseyVectorBrush*> pair : mBrushTextureMultiMap )
    {
        pair.second->SetTexture( pair.first );
    }
}

void
FOdysseyVectorImportV2::Read( FOdysseyVectorGroupPaint* iScene, FArchive &Ar, uint64 iChunkEnd )
{
    Reset();

    // prevent reloading when the scene already exists.
    // This happens when undoing layer deletion for example.
    // Emptying the scene would create inconsistencies in the undo layer stack.
    if( iScene && ( iScene->GetChildrenList().size() == 0 ) )
    {
        mObjectArray.clear();
        mObjectArray.reserve( 400 );

        mScene = iScene;

        FOdysseyFile::ReadChunks( iChunkEnd
                                , Ar
                                , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
            {
                switch ( iChunkID )
                {
                    case FOdysseyFile::VectorV2::CHUNK_DECLARE_OBJECTS :
                         // this call populates vectorObjectArray
                         FOdysseyVectorImportV2::ReadObjectsDeclare( Ar.Tell() + iChunkLen, Ar );
                    break;

                    case FOdysseyFile::VectorV2::CHUNK_DEFINE_OBJECTS :
                         FOdysseyVectorImportV2::ReadObjectsDefine( Ar.Tell() + iChunkLen, Ar );
                    break;

                    default:
                    // Mandatory
                        Ar.Seek( Ar.Tell() + iChunkLen );
                    break;
                }
            } );

        iScene->UpdateMatrix();
        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS
                      | FOdysseyVectorObject::UPDATE_FROMFILE );
    }

    // Jump to the end of the junk, regardless of the fact that we've read nested chunks or not.
    // if we have read them, we'll just jump to the location where we already are.
    Ar.Seek( iChunkEnd );
}
