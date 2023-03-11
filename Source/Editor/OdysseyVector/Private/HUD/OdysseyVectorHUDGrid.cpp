#include "HUD/OdysseyVectorHUDGrid.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDGrid::~FOdysseyVectorHUDGrid()
{
}

FOdysseyVectorHUDGrid::FOdysseyVectorHUDGrid()
    : FOdysseyVectorHUDSelection()
{

}

FOdysseyVectorPoint *
FOdysseyVectorHUDGrid::PickNode( double iWorldX, double iWorldY )
{

    return nullptr;
}

void
FOdysseyVectorHUDGrid::Draw( UOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{

}

void
FOdysseyVectorHUDGrid::MapPoint( FOdysseyVectorPoint* iPoint, double iSpaceX, double iSpaceY )
{
    double paramX = iSpaceX / mSelectionBox.rect.w;
    double paramY = iSpaceY / mSelectionBox.rect.h;
    uint32 rowid = paramX * mCellCountX;
    uint32 colid = paramY * mCellCountY;
    uint32 offset = ( colid * mCellCountX ) + rowid;
    double s = paramX - ( ( rowid * mCellSizeX ) / mCellSizeX );
    double t = paramY - ( ( rowid * mCellSizeY ) / mCellSizeY );
    FGridPoint gridPoint = { iPoint, s, t };

    mCellArray[offset].pointArray.push_back( gridPoint );
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

            MapPoint( cubicVertex, spaceX, spaceY );
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

                MapPoint( point[i], spaceX, spaceY );
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

        for( uint32 j = 0; j <= mNodeCountX; j++ )
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
            uint32 node0idx = ( i * mCellCountX ) + j;
            uint32 node1idx = ( i * mCellCountX ) + k;
            uint32 node2idx = ( n * mCellCountX ) + k;
            uint32 node3idx = ( n * mCellCountX ) + j;
            uint32 offset = node0idx;

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
        MakeNodes( iCellCountX, iCellCountY );
        MakeCells( iCellCountX, iCellCountY );
    }
}
