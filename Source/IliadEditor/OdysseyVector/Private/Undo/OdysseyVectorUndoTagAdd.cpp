// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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
                                                  , FOdysseyVectorTag* iTag )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mTagArray.push_back( iTag );
}

FOdysseyVectorUndoTagAdd::FOdysseyVectorUndoTagAdd( FOdysseyVectorGroupPaint* iScene
                                                  , const std::vector<FOdysseyVectorTag*>& iTagArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagAdd");
}
