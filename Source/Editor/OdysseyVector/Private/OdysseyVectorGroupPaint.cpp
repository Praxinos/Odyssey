#include "OdysseyVectorGroupPaint.h"

void
UOdysseyVectorGroupPaint::Init( std::string iName )
{
    SetName( iName );
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
/*
    if ( ( iX > mBBox.x ) && ( iX < ( mBBox.x + mBBox.w ) )
      && ( iY > mBBox.y ) && ( iY < ( mBBox.y + mBBox.h ) ) )
    {
        return this;
    }
*/
    return nullptr;
}

static void floodFill ( int32 x
                      , int32 y
                      , BLImage* iImage
                      , BLImage* iMask
                      , uint32 iColor )
{
    BLImageData imageData;
    BLImageData maskData;

    iImage->getData( &imageData );
    iMask->getData( &maskData );

    if ( ( x >= 0 ) && ( x < imageData.size.w ) &&
         ( y >= 0 ) && ( y < imageData.size.h ) )
    {
        uint32 maxItems = 0xFFFF; // stack size for 65k pixels at first. Then we increase by 65k every time needed
        int32 (*stack)[2] = (int32(*)[2]) calloc ( maxItems, sizeof(int32) * 2 );
        uint8* passed = (uint8*) calloc ( imageData.size.w * imageData.size.h, sizeof(uint8) );
        uint32 nbItems = 0;
        uint32 curItem = 0;
        uint32 i;

        // add first pixel to the stack
        stack[nbItems][0] = x;
        stack[nbItems][1] = y;

        nbItems++;

        // process neighboring pixels
        while ( curItem < nbItems ) {
            int32 sx = stack[curItem][0],
                  sy = stack[curItem][1];
            int32 pos[4][2] = {{ sx + 1, sy     },
                               { sx    , sy + 1 },
                               { sx - 1, sy     },
                               { sx    , sy - 1 }};

            for( i = 0; i < 4; i++ )
            {
                int32 nx = pos[i][0],
                      ny = pos[i][1];

                if( ( nx >= 0 ) && ( nx < imageData.size.w ) &&
                    ( ny >= 0 ) && ( ny < imageData.size.h ) )
                {
                    uint32 noffset = ( ny * imageData.stride ) + nx;
                    uint8* mskPixels = (uint8*)maskData.pixelData;
                    uint32* imgPixels = (uint32*)imageData.pixelData;

                    if( passed[noffset] == 0 )
                    {
                        if ( ( mskPixels[noffset] == 0 ) )
                        {
                            stack[nbItems][0] = nx;
                            stack[nbItems][1] = ny;

                            // this is super slow
                            imgPixels[noffset] = iColor;

                            nbItems++;

                            if ( nbItems == maxItems )
                            {
                                maxItems += 0xFFFF;

                                stack = (int32(*)[2]) realloc ( stack, sizeof ( int32 ) * 2 * maxItems );
                            }
                        }

                        passed[noffset] = 1;
                    }
                }
            }

            curItem++;
        }

        free ( stack );
        free ( passed );
    }
}

void
UOdysseyVectorGroupPaint::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();
    BLImage* image = GetRoot()->GetEngine()->GetBLImage();
    BLImage* mask = GetRoot()->GetEngine()->GetBLMask();

    if( mBucketList.size() )
    {
        blctx->begin( *mask );
        blctx->save();
        blctx->resetMatrix();

        for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            UOdysseyVectorObject *child = (*it);

            if( child->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
            {
                UOdysseyVectorPathCubic *cubicPath = Cast<UOdysseyVectorPathCubic>(*it);
                std::list<UOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();

                for( std::list<UOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
                {
                    UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*sit);
                    ::ULIS::FVec2D& point0 = cubicSegment->GetPoint(0)->GetCoords();
                    ::ULIS::FVec2D& point1 = cubicSegment->GetPoint(1)->GetCoords();
                    ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetControlPoint(0)->GetCoords();
                    ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetControlPoint(1)->GetCoords();
                    BLPoint p0 = cubicPath->GetWorldMatrix().mapPoint( point0.x, point0.y );
                    BLPoint p1 = cubicPath->GetWorldMatrix().mapPoint( point1.x, point1.y );
                    BLPoint cp0 = cubicPath->GetWorldMatrix().mapPoint( ctrlPoint0.x, ctrlPoint0.y );
                    BLPoint cp1 = cubicPath->GetWorldMatrix().mapPoint( ctrlPoint1.x, ctrlPoint1.y );
                    BLPath path;

                    path.moveTo( p0.x, p0.x );
                    path.cubicTo( cp0.x, cp0.y
                                , cp1.x, cp1.y
                                ,  p1.x,  p1.y );

                    blctx->setStrokeWidth( 1.0f );
                    blctx->setStrokeAlpha( 1.0f );
                    blctx->strokePath( path );
                }
            }
        }
        blctx->restore();
        blctx->begin( *image );

        for( std::list<FOdysseyVectorBucket*>::iterator it = mBucketList.begin(); it != mBucketList.end(); ++it )
        {
            FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*it);

            floodFill ( bucket->mPosition.x
                      , bucket->mPosition.y
                      , image
                      , mask
                      , bucket->mColor );
        }
    }
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::CopyShape()
{
    return NewObject<UOdysseyVectorGroupPaint>();
}
