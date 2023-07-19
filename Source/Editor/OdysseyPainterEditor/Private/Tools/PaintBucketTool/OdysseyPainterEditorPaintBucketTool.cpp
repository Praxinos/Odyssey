// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketToolHUD.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketToolContextMenu.h"

#include "OdysseyRasterBlock.h"
#include "Undo/OdysseyVectorUndoBucketAdd.h"
#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorPaintBucketTool::~UOdysseyPainterEditorPaintBucketTool()
{
    delete mBucketHUD;
}

UOdysseyPainterEditorPaintBucketTool::UOdysseyPainterEditorPaintBucketTool()
    : Tolerance( 0 )
    , Propagate( true )
    , Gradient( false )
    , Color1( 255, 255, 255, 255 )
    , Color2( 255, 255, 255, 255 )
    , PickingRadius( 10.0f )
    , ShowControls( false )
    , mPickedBucket( nullptr )
    , mPickedObject( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");

    mBucketHUD = new FOdysseyPainterEditorPaintBucketToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorPaintBucketTool::IsActivable() const
{
    return mToolContext->GetRasterBlock() || mToolContext->CanProvideRasterBlockOnDemand() || mToolContext->GetVectorEngine();
}

void
UOdysseyPainterEditorPaintBucketTool::Unload()
{
    mPaintEngine.RasterBlock(nullptr);

    //TODO: That's wrong
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorPaintBucketTool::UnloadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorPaintBucketTool::Load()
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mToolContext->GetRasterBlock();
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( rasterBlock )
    {
	    mPaintEngine.RasterBlock(rasterBlock);
    }
    else if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorPaintBucketTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorPaintBucketTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mBucketHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorPaintBucketTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    viewportWidget = mToolContext->GetEditor()->GetGUI()->GetViewportTab()->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    iEngine->ClearHUD();
    iEngine->AddHUD( mBucketHUD );

    mBucketHUD->Reset( iScene );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}


bool
UOdysseyPainterEditorPaintBucketTool::OnKeyDown( const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorPaintBucketTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorPaintBucketTool::OnKeyUp( const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorPaintBucketTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                 , const FKey& iKey )
{
    bool ret = false;
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mToolContext->GetRasterBlock();
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();

    if( rasterBlock )
    {
        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDownRaster( rasterBlock->GetBlock(), iPointInTexture, iKey );
    }

    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorPaintBucketTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorPaintBucketTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

bool
UOdysseyPainterEditorPaintBucketTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FKey& iKey )
{
    ShowControlsAtKeyDown =  ShowControls;

    if ( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        ShowControls = true;
    }

    //UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorPaintBucketTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                   , FOdysseyVectorScene* iScene
                                                   , const FKey& iKey )
{
    ShowControls = ShowControlsAtKeyDown;

    //UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
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
UOdysseyPainterEditorPaintBucketTool::OnMouseDownRaster( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    ::ULIS::FColor color = mToolContext->GetEditor()->PaintColor().GetValue();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();
    /*::ULIS::FRectI rect = paintBlock->Rect();*/
    ::ULIS::eFormat format = paintBlock->Format();

    /*if (!CanDraw())
        return false;*/

    if (!paintBlock)
        return false;

    GEditor->BeginTransaction(TEXT("PaintEngine"), LOCTEXT("OnPaintStroke", "FlooFill"), nullptr);

	//::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

/*	ctx.Fill(*paintBlock, color);*/

    floodFill ( iPointInTexture.x
              , iPointInTexture.y
              , iBlock
              , paintBlock
              , nullptr
              , color
              , Tolerance );

	/*ctx.Finish();*/

	paintBlock->Dirty();

    Commit();

    GEditor->EndTransaction();

    return true;
}

// static
bool
UOdysseyPainterEditorPaintBucketTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseDownVectorRotateBucket( FOdysseyVectorScene* iScene
                                                                   , FOdysseyVectorBucket* iBucket )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket->GetOwner(), iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    mDownMouseX = iPointInTexture.x;
    mDownMouseY = iPointInTexture.y;

    mPickedArea = FOdysseyPainterEditorPaintBucketToolHUD::PICK_NONE;

    // Scene
    if( selectedObject == nullptr )
    {
        mPickedBucket = &iScene->GetBackgroundBucket();
        mPickedObject =  iScene;
    }

    // Selected object
    if( selectedObject )
    {
        mPickedObject = selectedObject;

        if( mPickedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( mPickedObject );
            BLPoint localCoords = paintGroup->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            FOdysseyVectorBucket* bucket = mBucketHUD->PickBucket( paintGroup, iPointInTexture.x, iPointInTexture.y ); 

            mPickedArea = FOdysseyPainterEditorPaintBucketToolHUD::PICK_NONE;

            if( bucket )
            {
                mPickedArea = mBucketHUD->PickBucketArea( bucket, iPointInTexture.x, iPointInTexture.y );

                switch( mPickedArea )
                {
                    case FOdysseyPainterEditorPaintBucketToolHUD::PICK_BUCKET :
                        mPickedBucket = bucket;
                        mPointPosition.x = mPickedBucket->GetX();
                        mPointPosition.y = mPickedBucket->GetY();
                    break;

                    case FOdysseyPainterEditorPaintBucketToolHUD::PICK_HANDLE :
                        mPickedBucket = bucket;
                        mPointRotation = mPickedBucket->GetRotation();

                        OnMouseDownVectorRotateBucket( iScene, mPickedBucket );
                    break;

                    default :
                    break;
                }
            }

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;
        }
    }

    iScene->Update( 0 ); // paint group could be invalidated after bucket deletion / adding. Update it.

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    if( selectedObject )
    {
        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( selectedObject );

            mBucketHUD->SetCycle( mBucketHUD->PickCycle( paintGroup
                                                       , iPointInTexture.x
                                                       , iPointInTexture.y ) );
        }
    }

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

double
UOdysseyPainterEditorPaintBucketTool::GetRotationAngle( FOdysseyVectorBucket* iBucket
                                                      , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorObject& ownerObject = iBucket->GetOwner();
    BLMatrix2D& inverseMatrix = ownerObject.GetInverseWorldMatrix();
    double rotation = iBucket->GetRotation();
    ::ULIS::FVec2D bucketVector = ::ULIS::FVec2D( cos( rotation ), sin( rotation ) );
    BLPoint pt = inverseMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FVec2D& pivot = iBucket->GetCoords();
    ::ULIS::FVec2D ptVector;
    double angle = 0.0f;

    ptVector.x = pt.x - pivot.x;
    ptVector.y = pt.y - pivot.y;

    if( ptVector.DistanceSquared() )
    {
        ptVector.Normalize();

        angle = fabs( acos( bucketVector.DotProduct( ptVector ) ) );

        return FOdysseyVector::Cross2D( bucketVector, ptVector ) > 0.0f ? angle : - angle;
    }

    return 0.0f;
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse-click
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mPickedObject )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(mPickedObject);
            BLPoint localCoords = paintGroup->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            FOdysseyVectorPoint* point = nullptr;

            switch( mPickedArea )
            {
                case FOdysseyPainterEditorPaintBucketToolHUD::PICK_BUCKET :
                    point = mPickedBucket;
                break;

                case FOdysseyPainterEditorPaintBucketToolHUD::PICK_HANDLE:
                {
                    double deltaAngle = GetRotationAngle( mPickedBucket, iPointInTexture );

                    mPickedBucket->SetRotation( mPickedBucket->GetRotation() + deltaAngle );
                }
                break;

                default :
                break;
            }

            if( point )
            {
                double difX = localCoords.x - mOldLocalMouseX
                     , difY = localCoords.y - mOldLocalMouseY;

                point->Set( point->GetX() + difX, point->GetY() + difY );
            }

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;
        }
    }

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED ); // update vector scene

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorPaintBucketTool::SetBucketColor( FOdysseyVectorBucket* iBucket )
{
    if( Gradient )
    {
        iBucket->SetGradient( true );
        iBucket->SetGradientColor0( Color1.R, Color1.G, Color1.B, Color1.A );
        iBucket->SetGradientColor1( Color2.R, Color2.G, Color2.B, Color2.A );
    }
    else
    {
        ::ULIS::FColor color = mToolContext->GetEditor()->PaintColor().GetValue();
        UOdysseyPaletteEntry* entry = nullptr;
        if ( mToolContext->GetEditor()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette())
        {
            entry = mToolContext->GetEditor()->GetGUI()->GetColorPaletteTab()->PaletteWidget()->GetColorPalette()->GetPalette()->CurrentEntry.Get();
            if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
            {
                FColor colorEntry = Cast< UOdysseyPaletteEntryColor >(entry)->GetUsedColor();
                color = ::ULIS::FColor::RGBAF(colorEntry.R, colorEntry.G, colorEntry.B, colorEntry.A);
            }
        }

        ::ULIS::FColor rgba8 = color.ToFormat(::ULIS::eFormat::Format_RGBA8);
        uint8 R = rgba8.R8();
        uint8 G = rgba8.G8();
        uint8 B = rgba8.B8();
        uint8 A = rgba8.A8();

        if (entry && entry->IsA(UOdysseyPaletteEntryColor::StaticClass()))
            iBucket->SetPaletteEntry( entry );

        iBucket->SetGradient(false);
        iBucket->SetSolidColor( R, G, B, A );
    }

    iBucket->SetPropagated( Propagate );
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVectorCreateBucket( FOdysseyVectorScene* iScene
                                                                 , FOdysseyVectorGroupPaint* paintGroup
                                                                 , const FOdysseyPoint& iPointInTexture
                                                                 , const FKey& iKey )
{
    FOdysseyVectorCycle* cycle = mBucketHUD->PickCycle( paintGroup, iPointInTexture.x, iPointInTexture.y );
    FOdysseyVectorBucket* cycleBucket = ( cycle ) ? cycle->GetBucket() : nullptr;
    FOdysseyVectorBucket* pickedBucket;

    if( cycleBucket )
    {
        pickedBucket = cycleBucket;

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, pickedBucket->GetOwner(), pickedBucket );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();
    }
    else
    {
        BLMatrix2D& inverseWorldMatrix = paintGroup->GetInverseWorldMatrix();
        BLPoint localCoords = inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );

        pickedBucket = new FOdysseyVectorBucket( *paintGroup, localCoords.x, localCoords.y, Propagate );

        paintGroup->AddBucket( pickedBucket );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketAdd( iScene, paintGroup, pickedBucket );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();
    }

    SetBucketColor( pickedBucket );
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVectorRemoveBucket( FOdysseyVectorScene* iScene
                                                                 , FOdysseyVectorGroupPaint* paintGroup
                                                                 , FOdysseyVectorBucket* iBucket
                                                                 , const FOdysseyPoint& iPointInTexture
                                                                 , const FKey& iKey )
{
    paintGroup->RemoveBucket( iBucket );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( iScene, paintGroup, iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVectorMovePoint( FOdysseyVectorScene* iScene
                                                              , FOdysseyVectorPoint* iPoint
                                                              , ::ULIS::FVec2D iPointOriginalPosition )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene
                                                                      , iPoint
                                                                      , iPointOriginalPosition.x
                                                                      , iPointOriginalPosition.y
                                                                      , 0.0f );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVectorPropagateBucket( FOdysseyVectorScene* iScene
                                                                    , FOdysseyVectorBucket* iBucket
                                                                    , bool iPropagate )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket->GetOwner(), iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iBucket->SetPropagated( iPropagate );
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVectorColorBucket( FOdysseyVectorScene* iScene
                                                                , FOdysseyVectorBucket* iBucket )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket->GetOwner(), iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    SetBucketColor( iBucket );
}

void
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVectorClearBucket( FOdysseyVectorScene* iScene
                                                                , FOdysseyVectorBucket* iBucket )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool","Paint Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( iScene, iBucket->GetOwner(), iBucket );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iBucket->SetSolidColor( 0, 0, 0, 0 );
}

void
UOdysseyPainterEditorPaintBucketTool::PopUpMenu()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
    FOdysseyVectorObject* selectedObject = vectorScene->GetLastSelected();
    TSharedPtr<SWidget> contextMenu = FOdysseyPainterEditorPaintBucketToolContextMenu::CreateWidget( mToolContext.Get() );

    if( selectedObject )
    {
        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            if( paintGroup->GetSelectedBucket() )
            {
                FSlateApplication::Get().PushMenu( GetEditor()->GetGUI()->GetViewportTab().Get()->Widget().ToSharedRef(),
                                                   FWidgetPath(),
                                                   contextMenu.ToSharedRef(),
                                                   FSlateApplication::Get().GetCursorPos(),
                                                   FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
            }
        }
    }
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                               , const FKey& iKey )
{
    bool ret = false;
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mToolContext->GetRasterBlock();
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();

    if( rasterBlock )
    {
        //ret = UOdysseyPainterEditorPaintBucketTool::OnMouseUpRaster( rasterBlock->GetBlock(), iPointInTexture, iKey );
    }

    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    // Right mouse-click
    if( iKey == EKeys::RightMouseButton )
    {
        if( mPickedObject )
        {
            if( mPickedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(mPickedObject);

                switch( mPickedArea )
                {
                    case FOdysseyPainterEditorPaintBucketToolHUD::PICK_BUCKET:
                        paintGroup->SelectBucket( mPickedBucket );

                        PopUpMenu();
                    break;

                    default:
                        paintGroup->SelectBucket( nullptr );
                    break;
                }
            }
        }
    }

    // Left mouse-click
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mPickedObject )
        {
            if( mPickedObject->GetClass() == FOdysseyVectorScene::StaticClass() )
            {
                FOdysseyVectorScene* scene = static_cast<FOdysseyVectorScene*>(mPickedObject);

                if( ( static_cast<int>(iPointInTexture.x) == static_cast<int>(mDownMouseX) ) 
                 && ( static_cast<int>(iPointInTexture.y) == static_cast<int>(mDownMouseY) ) )
                {
                    if ( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
                    {
                        OnMouseUpVectorClearBucket( iScene, mPickedBucket );
                    }
                    else
                    {
                        OnMouseUpVectorColorBucket( iScene, mPickedBucket );
                    }
                }
            }

            if( mPickedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(mPickedObject);

                if( ( static_cast<int>(iPointInTexture.x) == static_cast<int>(mDownMouseX) ) 
                 && ( static_cast<int>(iPointInTexture.y) == static_cast<int>(mDownMouseY) ) )
                {
                    switch( mPickedArea )
                    {
                        case FOdysseyPainterEditorPaintBucketToolHUD::PICK_NONE :
                            OnMouseUpVectorCreateBucket( iScene, paintGroup, iPointInTexture, iKey );
                        break;

                        case FOdysseyPainterEditorPaintBucketToolHUD::PICK_BUCKET:
                            if ( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
                            {
                                OnMouseUpVectorRemoveBucket( iScene, paintGroup, mPickedBucket, iPointInTexture, iKey );
                            }
                            else
                            {
                                OnMouseUpVectorColorBucket( iScene, mPickedBucket );
                            }
                        break;

                        default:
                        break;
                    }
                }
                else
                {
                    switch( mPickedArea )
                    {
                        case FOdysseyPainterEditorPaintBucketToolHUD::PICK_BUCKET :
                            OnMouseUpVectorMovePoint( iScene, mPickedBucket, mPointPosition );
                        break;

                        case FOdysseyPainterEditorPaintBucketToolHUD::PICK_HANDLE:
                        // cf mouse down event
                        break;


                        default :
                        break;
                    }
                }
            }
        }
    }

    mPickedBucket = nullptr;
    mPickedObject = nullptr;

    iScene->Update( 0 );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return false;
}



void
UOdysseyPainterEditorPaintBucketTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}

void
UOdysseyPainterEditorPaintBucketTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // redraw
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName() );
    }
}

void
UOdysseyPainterEditorPaintBucketTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FName& iPropertyName )
{
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
