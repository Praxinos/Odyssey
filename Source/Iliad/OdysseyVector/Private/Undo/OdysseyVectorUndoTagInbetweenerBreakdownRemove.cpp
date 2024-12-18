// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::~FOdysseyVectorUndoTagInbetweenerBreakdownRemove()
{
    if( mApplied )
    {

    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorSharedEnv* iSharedEnv
                                                                                                , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerBreakdownRemove( iSharedEnv
                                                     , std::list<FOdysseyVectorTagInbetweener*> ({ iInbetweenerTag })
                                                     , iReturnFlags )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorSharedEnv* iSharedEnv
                                                                                                , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iSharedEnv, iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        FOdysseyVectorEngine* inbetweenerTagEngine = inbetweenerTag->GetOwner()->GetEngine();

        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                  , ( FSnapshotFlags::Breakdown::TRANSFORMATIONS
                                                    | FSnapshotFlags::Breakdown::CHART )
                                                  , ( FSnapshotFlags::Route::TRAJECTORIES
                                                    | FSnapshotFlags::Route::STEPS )
                                                  , ( FSnapshotFlags::Trajectory::BEZIER
                                                    | FSnapshotFlags::Trajectory::WAYPOINTS ) );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadAlteredState();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordAlteredState();
        inbetweenerTagSnapshot.LoadInitialState();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownRemove");
}
