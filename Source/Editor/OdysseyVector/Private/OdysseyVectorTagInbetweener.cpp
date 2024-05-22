#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorObject.h"

FInbetweenerGrid::~FInbetweenerGrid()
{
}

FInbetweenerGrid::FInbetweenerGrid( uint32 iNumCellX
                                  , uint32 iNumCellY
                                  , const ::ULIS::FRectD& iBoundingBox )
{
    Reset( iNumCellX, iNumCellY, iBoundingBox );
}

void
FInbetweenerGrid::Reset( uint32 iNumCellX
                       , uint32 iNumCellY
                       , const ::ULIS::FRectD& iBoundingBox )
{
    double x = iBoundingBox.x;
    double y = iBoundingBox.y;
    double stepx = iBoundingBox.w / iNumCellX;
    double stepy = iBoundingBox.h / iNumCellY;
    uint32 numVertexX = iNumCellX + 1;
    uint32 numVertexY = iNumCellY + 1;

    mVertexBuffer.resize( numVertexX * numVertexY );
    mCellBuffer.resize( iNumCellX * iNumCellY );

    // position vertices
    for( uint32 i = 0; i < numVertexY; i++ )
    {
        for( uint32 j = 0; j < numVertexX; j++ )
        {
            uint32 offset = ( i * numVertexX ) + j;

            mVertexBuffer[offset].position = ::ULIS::FVec2D( x, y );


            x += stepx;
        }

        y += stepy;
        x = iBoundingBox.x;
    }

    // design cells
    for( uint32 i = 0; i < iNumCellY; i++ )
    {
        for( uint32 j = 0; j < iNumCellX; j++ )
        {
            uint32 vertexOffset = ( i * numVertexX ) + j;
            uint32 cellOffset   = ( i * iNumCellX  ) + j;

            mCellBuffer[cellOffset].vertex[0] = &mVertexBuffer[vertexOffset];
            mCellBuffer[cellOffset].vertex[1] = &mVertexBuffer[vertexOffset+1];
            mCellBuffer[cellOffset].vertex[2] = &mVertexBuffer[vertexOffset+1+numVertexX];
            mCellBuffer[cellOffset].vertex[3] = &mVertexBuffer[vertexOffset+numVertexX];
        }
    }
}

FOdysseyVectorTagInbetweener::~FOdysseyVectorTagInbetweener()
{
}

FOdysseyVectorTagInbetweener::FOdysseyVectorTagInbetweener( FOdysseyVectorObject* iOwnerObject
                                                          , uint32 iNumCellX
                                                          , uint32 iNumCellY )
    : FOdysseyVectorTag( iOwnerObject )
    , mGrid( iNumCellX, iNumCellY, iOwnerObject->GetBBox( false ) )
{

}

void
FOdysseyVectorTagInbetweener::Reset()
{
    mGrid.Reset( mNumCellX, mNumCellY, mOwnerObject->GetBBox( false ) );
}

void
FOdysseyVectorTagInbetweener::Draw( BLContext* iBLContext
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{
    DrawGrid( iBLContext, iInvalidationArea, iAncestorsOpacity, iDrawingFlags );
}

void
FOdysseyVectorTagInbetweener::DrawGrid( BLContext* iBLContext
                                      , const ::ULIS::FRectD& iInvalidationArea
                                      , double iAncestorsOpacity
                                      , uint64 iDrawingFlags )
{
    BLMatrix2D worldMatrix = mOwnerObject->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    for( FInbetweenerCell& cell : mGrid.mCellBuffer )
    {
        BLPoint pt[4] = { worldMatrix.mapPoint( cell.vertex[0]->position.x
                                              , cell.vertex[0]->position.y )
                        , worldMatrix.mapPoint( cell.vertex[1]->position.x
                                              , cell.vertex[1]->position.y )
                        , worldMatrix.mapPoint( cell.vertex[2]->position.x
                                              , cell.vertex[2]->position.y )
                        , worldMatrix.mapPoint( cell.vertex[3]->position.x
                                              , cell.vertex[3]->position.y ) };

        iBLContext->strokeLine( pt[0], pt[1] );
        iBLContext->strokeLine( pt[1], pt[2] );
        iBLContext->strokeLine( pt[2], pt[3] );
        iBLContext->strokeLine( pt[3], pt[0] );
    }

    iBLContext->restore();
}
