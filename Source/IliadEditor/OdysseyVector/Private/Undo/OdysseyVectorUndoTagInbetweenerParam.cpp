// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerParam::~FOdysseyVectorUndoTagInbetweenerParam()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerParam::FOdysseyVectorUndoTagInbetweenerParam( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
}

FOdysseyVectorUndoTagInbetweenerWithThickness::~FOdysseyVectorUndoTagInbetweenerWithThickness()
{
}

FOdysseyVectorUndoTagInbetweenerWithThickness::FOdysseyVectorUndoTagInbetweenerWithThickness( FOdysseyVectorGroupPaint* iScene
                                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::WITHTHICKNESS
                                                 , 0
                                                 , 0
                                                 , 0 );
    }
}

FOdysseyVectorUndoTagInbetweenerConstantWidth::~FOdysseyVectorUndoTagInbetweenerConstantWidth()
{

}

FOdysseyVectorUndoTagInbetweenerConstantWidth::FOdysseyVectorUndoTagInbetweenerConstantWidth( FOdysseyVectorGroupPaint* iScene
                                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::CONSTANTWIDTH
                                                 , 0
                                                 , 0
                                                 , 0 );
    }
}

FOdysseyVectorUndoTagInbetweenerGridSize::~FOdysseyVectorUndoTagInbetweenerGridSize()
{
}

FOdysseyVectorUndoTagInbetweenerGridSize::FOdysseyVectorUndoTagInbetweenerGridSize( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::GRIDSIZE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , FSnapshotFlags::ALL // save all breakdown details
                                                 , FSnapshotFlags::ALL // save all route details
                                                 , FSnapshotFlags::ALL ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerGridType::~FOdysseyVectorUndoTagInbetweenerGridType()
{
}

FOdysseyVectorUndoTagInbetweenerGridType::FOdysseyVectorUndoTagInbetweenerGridType( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::GRIDTYPE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
                                                 , FSnapshotFlags::ALL   // save all breakdown details
                                                 , 0                     // ignore route details
                                                 , FSnapshotFlags::ALL ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerSquare::~FOdysseyVectorUndoTagInbetweenerSquare()
{
}

FOdysseyVectorUndoTagInbetweenerSquare::FOdysseyVectorUndoTagInbetweenerSquare( FOdysseyVectorGroupPaint* iScene
                                                                              , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::SQUARE
                                                   | FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , FSnapshotFlags::ALL // save all breakdown details
                                                 , FSnapshotFlags::ALL // save all route details
                                                 , FSnapshotFlags::ALL );// save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerInterpolationType::~FOdysseyVectorUndoTagInbetweenerInterpolationType()
{
}

FOdysseyVectorUndoTagInbetweenerInterpolationType::FOdysseyVectorUndoTagInbetweenerInterpolationType( FOdysseyVectorGroupPaint* iScene
                                                                                                    , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , ( FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE
                                                   | FSnapshotFlags::Tag::Inbetweener::ROUTES )
                                                 , 0                     // ignore breakdowns (grids)
                                                 , FSnapshotFlags::ALL   // save all route details
                                                 , FSnapshotFlags::ALL ); // save all trajectory details
    }
}

FOdysseyVectorUndoTagInbetweenerColor::~FOdysseyVectorUndoTagInbetweenerColor()
{
}

FOdysseyVectorUndoTagInbetweenerColor::FOdysseyVectorUndoTagInbetweenerColor( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::COLOR
                                                 , 0
                                                 , 0
                                                 , 0 );
    }
}

FOdysseyVectorUndoTagInbetweenerMapAsPolyline::~FOdysseyVectorUndoTagInbetweenerMapAsPolyline()
{
}

FOdysseyVectorUndoTagInbetweenerMapAsPolyline::FOdysseyVectorUndoTagInbetweenerMapAsPolyline( FOdysseyVectorGroupPaint* iScene
                                                                                            , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndoTagInbetweenerParam( iScene, iInbetweenerTagArray )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE
                                                 , 0
                                                 , 0
                                                 , 0 );
    }
}

void
FOdysseyVectorUndoTagInbetweenerParam::Begin()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerParam::End()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Altered );
    }
}

void
FOdysseyVectorUndoTagInbetweenerParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
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

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
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
