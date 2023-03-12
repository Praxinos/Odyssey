#include "HUD/OdysseyVectorHUDGrid.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDGrid::~FOdysseyVectorHUDGrid()
{
}

FOdysseyVectorHUDGrid::FOdysseyVectorHUDGrid()
    : FOdysseyVectorHUDSelection()
{

}

void
FOdysseyVectorHUDGrid::Deform()
{
    for( int i = 0; i < mCellArray.size(); i++ )
    {
        DeformCell( mCellArray[i] );
    }
}

void
FOdysseyVectorHUDGrid::DeformCell( FGridCell& iCell )
{
    ::ULIS::FVec2D& p0 = iCell.mNode[0]->GetCoords();
    ::ULIS::FVec2D& p1 = iCell.mNode[1]->GetCoords();
    ::ULIS::FVec2D& p2 = iCell.mNode[2]->GetCoords();
    ::ULIS::FVec2D& p3 = iCell.mNode[3]->GetCoords();
    ::ULIS::FVec2D p0p1 = p1 - p0;
    ::ULIS::FVec2D p1p2 = p2 - p1;
    ::ULIS::FVec2D p3p2 = p2 - p3; // same way as p0p1
    ::ULIS::FVec2D p0p3 = p3 - p0; // same way as p1p2

    for( int i = 0; i < iCell.mPointArray.size(); i++ )
    {
        ::ULIS::FVec2D p0p1s = p0 + ( p0p1 * iCell.mPointArray[i].s );
        ::ULIS::FVec2D p3p2s = p3 + ( p3p2 * iCell.mPointArray[i].s );
        ::ULIS::FVec2D sLine = ( p3p2s - p0p1s );
        ::ULIS::FVec2D p1p2t = p1 + ( p1p2 * iCell.mPointArray[i].t );
        ::ULIS::FVec2D p0p3t = p0 + ( p0p3 * iCell.mPointArray[i].t );
        ::ULIS::FVec2D isxCoords;
        BLPoint worldPt;
        BLPoint objectPt;
        double isx;

        FOdysseyVector::IntersectSegment( p0p1s, p3p2s, p1p2t, p0p3t, &isx, nullptr );

        isxCoords = p0p1s + ( sLine * isx );

        worldPt = mSelectionBox.space->GetWorldMatrix().mapPoint( isxCoords.x, isxCoords.y );
        objectPt = iCell.mPointArray[i].mDeformedObject->GetInverseWorldMatrix().mapPoint( worldPt.x, worldPt.y );

        iCell.mPointArray[i].mPoint->Set( objectPt.x, objectPt.y );
    }
}

FOdysseyVectorPoint *
FOdysseyVectorHUDGrid::PickNode( double iWorldX, double iWorldY )
{
    if( mSelectionBox.space )
    {
        BLMatrix2D worldMatrix = mSelectionBox.space->GetWorldMatrix();

        for( int i = 0; i < mNodeArray.size(); i++ )
        {
            BLPoint pt = worldMatrix.mapPoint( mNodeArray[i].GetX(), mNodeArray[i].GetY() );
            ::ULIS::FVec2D vec = ::ULIS::FVec2D( iWorldX - pt.x, iWorldY - pt.y );

            if ( vec.Distance() <= FOdysseyVectorHUDGrid::HANDLE_RADIUS )
            {
                return &mNodeArray[i];
            }
        }
    }

    return nullptr;
}

void
FOdysseyVectorHUDGrid::Draw( UOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();

    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF ) );
    blctx->setStrokeWidth( 1.0f );

    blctx->setFillStyle( BLRgba32( 0xFF0000FF ) );

    if( mSelectionBox.space )
    {
        BLMatrix2D worldMatrix = mSelectionBox.space->GetWorldMatrix();

        for( int i = 0; i < mCellArray.size(); i++ )
        {
            BLPoint pt[4] = { worldMatrix.mapPoint( mCellArray[i].mNode[0]->GetX(), mCellArray[i].mNode[0]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[1]->GetX(), mCellArray[i].mNode[1]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[2]->GetX(), mCellArray[i].mNode[2]->GetY() ),
                              worldMatrix.mapPoint( mCellArray[i].mNode[3]->GetX(), mCellArray[i].mNode[3]->GetY() ) };

            blctx->strokePolygon( pt, 4 );
        }

        for( int i = 0; i < mNodeArray.size(); i++ )
        {
            BLPoint pt = worldMatrix.mapPoint( mNodeArray[i].GetX(), mNodeArray[i].GetY() );

            blctx->fillCircle( pt.x, pt.y, FOdysseyVectorHUDGrid::HANDLE_RADIUS );
        }
    }

    blctx->restore();
}

void
FOdysseyVectorHUDGrid::MapPoint( UOdysseyVectorObject* iObject, FOdysseyVectorPoint* iPoint, double iSpaceX, double iSpaceY )
{
    double paramX = iSpaceX / mSelectionBox.rect.w;
    double paramY = iSpaceY / mSelectionBox.rect.h;
    uint32 rowid = paramX * mCellCountX;
    uint32 colid = paramY * mCellCountY;
    uint32 offset = ( colid * mCellCountX ) + rowid;
    double s = ( iSpaceX - (double) rowid * mCellSizeX ) / mCellSizeX;
    double t = ( iSpaceY - (double) colid * mCellSizeY ) / mCellSizeY;
    FGridPoint gridPoint = { iObject, iPoint, s, t };

    mCellArray[offset].mPointArray.push_back( gridPoint );
}

void
FOdysseyVectorHUDGrid::Map( UOdysseyVectorScene& iScene )
{
    std::list<UOdysseyVectorObject*>& selectedObjectList = iScene.GetSelectedObjectList();

    for( std::list<UOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);

        MapObject( obj );
    }
}

void
FOdysseyVectorHUDGrid::MapObject( UOdysseyVectorObject* iObject )
{
    BLMatrix2D& spaceMatrix = mSelectionBox.space->GetInverseWorldMatrix();
    std::list<UOdysseyVectorObject*>& childrenList = iObject->GetChildrenList();

    if( iObject->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
    {
        UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(iObject);
        std::list<FOdysseyVectorVertex*>& vertexList = cubicPath->GetVertexList();
        std::list<FOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();
        BLMatrix2D conversionMatrix = spaceMatrix;

        conversionMatrix.transform( cubicPath->GetWorldMatrix() );

        for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
        {
            FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>(*it);
            BLPoint pt = conversionMatrix.mapPoint( cubicVertex->GetX(), cubicVertex->GetY() );
            double spaceX = pt.x - mSelectionBox.rect.x; // Hi Elon :) !
            double spaceY = pt.y - mSelectionBox.rect.y;

            MapPoint( iObject, cubicVertex, spaceX, spaceY );
        }

        for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorPoint* point[2] = { cubicSegment->GetHandle(0), cubicSegment->GetHandle(1) };

            for( int i = 0; i < 2; i++ )
            {
                BLPoint pt = conversionMatrix.mapPoint( point[i]->GetX(), point[i]->GetY() );
                double spaceX = pt.x - mSelectionBox.rect.x; // Hi again, Elon :) !
                double spaceY = pt.y - mSelectionBox.rect.y;

                MapPoint( iObject, point[i], spaceX, spaceY );
            }
        }
    }

    // Recurse
    for( std::list<UOdysseyVectorObject*>::iterator it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);

        MapObject( child );
    }
}

void
FOdysseyVectorHUDGrid::MakeNodes( uint32 iCellCountX, uint32 iCellCountY  )
{
    double y = mSelectionBox.rect.y;

    mNodeArray.clear();

    mCellSizeX = mSelectionBox.rect.w / iCellCountX;
    mCellSizeY = mSelectionBox.rect.h / iCellCountY;

    mNodeCountX = iCellCountX + 1;
    mNodeCountY = iCellCountY + 1;

    mNodeArray.resize( mNodeCountX * mNodeCountY );

    for( uint32 i = 0; i < mNodeCountY; i++ )
    {
        double x = mSelectionBox.rect.x;

        for( uint32 j = 0; j < mNodeCountX; j++ )
        {
            uint32 offset = ( i * mNodeCountX ) + j;

            mNodeArray[offset].Set( x, y );

            x += mCellSizeX;
        }
        y += mCellSizeY;
    }
}

void
FOdysseyVectorHUDGrid::MakeCells( uint32 iCellCountX, uint32 iCellCountY )
{
    mCellArray.clear();

    mCellCountX = iCellCountX;
    mCellCountY = iCellCountY;

    mCellArray.resize( mCellCountX * mCellCountY );

    for( uint32 i = 0; i < mCellCountY; i++ )
    {
        uint32 n = i + 1;

        for( uint32 j = 0; j < mCellCountX; j++ )
        {
            uint32 k = j + 1;
            uint32 node0idx = ( i * mNodeCountX ) + j;
            uint32 node1idx = ( i * mNodeCountX ) + k;
            uint32 node2idx = ( n * mNodeCountX ) + k;
            uint32 node3idx = ( n * mNodeCountX ) + j;
            uint32 offset = ( i * mCellCountX ) + j;

            mCellArray[offset].mNode[0] = &mNodeArray[node0idx];
            mCellArray[offset].mNode[1] = &mNodeArray[node1idx];
            mCellArray[offset].mNode[2] = &mNodeArray[node2idx];
            mCellArray[offset].mNode[3] = &mNodeArray[node3idx];
        }
    }
}

void
FOdysseyVectorHUDGrid::MakeGrid( UOdysseyVectorScene& iScene, uint32 iCellCountX, uint32 iCellCountY )
{
    UpdateSelectionBox( iScene );

    if( mSelectionBox.rect.Area() )
    {
        // Make the box a little bit bigger in order to have coordinates > 0.0f and < 1.0f.
        mSelectionBox.rect.x -= ( mSelectionBox.rect.w * 0.01f );
        mSelectionBox.rect.y -= ( mSelectionBox.rect.h * 0.01f );
        mSelectionBox.rect.w += ( mSelectionBox.rect.w * 0.02f );
        mSelectionBox.rect.h += ( mSelectionBox.rect.h * 0.02f );

        MakeNodes( iCellCountX, iCellCountY );
        MakeCells( iCellCountX, iCellCountY );

        Map( iScene );
    }
}
