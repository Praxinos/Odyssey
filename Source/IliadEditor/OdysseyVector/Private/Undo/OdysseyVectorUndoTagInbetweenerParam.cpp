// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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
                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
}

FOdysseyVectorUndoTagInbetweenerWithThickness::~FOdysseyVectorUndoTagInbetweenerWithThickness()
{
}

FOdysseyVectorUndoTagInbetweenerWithThickness::FOdysseyVectorUndoTagInbetweenerWithThickness( FOdysseyVectorGroupPaint* iScene
                                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                            , uint64 iReturnFlags  )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::WITHTHICKNESS
                                                 , 0    // save all breakdown details
                                                 , 0    // save all route details
                                                 , 0
                                                 , eSnapshotState::Initial ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerConstantWidth::~FOdysseyVectorUndoTagInbetweenerConstantWidth()
{

}

FOdysseyVectorUndoTagInbetweenerConstantWidth::FOdysseyVectorUndoTagInbetweenerConstantWidth( FOdysseyVectorGroupPaint* iScene
                                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                            , uint64 iReturnFlags  )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::CONSTANTWIDTH
                                                 , 0    // save all breakdown details
                                                 , 0    // save all route details
                                                 , 0
                                                 , eSnapshotState::Initial ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerGridSize::~FOdysseyVectorUndoTagInbetweenerGridSize()
{
}

FOdysseyVectorUndoTagInbetweenerGridSize::FOdysseyVectorUndoTagInbetweenerGridSize( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::GRIDSIZE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , FSnapshotFlags::ALL    // save all breakdown details
                                                 , FSnapshotFlags::ALL    // save all route details
                                                 , FSnapshotFlags::ALL
                                                 , eSnapshotState::Initial ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerGridType::~FOdysseyVectorUndoTagInbetweenerGridType()
{
}

FOdysseyVectorUndoTagInbetweenerGridType::FOdysseyVectorUndoTagInbetweenerGridType( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::GRIDTYPE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
                                                 , FSnapshotFlags::ALL    // save all breakdown details
                                                 , 0                      // ignore route details
                                                 , FSnapshotFlags::ALL
                                                 , eSnapshotState::Initial ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerSquare::~FOdysseyVectorUndoTagInbetweenerSquare()
{
}

FOdysseyVectorUndoTagInbetweenerSquare::FOdysseyVectorUndoTagInbetweenerSquare( FOdysseyVectorGroupPaint* iScene
                                                                              , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::SQUARE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , FSnapshotFlags::ALL    // save all breakdown details
                                                 , FSnapshotFlags::ALL    // save all route details
                                                 , FSnapshotFlags::ALL
                                                 , eSnapshotState::Initial ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerInterpolationType::~FOdysseyVectorUndoTagInbetweenerInterpolationType()
{
}

FOdysseyVectorUndoTagInbetweenerInterpolationType::FOdysseyVectorUndoTagInbetweenerInterpolationType( FOdysseyVectorGroupPaint* iScene
                                                                                                    , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , 0                      // ignore breakdowns (grids)
                                                 , FSnapshotFlags::ALL    // save all route details
                                                 , FSnapshotFlags::ALL
                                                 , eSnapshotState::Initial ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerColor::~FOdysseyVectorUndoTagInbetweenerColor()
{
}

FOdysseyVectorUndoTagInbetweenerColor::FOdysseyVectorUndoTagInbetweenerColor( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::COLOR
                                                 , 0
                                                 , 0
                                                 , 0
                                                 , eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerMapAsPolyline::~FOdysseyVectorUndoTagInbetweenerMapAsPolyline()
{
}

FOdysseyVectorUndoTagInbetweenerMapAsPolyline::FOdysseyVectorUndoTagInbetweenerMapAsPolyline( FOdysseyVectorGroupPaint* iScene
                                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE
                                                 , 0
                                                 , 0
                                                 , 0
                                                 , eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotArray )
    {
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotArray )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Altered );
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerParam::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerParam");
}
