#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerParam::~FOdysseyVectorUndoTagInbetweenerParam()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerParam::FOdysseyVectorUndoTagInbetweenerParam( FOdysseyVectorGroupPaint* iScene
                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
}

FOdysseyVectorUndoTagInbetweenerGridSize::~FOdysseyVectorUndoTagInbetweenerGridSize()
{
}

FOdysseyVectorUndoTagInbetweenerGridSize::FOdysseyVectorUndoTagInbetweenerGridSize( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::GRIDSIZE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , FSnapshotFlags::ALL
                                                 , FSnapshotFlags::ALL );
    }
}

FOdysseyVectorUndoTagInbetweenerGridType::~FOdysseyVectorUndoTagInbetweenerGridType()
{
}

FOdysseyVectorUndoTagInbetweenerGridType::FOdysseyVectorUndoTagInbetweenerGridType( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::GRIDTYPE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , FSnapshotFlags::ALL
                                                 , FSnapshotFlags::ALL );
    }
}

FOdysseyVectorUndoTagInbetweenerInterpolationType::~FOdysseyVectorUndoTagInbetweenerInterpolationType()
{
}

FOdysseyVectorUndoTagInbetweenerInterpolationType::FOdysseyVectorUndoTagInbetweenerInterpolationType( FOdysseyVectorGroupPaint* iScene
                                                                                                    , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , 0 
                                                 , FSnapshotFlags::ALL );
    }
}

FOdysseyVectorUndoTagInbetweenerColor::~FOdysseyVectorUndoTagInbetweenerColor()
{
}

FOdysseyVectorUndoTagInbetweenerColor::FOdysseyVectorUndoTagInbetweenerColor( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::COLOR
                                                 , 0 
                                                 , 0 );
    }
}

FOdysseyVectorUndoTagInbetweenerMapAsPolyline::~FOdysseyVectorUndoTagInbetweenerMapAsPolyline()
{
}

FOdysseyVectorUndoTagInbetweenerMapAsPolyline::FOdysseyVectorUndoTagInbetweenerMapAsPolyline( FOdysseyVectorGroupPaint* iScene
                                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE
                                                 , 0 
                                                 , 0 );
    }
}

void
FOdysseyVectorUndoTagInbetweenerParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotArray )
    {
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotArray )
    {
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerParam::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerParam");
}
