#include "Undo/OdysseyVectorUndoTagAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagAdd::~FOdysseyVectorUndoTagAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        for( int i = 0; i < mTagArray.size(); i++ )
        {
            delete mTagArray[i];
        }
    }
}

FOdysseyVectorUndoTagAdd::FOdysseyVectorUndoTagAdd( FOdysseyVectorGroupPaint* iScene
                                                  , FOdysseyVectorTag* iTag
                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mTagArray.push_back( iTag );
}

FOdysseyVectorUndoTagAdd::FOdysseyVectorUndoTagAdd( FOdysseyVectorGroupPaint* iScene
                                                  , const std::vector<FOdysseyVectorTag*>& iTagArray
                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mTagArray = iTagArray;
}

void
FOdysseyVectorUndoTagAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mTagArray.size(); i++ )
    {
        mTagArray[i]->GetOwner()->AddTag( mTagArray[i] );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoTagAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for(int i = 0; i < mTagArray.size(); i++)
    {
        mTagArray[i]->GetOwner()->RemoveTag( mTagArray[i] );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagAdd");
}
