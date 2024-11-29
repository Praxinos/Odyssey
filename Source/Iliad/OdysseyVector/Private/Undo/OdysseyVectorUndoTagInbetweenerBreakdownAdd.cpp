// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::~FOdysseyVectorUndoTagInbetweenerBreakdownAdd()
{
    if( mApplied )
    {

    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::FOdysseyVectorUndoTagInbetweenerBreakdownAdd( FOdysseyVectorSharedEnv* iSharedEnv
                                                                                          , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerBreakdownAdd( iSharedEnv
                                                  , std::list<FOdysseyVectorTagInbetweener*> ({ iInbetweenerTag })
                                                  , iReturnFlags )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::FOdysseyVectorUndoTagInbetweenerBreakdownAdd( FOdysseyVectorSharedEnv* iSharedEnv
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
                                                  , FSnapshotFlags::Breakdown::CHART
                                                  , FSnapshotFlags::Route::TRAJECTORIES
                                                  | FSnapshotFlags::Route::STEPS
                                                  , FSnapshotFlags::Trajectory::BEZIER
                                                  | FSnapshotFlags::Trajectory::WAYPOINTS );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadAlteredState();
    }

    // note: will update and request redraw
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordAlteredState(); // will run once
        inbetweenerTagSnapshot.LoadInitialState();
    }

    // note: will update and request redraw
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAdd");
}
