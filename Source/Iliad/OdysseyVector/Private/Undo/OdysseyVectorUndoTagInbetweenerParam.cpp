// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

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
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    // we build a list of engines we will need to redraw
    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        FOdysseyVectorEngine* engine = inbetweenerTag->GetOwner()->GetEngine();

        if( std::find( mEngineList.begin(), mEngineList.end(), engine ) == mEngineList.end() )
        {
            mEngineList.push_back( engine );
        }
    }
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
                                                 , 0 ); // save all trajectory details
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
                                                 , FSnapshotFlags::ALL ); // save all trajectory details
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
                                                 , FSnapshotFlags::ALL ); // save all trajectory details
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
                                                 , FSnapshotFlags::ALL ); // save all trajectory details
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
                                                 , FSnapshotFlags::ALL ); // save all trajectory details
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
                                                 , 0 );
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
        inbetweenerTagSnapshot.LoadAlteredState();
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotArray )
    {
        inbetweenerTagSnapshot.RecordAlteredState();
        inbetweenerTagSnapshot.LoadInitialState();
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
FOdysseyVectorUndoTagInbetweenerParam::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerParam");
}
