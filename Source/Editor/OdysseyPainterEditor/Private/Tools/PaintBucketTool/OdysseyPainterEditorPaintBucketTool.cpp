// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"

#include "OdysseyRasterBlock.h"
#include "HUD/OdysseyVectorHUDBucket.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorPaintBucketTool::~UOdysseyPainterEditorPaintBucketTool()
{
}

UOdysseyPainterEditorPaintBucketTool::UOdysseyPainterEditorPaintBucketTool()
    : mBucketHUD()
    , mPickedBucket( nullptr )
    , mPickedBucketHandle( nullptr )
    , Gradient( false )
    , Color1( 255, 255, 255, 255 )
    , Color2( 255, 255, 255, 255 )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorPaintBucketTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();
        vectorEngine->AddHUD(&mBucketHUD);

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorPaintBucketTool::CanDraw()
{
    return IsActivable();
}

static void floodFill ( int32 x
                      , int32 y
                      , TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iSrcImage
                      , TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iDstImage
                      , TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iMask
                      , ::ULIS::ISample& iColor
                      , uint8 iTolerance ) {
    if ( ( x >= 0 ) && ( x < iSrcImage->Width()  ) &&
         ( y >= 0 ) && ( y < iSrcImage->Height() ) ) {
        uint32 maxItems = 0xFFFF; // stack size for 65k pixels at first. Then we increase by 65k every time needed
        int32 (*stack)[2] = (int32(*)[2]) calloc ( maxItems, sizeof(int32) * 2 );
        // buffer to remember which pixels were already processed
        uint8* passed = (uint8*) calloc ( iSrcImage->Width() * iSrcImage->Height(), sizeof(uint8) );
        uint32 offset = ( y * iSrcImage->Width() ) + x;
        ::ULIS::FColor oldColor = iSrcImage->Color( x, y );
        int16 oldR = oldColor.R8();
        int16 oldG = oldColor.G8();
        int16 oldB = oldColor.B8();
        int16 oldA = oldColor.A8();
        uint32 nbItems = 0;
        uint32 curItem = 0;
        uint32 i;

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

                if ( ( nx >= 0 ) && ( nx < iSrcImage->Width()  ) &&
                     ( ny >= 0 ) && ( ny < iSrcImage->Height() ) ) {
                    uint32 noffset = ( ny * iSrcImage->Width() ) + nx;
                    ::ULIS::FColor curColor = iSrcImage->Color( nx, ny );

                    if ( ( iMask == nullptr ) || *(iMask->PixelBits(nx,ny)) ) {
                        /*if ( ( dstPixel[0] != iNewR ) &&
                             ( dstPixel[1] != iNewG ) &&
                             ( dstPixel[2] != iNewB ) )  {*/
                        if ( passed[noffset] == 0 ) {
                            switch ( iSrcImage->BytesPerPixel() ) {
                                case 3 :
                                case 4 : {
                                    if ( ( curColor.R8() >= ::ULIS::FMath::Max( oldR - iTolerance, 0x00 ) ) &&
                                         ( curColor.R8() <= ::ULIS::FMath::Min( oldR + iTolerance, 0xFF ) ) &&
                                         ( curColor.G8() >= ::ULIS::FMath::Max( oldG - iTolerance, 0x00 ) ) &&
                                         ( curColor.G8() <= ::ULIS::FMath::Min( oldG + iTolerance, 0xFF ) ) &&
                                         ( curColor.B8() >= ::ULIS::FMath::Max( oldB - iTolerance, 0x00 ) ) &&
                                         ( curColor.B8() <= ::ULIS::FMath::Min( oldB + iTolerance, 0xFF ) ) &&
                                         ( curColor.A8() >= ::ULIS::FMath::Max( oldA - iTolerance, 0x00 ) ) &&
                                         ( curColor.A8() <= ::ULIS::FMath::Min( oldA + iTolerance, 0xFF ) ) ) {
                                        stack[nbItems][0] = nx;
                                        stack[nbItems][1] = ny;

                                        // this is super slow
                                        iDstImage->SetPixel( nx, ny, iColor );

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

        free ( stack );
        free ( passed );
    }
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseDownRaster( UOdysseyTextureLayerImageRaster& currentRasterLayer
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    /*::ULIS::FRectI rect = paintBlock->Rect();*/
    ::ULIS::eFormat format = paintBlock->Format();

    /*if (!CanDraw())
        return false;*/

    if (!mPaintEngine.PaintBlock())
        return false;

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

/*	ctx.Fill(*paintBlock, color);*/

    floodFill ( iPointInTexture.x
              , iPointInTexture.y
              , currentRasterLayer.GetRasterBlock()->GetBlock()
              , paintBlock
              , nullptr
              , color
              , Tolerance );

    

	/*ctx.Finish();*/

	paintBlock->Dirty();

    Commit();

    currentRasterLayer.RenderImageChanged( false );

    return true;
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseDownVector( UOdysseyTextureLayerImageVector& currentVectorLayer
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    UOdysseyVectorObject* selectedObject = currentVectorLayer.GetScene()->GetLastSelected();

    if( selectedObject )
    {
        if( selectedObject->GetClass() == UOdysseyVectorGroupPaint::StaticClass() )
        {
            UOdysseyVectorGroupPaint* paintGroup = Cast<UOdysseyVectorGroupPaint>( selectedObject );
            BLPoint localCoords = paintGroup->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            FOdysseyVectorBucket* bucket = paintGroup->PickBucket( localCoords.x, localCoords.y ); 
            uint32 pickedArea = bucket ? bucket->Pick( localCoords.x, localCoords.y ) : 0;

            switch( pickedArea )
            {
                case FOdysseyVectorBucket::PICKBUCKET :
                    mPickedBucket = bucket;
                break;

                case FOdysseyVectorBucket::PICKHANDLE :
                    mPickedBucketHandle = bucket->GetHandle();
                break;

                case FOdysseyVectorBucket::PICKCROSS:
                    paintGroup->RemoveBucket( bucket );
                break;

                default :
                    mPickedBucket = new FOdysseyVectorBucket( *paintGroup, localCoords.x, localCoords.y );

                    paintGroup->AddBucket( mPickedBucket );
                break;
            }

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;
        }
    }

    currentVectorLayer.RenderImageChanged( false );

    return true;
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    mDownMouseX = iPointInTexture.x;
    mDownMouseY = iPointInTexture.y;

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageRaster::StaticClass() )
    {
        UOdysseyTextureLayerImageRaster* currentRasterLayer = Cast<UOdysseyTextureLayerImageRaster>(currentLayer);

        return OnMouseDownRaster( *currentRasterLayer, iPointInTexture, iKey );
    }

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        return OnMouseDownVector( *currentVectorLayer, iPointInTexture, iKey );
    }

    return true;
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseDragVector( UOdysseyTextureLayerImageVector& currentVectorLayer
                                                       , const FOdysseyPoint& iPointInTexture )
{
    if( mPickedBucketHandle )
    {
        FOdysseyVectorBucket* bucket = mPickedBucketHandle->GetParent();
        UOdysseyVectorObject& object = bucket->GetParent();

        if( object.GetClass() == UOdysseyVectorGroupPaint::StaticClass() )
        {
            UOdysseyVectorGroupPaint* paintGroup = Cast<UOdysseyVectorGroupPaint>(&object);
            BLPoint localCoords = paintGroup->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            double difX = localCoords.x - mOldLocalMouseX
                 , difY = localCoords.y - mOldLocalMouseY;

            mPickedBucketHandle->Set( mPickedBucketHandle->GetX() + difX, mPickedBucketHandle->GetY() + difY );

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            currentVectorLayer.RenderImageChanged( true );
        }
    }

    if( mPickedBucket )
    {
        UOdysseyVectorObject& object = mPickedBucket->GetParent();

        if( object.GetClass() == UOdysseyVectorGroupPaint::StaticClass() )
        {
            UOdysseyVectorGroupPaint* paintGroup = Cast<UOdysseyVectorGroupPaint>(&object);
            BLPoint localCoords = paintGroup->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            double difX = localCoords.x - mOldLocalMouseX
                 , difY = localCoords.y - mOldLocalMouseY;
            ::ULIS::FVec2D& bucketCoords = mPickedBucket->GetCoords();

            mPickedBucket->SetCoords( bucketCoords.x + difX, bucketCoords.y + difY );

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            currentVectorLayer.RenderImageChanged( true );
        }
    }
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageRaster::StaticClass() )
    {
        UOdysseyTextureLayerImageRaster* currentRasterLayer = Cast<UOdysseyTextureLayerImageRaster>(currentLayer);

        // no action
    }

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        OnMouseDragVector( *currentVectorLayer, iPointInTexture );
    }
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVector( UOdysseyTextureLayerImageVector& currentVectorLayer
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    UOdysseyVectorObject* selectedObject = currentVectorLayer.GetScene()->GetLastSelected();

    if( selectedObject )
    {
        if( selectedObject->GetClass() == UOdysseyVectorGroupPaint::StaticClass() )
        {
            UOdysseyVectorGroupPaint* paintGroup = Cast<UOdysseyVectorGroupPaint>( selectedObject );
            BLPoint localCoords = paintGroup->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            if( mPickedBucket )
            {
                if( ( static_cast<int>(iPointInTexture.x) == static_cast<int>(mDownMouseX) ) 
                 && ( static_cast<int>(iPointInTexture.y) == static_cast<int>(mDownMouseY) ) )
                {
                    if( Gradient )
                    {
                        mPickedBucket->SetGradient( true );
                        mPickedBucket->SetGradientColors( Color1.R, Color1.G, Color1.B, Color1.A
                                                        , Color2.R, Color2.G, Color2.B, Color2.A );
                    }
                    else
                    {
                        ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
                        ::ULIS::FColor rgba8 = color.ToFormat(::ULIS::eFormat::Format_RGBA8);
                        uint8 R = rgba8.R8();
                        uint8 G = rgba8.G8();
                        uint8 B = rgba8.B8();
                        uint8 A = rgba8.A8();

                        mPickedBucket->SetGradient( false );
                        mPickedBucket->SetColor( R, G, B, A );
                    }
                }
            }

            paintGroup->Colorize();
        }
    }

    mPickedBucketHandle = nullptr;
    mPickedBucket = nullptr;

    currentVectorLayer.RenderImageChanged( false );

    return false;
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        return OnMouseUpVector( *currentVectorLayer, iPointInTexture, iKey );
    }

    return false;
}

void
UOdysseyPainterEditorPaintBucketTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}
