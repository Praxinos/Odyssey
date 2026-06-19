// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoMassModifier.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoMassModifier::~FOdysseyVectorUndoMassModifier()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoMassModifier::FOdysseyVectorUndoMassModifier( FOdysseyVectorLayer* iLayer
                                                              , const std::list<FOdysseyVectorObject*>& iObjectList
                                                              , const FName& iCategoryName
                                                              , const FName& iMemberPropertyName
                                                              , const FName& iPropertyName
                                                              , bool iSavePaths )
    : FOdysseyVectorUndoObjectParam( iLayer, iObjectList, iCategoryName, iMemberPropertyName, iPropertyName )
{
    if( iSavePaths )
    {
        mPathSnapshotArray.reserve( iObjectList.size() );

        for( FOdysseyVectorObject* vectorObject : iObjectList )
        {
            if( vectorObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
            {
                mPathSnapshotArray.emplace_back( static_cast<FOdysseyVectorPath*>(vectorObject)
                                               , FSnapshotFlags::Object::Path::VERTICES
                                               , eSnapshotState::Initial );
            }
        }
    }
}

void
FOdysseyVectorUndoMassModifier::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndoObjectParam::Apply( iIgnored );

    for( FSnapshotPath& path : mPathSnapshotArray )
    {
        path.LoadState( eSnapshotState::Altered );
    }

    // Note: there will be 2 updates due to the first one being called by the base method. To improve.
    Update();
}

void
FOdysseyVectorUndoMassModifier::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndoObjectParam::Revert( iIgnored );

    for( FSnapshotPath& path : mPathSnapshotArray )
    {
        path.RecordState( eSnapshotState::Altered );
    }

    for( FSnapshotPath& path : mPathSnapshotArray )
    {
        path.LoadState( eSnapshotState::Initial );
    }

    // Note: there will be 2 updates due to the first one being called by the base method. To improve.
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoMassModifier::ToString() const
{
    return FString("FOdysseyVectorUndoMassModifier");
}
