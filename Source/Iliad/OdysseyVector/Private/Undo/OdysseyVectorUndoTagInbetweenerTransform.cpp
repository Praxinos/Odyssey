// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerTransform.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerTransform::~FOdysseyVectorUndoTagInbetweenerTransform()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerTransform::FOdysseyVectorUndoTagInbetweenerTransform( FOdysseyVectorGroupPaint* iScene
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
        mBreakdownSnapshotBuffer.emplace_back( breakdown
                                             , FSnapshotFlags::Breakdown::TRANSFORMATIONS );
    }
}

void
FOdysseyVectorUndoTagInbetweenerTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.LoadAlteredState();
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerTransform::Revert( UObject* iIgnored )
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
FOdysseyVectorUndoTagInbetweenerTransform::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTransform");
}
