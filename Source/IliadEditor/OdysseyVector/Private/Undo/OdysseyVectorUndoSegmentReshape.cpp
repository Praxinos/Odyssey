// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoSegmentReshape.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoSegmentReshape::~FOdysseyVectorUndoSegmentReshape()
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

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene
                                                                  , uint64 iReturnFlags
 )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
}

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene
                                                                  , const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    std::vector<FOdysseyVectorSegment*> segmentArray;

    FOdysseyVectorVertex::ArrayToSegmentArray( iVertexArray, segmentArray );

    mVertexSnapshotArray.reserve( iVertexArray.size() );

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        mVertexSnapshotArray.push_back( FSnapshotVertex( iVertexArray[i]
                                                        , FSnapshotFlags::ALL ) );
    }

    RecordSegment( segmentArray );
}

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene
                                                                  , const std::vector<FOdysseyVectorSegment*>& iSegmentArray
                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    RecordSegment( iSegmentArray );
}


void
FOdysseyVectorUndoSegmentReshape::RecordVertex( FOdysseyVectorVertex* iVertex )
{
    FOdysseyVectorSegment* segment[2] = { iVertex->GetFirstSegment()
                                        , iVertex->GetLastSegment() };

    mVertexSnapshotArray.push_back( FSnapshotVertex( iVertex, FSnapshotFlags::ALL ) );

    // record segment for undos first
    if( segment[0] && HasSegment( segment[0] ) == false )
    {
        RecordSegment( segment[0] );
    }

    if( segment[1] && HasSegment( segment[1] ) == false )
    {
        RecordSegment( segment[1] );
    }
}

void
FOdysseyVectorUndoSegmentReshape::RecordSegment( const std::vector<FOdysseyVectorSegment*>& iSegmentArray )
{
    mCubicSegmentSnapshotArray.reserve( iSegmentArray.size() );

    for( int i = 0; i < iSegmentArray.size(); i++ )
    {
        if( iSegmentArray[i]->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegmentArray[i]);

            mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic( cubicSegment, FSnapshotFlags::ALL ));
        }
    }
}

void
FOdysseyVectorUndoSegmentReshape::RecordSegment( FOdysseyVectorSegment* iSegment )
{
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);

        mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic( cubicSegment, FSnapshotFlags::ALL ));
    }
}

bool
FOdysseyVectorUndoSegmentReshape::HasVertex( FOdysseyVectorVertex* iVertex )
{
    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        if( mVertexSnapshotArray[i].GetVertex() == iVertex )
        {
            return true;
        }
    }

    return false;
}

bool
FOdysseyVectorUndoSegmentReshape::HasSegment( FOdysseyVectorSegment* iSegment )
{
    for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
    {
        if( mCubicSegmentSnapshotArray[i].GetCubicSegment() == iSegment )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorUndoSegmentReshape::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
    {
        mCubicSegmentSnapshotArray[i].Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoSegmentReshape::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
    {
        mCubicSegmentSnapshotArray[i].Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSegmentReshape::ToString() const
{
    return FString("FOdysseyVectorUndoSegmentReshape");
}
