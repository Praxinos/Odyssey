// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include <Brush/OdysseyVectorBrushSegment.h>
#include <OdysseyVectorSegment.h>
#include <OdysseyVectorPath.h>

FOdysseyVectorBrushSegment::~FOdysseyVectorBrushSegment()
{
}

FOdysseyVectorBrushSegment::FOdysseyVectorBrushSegment( FOdysseyVectorSegment* iSegment
                                                      , const ::ULIS::FRectD& iBoundingBox )
{
    BLMatrix2D& worldMatrix = iSegment->GetOwnerAsPath()->GetWorldMatrix();
    std::vector<FOdysseyVectorFraction>& fractionCache = iSegment->GetFractionCache();

    if( fractionCache.size() )
    {
        BLPoint worldFirstPoint = worldMatrix.mapPoint( fractionCache.front().polygon.point[1].x
                                                      , fractionCache.front().polygon.point[1].y );
        BLPoint worldLastPoint  = worldMatrix.mapPoint( fractionCache.front().polygon.point[5].x
                                                      , fractionCache.front().polygon.point[5].y );

        mBLPath.moveTo( ( worldFirstPoint.x - iBoundingBox.x ) / iBoundingBox.w
                      , ( worldFirstPoint.y - iBoundingBox.y ) / iBoundingBox.h );

        for( std::vector<FOdysseyVectorFraction>::iterator it = fractionCache.begin(); it != fractionCache.end(); ++it )
        {
            FOdysseyVectorFraction& fraction = (*it);
            BLPoint worldPoint = worldMatrix.mapPoint( fraction.polygon.point[2].x
                                                     , fraction.polygon.point[2].y );

            // convert to coords in the 0.0f - 1.0f range
            mBLPath.lineTo( ( worldPoint.x - iBoundingBox.x ) / iBoundingBox.w
                          , ( worldPoint.y - iBoundingBox.y ) / iBoundingBox.h );
        }

        for( std::vector<FOdysseyVectorFraction>::reverse_iterator it = fractionCache.rbegin(); it != fractionCache.rend(); ++it )
        {
            FOdysseyVectorFraction& fraction = (*it);
            BLPoint worldPoint = worldMatrix.mapPoint( fraction.polygon.point[4].x
                                                     , fraction.polygon.point[4].y );

            // convert to coords in the 0.0f - 1.0f range
            mBLPath.lineTo( ( worldPoint.x - iBoundingBox.x ) / iBoundingBox.w
                          , ( worldPoint.y - iBoundingBox.y ) / iBoundingBox.h );
        }

        mBLPath.lineTo( ( worldLastPoint.x - iBoundingBox.x ) / iBoundingBox.w
                      , ( worldLastPoint.y - iBoundingBox.y ) / iBoundingBox.h );

        //mBLPath.close();
    }
}

// this is too slow, we'll have to change our technique by building a BSP tree.
FOdysseyVectorFraction*
FOdysseyVectorBrushSegment::GetSegmentFraction( FOdysseyVectorSegment* iSegment
                                              , double iU )
{
    std::vector<FOdysseyVectorFraction>& fractionCache = iSegment->GetFractionCache();

    for( FOdysseyVectorFraction& fraction : fractionCache )
    {
        if ( ( iU >= fraction.polygon.U[0] ) && ( iU <= fraction.polygon.U[3] ) )
        {
            return &fraction;
        }
    }

    return nullptr;
}

void
FOdysseyVectorBrushSegment::Draw( BLContext* iBLContext
                                , const ::ULIS::FRectD& iInvalidationArea
                                , double iAncestorsOpacity
                                , FOdysseyVectorChain* iChain
                                , uint64 iDrawingFlags )
{
    iChain->IterateSegments( [ this
                             , iBLContext
                                ]( FOdysseyVectorVertex* vertex
                                 , FOdysseyVectorSegment* segment ) -> bool
        {
            std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
            const BLPoint* vertexData = mBLPath.vertexData();
            BLPath brushPath;

            //iBLContext->setStrokeWidth( 3.0f );
            iBLContext->setFillStyle( BLRgba32( 0, 0, 0, 255 ) );
            iBLContext->setFillRule( BL_FILL_RULE_EVEN_ODD );

            if( fractionCache.size() )
            {
                for( int i = 0; i < mBLPath.size(); i++ )
                {
                    FOdysseyVectorFraction* fraction = GetSegmentFraction( segment
                                                                         , vertexData[i].x );
                    ::ULIS::FVec2D* polyPoint = fraction->polygon.point;
                    double difu = ( fraction->polygon.U[3] - fraction->polygon.U[0] );
                    double u = difu ? ( vertexData[i].x - fraction->polygon.U[0] ) / difu : 0.0f;
                    double x, y;
                    ::ULIS::FVec2D p03 = ( polyPoint[0] + ( ( polyPoint[3] - polyPoint[0] ) * u ) );
                    ::ULIS::FVec2D p12 = ( polyPoint[1] + ( ( polyPoint[2] - polyPoint[1] ) * u ) );
                    ::ULIS::FVec2D p54 = ( polyPoint[5] + ( ( polyPoint[4] - polyPoint[5] ) * u ) );

                    {
                        double v = 1.0f - vertexData[i].y;

                        x = p12.x + ( p54.x - p12.x ) * v;
                        y = p12.y + ( p54.y - p12.y ) * v;
                    }

/*
                    if ( vertexData[i].y < 0.5f )
                    {
                        double v = vertexData[i].y / 0.5f;

                        x = p12.x + ( p03.x - p12.x ) * v;
                        y = p12.y + ( p03.y - p12.y ) * v;
                    }
                    else
                    {
                        double v = 0.5f + ( vertexData[i].y / 0.5f );

                        x = p03.x + ( p54.x - p03.x ) * v;
                        y = p03.y + ( p54.y - p03.y ) * v;
                    }
*/

                    // if this is the first point it will equal a moveTo command.
                    if( i == 0 ) brushPath.moveTo( x, y );
                    else         brushPath.lineTo( x, y );


                }

                //brushPath.close();
            }

  //brushPath.moveTo(26, 31);
  //brushPath.lineTo(642, 132);
  //brushPath.lineTo(882, 404);

            iBLContext->fillPath( brushPath );

            return false; // continue
        } );
}
