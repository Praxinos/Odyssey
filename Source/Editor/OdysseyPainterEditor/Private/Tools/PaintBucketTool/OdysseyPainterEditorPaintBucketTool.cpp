// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorPaintBucketTool::~UOdysseyPainterEditorPaintBucketTool()
{
}

UOdysseyPainterEditorPaintBucketTool::UOdysseyPainterEditorPaintBucketTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorPaintBucketTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
}

bool
UOdysseyPainterEditorPaintBucketTool::CanDraw()
{
    return IsActivable();
}

static void getRGB ( uint8* pixel
                   , uint32 Bpp
                   , int16 *oldR
                   , int16 *oldG
                   , int16 *oldB
                   , int16 *oldA )
{
    switch ( Bpp ) {
        case 3 : {
            *oldR = pixel[0];
            *oldG = pixel[1];
            *oldB = pixel[2];
        } break;

        case 4 : {
            *oldR = pixel[0];
            *oldG = pixel[1];
            *oldB = pixel[2];
            *oldA = pixel[3];
        } break;

        default : 
        break;
    }
}

static void floodFill ( int32 x
                      , int32 y
                      , ::ULIS::FBlock* iImage
                      , ::ULIS::FBlock* iDest
                      , ::ULIS::FBlock* iMask
                      , uint8 iNewR
                      , uint8 iNewG
                      , uint8 iNewB
                      , uint8 iTolerance ) {
    if ( ( x >= 0 ) && ( x < iImage->Width()  ) &&
         ( y >= 0 ) && ( y < iImage->Height() ) ) {
        uint32 maxItems = 0xFFFF; // stack size for 65k pixels at first. Then we increase by 65k every time needed
        int32 (*stack)[2] = (int32(*)[2]) calloc ( maxItems, sizeof(int32) * 2 );
        // buffer to remember which pixels were already processed
        uint8* passed = (uint8*) calloc ( iImage->Width() * iImage->Height(), sizeof(uint8) );
        uint32 offset = ( y * iImage->Width() ) + x;
        uint32 nbItems = 0;
        uint32 curItem = 0;
        int16 oldR;
        int16 oldG;
        int16 oldB;
        int16 oldA;
        uint32 i;

        getRGB( iImage->PixelBits( x, y ), iImage->BytesPerPixel(), &oldR, &oldG, &oldB, &oldA );

        // add first pixel to the stack
        if ( ( iMask == nullptr ) || *(iMask->PixelBits(x,y)) ) {
            stack[nbItems][0] = x;
            stack[nbItems][1] = y;

            nbItems++;
        }

        // process neighboring pixels
        while ( curItem < nbItems ) {
            int32 sx = stack[curItem][0],
                  sy = stack[curItem][1];
            int32 pos[4][2] = {{ sx + 1, sy     },
                               { sx    , sy + 1 },
                               { sx - 1, sy     },
                               { sx    , sy - 1 }};

            for ( i = 0; i < 4; i++ ) {
                int32 nx = pos[i][0],
                      ny = pos[i][1];

                if ( ( nx >= 0 ) && ( nx < iImage->Width()  ) &&
                     ( ny >= 0 ) && ( ny < iImage->Height() ) ) {
                    uint32 noffset = ( ny * iImage->Width() ) + nx;
                    uint8* pixel = iImage->PixelBits( nx, ny );

                    if ( ( iMask == nullptr ) || *(iMask->PixelBits(nx,ny)) ) {
                        if ( passed[noffset] == 0 ) {
                            switch ( iImage->BytesPerPixel() ) {
                                case 3 :
                                case 4 : {
                                    if ( ( pixel[0] >= ::ULIS::FMath::Max( oldR - iTolerance, 0x00 ) ) &&
                                         ( pixel[0] <= ::ULIS::FMath::Min( oldR + iTolerance, 0xFF ) ) &&
                                         ( pixel[1] >= ::ULIS::FMath::Max( oldG - iTolerance, 0x00 ) ) &&
                                         ( pixel[1] <= ::ULIS::FMath::Min( oldG + iTolerance, 0xFF ) ) &&
                                         ( pixel[2] >= ::ULIS::FMath::Max( oldB - iTolerance, 0x00 ) ) &&
                                         ( pixel[2] <= ::ULIS::FMath::Min( oldB + iTolerance, 0xFF ) ) &&
                                         ( pixel[3] >= ::ULIS::FMath::Max( oldA - iTolerance, 0x00 ) ) &&
                                         ( pixel[3] <= ::ULIS::FMath::Min( oldA + iTolerance, 0xFF ) ) ) {
                                        stack[nbItems][0] = nx;
                                        stack[nbItems][1] = ny;

                                        nbItems++;

                                        if ( nbItems == maxItems ) {
                                            maxItems += 0xFFFF;

                                            stack = (int32(*)[2]) realloc ( stack, sizeof ( int32 ) * 2 * maxItems );
                                        }
                                    }
                                } break;

                                default : 
                                break;
                            }

                            passed[noffset] = 1;
                        }
                    }
                }
            }

            curItem++;
        }

        /*** Note: the 2nd step is not necessary and the whole algo could ***/
        /*** fit in one step. this is just for better code clarity ***/
        for ( i = 0; i < nbItems; i++ ) {
            int32 sx = stack[i][0],
                  sy = stack[i][1];
            uint8* pixel = iDest->PixelBits( sx, sy );

            switch ( iDest->BytesPerPixel() ) {
                case 3 : {
                    pixel[0] = iNewR;
                    pixel[1] = iNewG;
                    pixel[2] = iNewB;
                } break;

                case 4 : {
                    pixel[0] = iNewR;
                    pixel[1] = iNewG;
                    pixel[2] = iNewB;
                    pixel[3] = 0xFF/*oldA*/;
                } break;

                default :
                break;
            }
        }

        free ( stack );
        free ( passed );
    }
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageRaster* currentRasterLayer = GetCurrentLayerImageRaster();
	::ULIS::FBlock* paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
   /* ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );*/

    ::ULIS::FRectI rect = paintBlock->Rect();
    ::ULIS::eFormat format = paintBlock->Format();

    /*if (!CanDraw())
        return false;*/

    if (!mPaintEngine.PaintBlock())
        return false;

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

/*	ctx.Fill(*paintBlock, color);*/

    floodFill ( iPointInTexture.x
              , iPointInTexture.y
              , (::ULIS::FBlock*)currentRasterLayer->GetBlock()
              , paintBlock
              , nullptr
              , color.R8(), color.G8(), color.B8(), 0 );

	ctx.Finish();
	paintBlock->Dirty();

    Commit(); */

    return true;
}

void
UOdysseyPainterEditorPaintBucketTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}
