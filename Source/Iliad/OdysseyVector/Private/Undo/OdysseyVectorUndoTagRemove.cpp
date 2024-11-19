// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagRemove::~FOdysseyVectorUndoTagRemove()
{
    if( mApplied )
    {
        for( FOdysseyVectorTag* tag : mTagArray )
        {
            delete tag;
        }
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , FOdysseyVectorTag* iTag
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    mTagArray.push_back( iTag );

    mEngineList.push_back( iTag->GetOwner()->GetEngine() );
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , const std::vector<FOdysseyVectorTag*>& iTagArray
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
    , mTagArray( iTagArray )
{
    // we build a list of engines we will need to redraw
    for( FOdysseyVectorTag* tag : iTagArray )
    {
        FOdysseyVectorEngine* engine = tag->GetOwner()->GetEngine();

        if( std::find( mEngineList.begin(), mEngineList.end(), engine ) == mEngineList.end() )
        {
            mEngineList.push_back( engine );
        }
    }
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , const std::list<FOdysseyVectorTag*>& iTagList
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
    , mTagArray()
{
    mTagArray.reserve( iTagList.size() );

    for( FOdysseyVectorTag* tag : iTagList )
    {
        FOdysseyVectorEngine* engine = tag->GetOwner()->GetEngine();

        // we build a list of engines we will need to redraw
        if( std::find( mEngineList.begin(), mEngineList.end(), engine ) == mEngineList.end() )
        {
            mEngineList.push_back( engine );
        }

        mTagArray.push_back( tag );
    }
}

void
FOdysseyVectorUndoTagRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorTag* tag : mTagArray )
    {
        tag->GetOwner()->RemoveTag( tag );
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FOdysseyVectorTag* tag : mTagArray )
    {
        tag->GetOwner()->AddTag( tag );
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagRemove");
}
