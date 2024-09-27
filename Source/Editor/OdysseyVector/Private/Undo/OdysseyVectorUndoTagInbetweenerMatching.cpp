#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerMatching::~FOdysseyVectorUndoTagInbetweenerMatching()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerMatching::FOdysseyVectorUndoTagInbetweenerMatching( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::list<FInbetweenerBreakdown*>& iBreakdownList
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    // we build a list of engines we will need to redraw
    for( FInbetweenerBreakdown* breakdown : iBreakdownList )
    {
        FOdysseyVectorEngine* engine = breakdown->GetInbetweenerTag()->GetOwner()->GetEngine();

        if( std::find( mEngineList.begin(), mEngineList.end(), engine ) == mEngineList.end() )
        {
            mEngineList.push_back( engine );
        }
    }

    mBreakdownSnapshotBuffer.reserve( iBreakdownList.size() );

    for( FInbetweenerBreakdown* breakdown : iBreakdownList )
    {
        mBreakdownSnapshotBuffer.emplace_back( breakdown, FSnapshotFlags::Breakdown::GRIDGEOMETRY );
    }
}

FOdysseyVectorUndoTagInbetweenerMatching::FOdysseyVectorUndoTagInbetweenerMatching( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    uint32 breakdownCount = 0;

    // we build a list of engines we will need to redraw
    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        FOdysseyVectorEngine* engine = inbetweenerTag->GetOwner()->GetEngine();

        if( std::find( mEngineList.begin(), mEngineList.end(), engine ) == mEngineList.end() )
        {
            mEngineList.push_back( engine );
        }
    }

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        breakdownCount += inbetweenerTag->GetBreakdownCount();
    }

    mBreakdownSnapshotBuffer.reserve( breakdownCount );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
        {
            mBreakdownSnapshotBuffer.emplace_back( breakdown, FSnapshotFlags::Breakdown::GRIDGEOMETRY );
        }
    }
}

void
FOdysseyVectorUndoTagInbetweenerMatching::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.LoadAlteredState();
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request radraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerMatching::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.RecordAlteredState();
        breakdownSnapshot.LoadInitialState();
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
FOdysseyVectorUndoTagInbetweenerMatching::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerMatching");
}
