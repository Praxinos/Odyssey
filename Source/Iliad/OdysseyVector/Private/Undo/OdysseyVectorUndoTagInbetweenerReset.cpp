#include "Undo/OdysseyVectorUndoTagInbetweenerReset.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerReset::~FOdysseyVectorUndoTagInbetweenerReset()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerReset::FOdysseyVectorUndoTagInbetweenerReset( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::list<FInbetweenerBreakdown*>& iBreakdownList
                                                                            , bool iResetGridGeometry
                                                                            , bool iResetTransformations
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
        mBreakdownSnapshotBuffer.emplace_back( breakdown
                                             , ( iResetGridGeometry    ? FSnapshotFlags::Breakdown::GRIDGEOMETRY    : 0 )
                                             | ( iResetTransformations ? FSnapshotFlags::Breakdown::TRANSFORMATIONS : 0 ) );
    }
}

FOdysseyVectorUndoTagInbetweenerReset::FOdysseyVectorUndoTagInbetweenerReset( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                            , bool iResetGridGeometry
                                                                            , bool iResetTransformations
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
            mBreakdownSnapshotBuffer.emplace_back( breakdown
                                                 , ( iResetGridGeometry    ? FSnapshotFlags::Breakdown::GRIDGEOMETRY    : 0 )
                                                 | ( iResetTransformations ? FSnapshotFlags::Breakdown::TRANSFORMATIONS : 0 ) );
        }
    }
}

void
FOdysseyVectorUndoTagInbetweenerReset::Apply( UObject* iIgnored )
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
FOdysseyVectorUndoTagInbetweenerReset::Revert( UObject* iIgnored )
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
FOdysseyVectorUndoTagInbetweenerReset::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerReset");
}
