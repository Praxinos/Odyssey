// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mTagArray.push_back( iTag );
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , const std::vector<FOdysseyVectorTag*>& iTagArray
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mTagArray( iTagArray )
{
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , const std::list<FOdysseyVectorTag*>& iTagList
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mTagArray()
{
    mTagArray.reserve( iTagList.size() );

    for( FOdysseyVectorTag* tag : iTagList )
    {
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagRemove");
}
