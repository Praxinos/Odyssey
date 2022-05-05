// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Models/OdysseyPainterEditorViewportClient.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "CubemapUnwrapUtils.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Engine/VolumeTexture.h"
#include "ImageUtils.h"
#include "RawMesh.h"
#include "FOdysseySceneViewport.h"
#include "Texture2DPreview.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "UnrealEdGlobals.h"
#include "VolumeTexturePreview.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SViewport.h"

#include "IOdysseyStylusInputModule.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSettings.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"
#include "OdysseyStylusInputSettings.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "OdysseySurface.h"
#include "SOdysseyCursorWidget.h"
#include "SOdysseySurfaceViewport.h"
#include "Models/OdysseyPainterEditorCommands.h"

#include <memory>
#include <chrono>
#include <ULIS>

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportClientt"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportClient::FOdysseyPainterEditorViewportClient( FOdysseyPainterEditor*                    iOdysseyPainterEditor,
                                                                          TWeakPtr< SOdysseySurfaceViewport >       iOdysseyPainterEditorViewport,
                                                                          FOdysseyMeshSelector*                     iMeshSelector)
    : InputSubsystem( nullptr )
    , mLastKey( EKeys::Invalid )
    , mLastEvent( EInputEvent::IE_MAX )
    , mOdysseyPainterEditor(iOdysseyPainterEditor)
    , mOdysseyPainterEditorViewportPtr( iOdysseyPainterEditorViewport )
    , mMeshSelector( iMeshSelector )
    , mCheckerboardTexture( NULL )
    , mCurrentMouseCursor( EMouseCursor::Default )
    , mPivotPointRatio( FVector2D( 0.5, 0.5 ) )
    , mCurrentToolState( eState::kIdle )
    , mIsCapturedByStylus(false)
    , mNearestNeighbourTexture()
    , mBilinearTexture()
    , mIsCurrentModeActive(false)
    , mIsReadyToCreateTool(false)
{
    check( // mOdysseyPainterEditor.IsValid() &&
           mOdysseyPainterEditorViewportPtr.IsValid() );

    InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    //PATCH: as I don't know how to initialize usubsystem inside settings ctor (as usubsystem are called after)
	UOdysseyStylusInputSettings* settings = GetMutableDefault< UOdysseyStylusInputSettings >();
    settings->RefreshStylusInputDriver();
    //PATCH

    InputSubsystem->AddMessageHandler( *this );
    InputSubsystem->OnStylusInputChanged().BindRaw( this, &FOdysseyPainterEditorViewportClient::OnStylusInputChanged );

    ENQUEUE_RENDER_COMMAND(InitOdysseyPainterEditorViewportClientTextures)(
        [&](FRHICommandListImmediate& RHICmdList)
        {
            FSamplerStateInitializerRHI SamplerStateInitializerNN(SF_Point , AM_Clamp, AM_Clamp, AM_Clamp);
            mNearestNeighbourTexture.SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializerNN);
            FSamplerStateInitializerRHI SamplerStateInitializerB(SF_Bilinear , AM_Clamp, AM_Clamp, AM_Clamp);
            mBilinearTexture.SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializerB);
        }
    );
    
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    ModifyCheckerboardTextureColors();
}

FOdysseyPainterEditorViewportClient::~FOdysseyPainterEditorViewportClient( )
{
    InputSubsystem->OnStylusInputChanged().Unbind();
    InputSubsystem->RemoveMessageHandler( *this );

    DestroyCheckerboardTexture();
}

void
FOdysseyPainterEditorViewportClient::OnStylusInputChanged( TSharedPtr<IStylusInputInterfaceInternal> iStylusInput )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FViewportClient API
void
FOdysseyPainterEditorViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
	const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
	ioCanvas->Clear(settings.BackgroundColor);

    IOdysseySurface* surface = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface();
    if (!surface)
        return;

    UTexture* texture       = surface->Texture();
    if (!texture)
        return;

    mNearestNeighbourTexture.TextureRHI = texture->Resource->TextureRHI;
    mBilinearTexture.TextureRHI = texture->Resource->TextureRHI;

    uint32 width = 0;
    uint32 height = 0;
    mOdysseyPainterEditorViewportPtr.Pin()->ComputeTextureDisplayDimensions( width, height );

    double rotation         = FMath::RadiansToDegrees(mOdysseyPainterEditorViewportPtr.Pin()->GetRotation());
    FVector2D pivotPoint = FVector2D(0.5f, 0.5f);

    //if SViewport->GetPan() == 0,0, it means the center of the texture should be centered in the viewport
    FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();
    pan += mOdysseyPainterEditorViewportPtr.Pin()->GetViewportCenter() - (FVector2D(width, height) / 2.0f);
    //FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetTransformToDisplayedTexture().TransformPoint(FVector2D(0, 0));

    // Fully stream in the texture before drawing it.
    if(texture)
    {
        texture->SetForceMipLevelsToBeResident( 30.0f );
        texture->WaitForStreaming();
    }

    // Figure out the size we need
    const float mipLevel = 0; //should be -1, but as we are editing only the first mipmap, then the other mipmaps are not updated and so we cannot use -1 to have automatic mipmap selection.

    TRefCountPtr<FBatchedElementParameters> batchedElementParameters;

    if( GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM5 )
    {
        UTexture2D* texture2D = Cast<UTexture2D>(texture);

        //ODYSSEY: PATCH
        bool isNormalMap = texture->IsNormalMap();
        bool isSingleChannel = texture->CompressionSettings == TC_Grayscale || texture->CompressionSettings == TC_Alpha;
        bool isVirtual = texture->IsCurrentlyVirtualTextured();
        bool isVTSPS = texture2D ? texture2D->IsVirtualTexturedWithSinglePhysicalSpace() : false;
        bool isTextureArray = false;
        float layerIndex = 0.f;
        batchedElementParameters = new FBatchedElementTexture2DPreviewParameters( mipLevel, layerIndex, isNormalMap, isSingleChannel, isVTSPS, isVirtual, isTextureArray );
    }

    FVector2D viewport_center( iViewport->GetSizeXY().X / 2, iViewport->GetSizeXY().Y / 2 );
    //FVector2D position_in_texture = GetLocalMousePosition( viewport_center, false );
    //mPivotPointRatio = FVector2D( position_in_texture.X / texture->GetSurfaceWidth(), position_in_texture.Y / texture->GetSurfaceHeight());

    // Draw background Checker
    {
        FCanvasTileItem tileItem(pan, mCheckerboardTexture->Resource, FVector2D( width, height ), FVector2D( 0.f, 0.f ), FVector2D( width / mCheckerboardTexture->GetSizeX(), height / mCheckerboardTexture->GetSizeY() ), FLinearColor::White );
        tileItem.BlendMode = SE_BLEND_Opaque;
        tileItem.PivotPoint = pivotPoint;
        tileItem.Rotation.Add( 0, rotation, 0 );
        ioCanvas->DrawItem( tileItem );
    }

    // Draw Drawing Surface
    if( texture->Resource != nullptr )
    {
        FCanvasTileItem tileItem(pan, GetZoom() <= 1.0 ? &mBilinearTexture : &mNearestNeighbourTexture, FVector2D( width, height ), FLinearColor::White );
        tileItem.BatchedElementParameters = batchedElementParameters;
        uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
        result += ( 1 << 0 );
        result += ( 1 << 1 );
        result += ( 1 << 2 );
        result += ( 1 << 3 );
        tileItem.BlendMode = (ESimpleElementBlendMode)result;
        tileItem.PivotPoint = pivotPoint;
        tileItem.Rotation.Add( 0, rotation, 0 );
        ioCanvas->DrawItem( tileItem );

        // TODO: Unreal BoxItem doesn't support rotation, so we can't draw it properly. We'll have to come up with our own HUD
        // Draw a white border around the texture to show its extents
        // if( settings.TextureBorderEnabled )
        // {
        //     FCanvasBoxItem boxItem( FVector2D( xPos, yPos ), FVector2D( width , height ) );
        //     boxItem.SetColor( Settings.TextureBorderColor );
        //     Canvas->DrawItem( boxItem );
        // }
    }

    //Tool system HUD drawing
    /*IOdysseySurfaceEditable* HUDSurface = mOdysseyPainterEditor->HUDSystem()->GetHUDSurface();
    UTexture* HUDTexture = nullptr;
    if (HUDSurface)
    {
        HUDTexture = HUDSurface->Texture();
    }
    // Draw HUD Surface
    if( HUDTexture && HUDTexture->Resource )
    {
        /*FTransform2D invertY = FTransform2D(FMatrix2x2(1,0,0,-1));

        FTransform2D transformMinusCenter = FTransform2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewportCenter() * -1).Concatenate(invertY);
        FTransform2D transformAddHalfTexture = FTransform2D( FVector2D(texture->GetSurfaceWidth(), texture->GetSurfaceHeight()) / 2.f );
        FTransform2D transformViewport = mOdysseyPainterEditorViewportPtr.Pin()->GetTransform();
        FTransform2D transform = transformMinusCenter.Concatenate( transformViewport.Inverse() );
        transform.SetTranslation( transform.GetTranslation() + FVector2D(texture->GetSurfaceWidth(), texture->GetSurfaceHeight() * -1) / 2.f );
        transform = transform.Concatenate( invertY );
        transform = transform.Inverse(); */

        /* if( mOdysseyPainterEditor->ToolSystem()->GetSelectedTool() )
            mOdysseyPainterEditor->ToolSystem()->GetSelectedTool()->Draw( HUDSurface->Block(), mOdysseyPainterEditorViewportPtr.Pin()->GetTransform() );
        // mOdysseyPainterEditor->GetGUI()->GetHUDTab()->GetHUD()->Draw( HUDSurface->Block(), transform );
        
        FCanvasTileItem tileItem( FVector2D(0,0), HUDTexture->Resource, FVector2D( iViewport->GetSizeXY().X, iViewport->GetSizeXY().Y ), FLinearColor::White );
        tileItem.BatchedElementParameters = batchedElementParameters;
        uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
        result += ( 1 << 0 );
        result += ( 1 << 1 );
        result += ( 1 << 2 );
        result += ( 1 << 3 );
        tileItem.BlendMode = (ESimpleElementBlendMode)result;
        tileItem.PivotPoint = pivotPoint;
        ioCanvas->DrawItem( tileItem );
    }*/
    //---

    // Draw Cursor Preview
    /* if (mOdysseyPainterEditor->DrawBrushPreview())
    {
        //auto paintEngine = mOdysseyPainterEditor->PaintEngine();
        // paintEngine->UpdateBrushCursorPreview();
        if (paintEngine->mBrushCursorPreviewSurface)
        {
            paintEngine->mBrushCursorPreviewSurface->Texture()->SetForceMipLevelsToBeResident( 30.0f );
            paintEngine->mBrushCursorPreviewSurface->Texture()->WaitForStreaming();
            FVector2D pos_in_viewport( iViewport->GetMouseX(), iViewport->GetMouseY() );
            FVector2D shift = paintEngine->mBrushCursorPreviewShift;
            FVector2D pos = pos_in_viewport + shift * GetZoom();
            FVector2D size = FVector2D( paintEngine->mBrushCursorPreviewSurface->Block()->Width(), paintEngine->mBrushCursorPreviewSurface->Block()->Height() ) * GetZoom();
            FCanvasTileItem cursorItem( pos, paintEngine->mBrushCursorPreviewSurface->Texture()->Resource, size, FLinearColor::White );
            uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
            result += ( 1 << 0 );
            result += ( 1 << 1 );
            result += ( 1 << 2 );
            result += ( 1 << 3 );
            cursorItem.BlendMode = (ESimpleElementBlendMode)result;
            ioCanvas->DrawItem( cursorItem );
        }
    } */

    if( mMeshSelector->GetCurrentMesh() )
    {
        int currentLOD = mMeshSelector->GetCurrentLOD();
        int currentUV = mMeshSelector->GetCurrentUVChannel();
        if( currentLOD >= 0 && currentUV >= 0 )
        {
            FRawMesh rawMesh;
            //ODYSSEY: PATCH
            //mMeshSelector->GetCurrentMesh()->SourceModels[currentLOD].LoadRawMesh(RawMesh);
            mMeshSelector->GetCurrentMesh()->GetSourceModel( currentLOD ).LoadRawMesh( rawMesh );

            FIndexArrayView indexBuffer = mMeshSelector->GetCurrentMesh()->GetRenderData()->LODResources[currentLOD].IndexBuffer.GetArrayView();
            DrawUVsOntoViewport( iViewport, ioCanvas, currentUV, mMeshSelector->GetCurrentMesh()->GetRenderData()->LODResources[0].VertexBuffers.StaticMeshVertexBuffer, indexBuffer );
        }
    }
}

bool
FOdysseyPainterEditorViewportClient::InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad )
{
    //UE_LOG( LogStylusInput, Log, TEXT( "InputKey Key:%s Event:%d" ), *iKey.GetFName().ToString(), iEvent );

    //Cleanup PressedKeys
    TSharedPtr<SOdysseySurfaceViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    for (int i = mKeysPressed.Num() - 1; i >= 0; i--)
    {
        if (!viewportWidget->GetViewport()->KeyState(mKeysPressed[i]))
        {
            mKeysPressed.RemoveAt(i);
        }
    }

    if( iEvent == EInputEvent::IE_Pressed )
    {
        if (!mKeysPressed.Contains(iKey))
            mKeysPressed.Add( iKey );
    }
    else if( iEvent == EInputEvent::IE_Released )
    {
        mKeysPressed.Remove(iKey);
    }

    mLastKey = iKey;
    mLastEvent = iEvent;

    //ToolSystem InputKey
    /*if (mCurrentToolState == eState::kIdle)
    {
        FReply replyHUD = FReply::Unhandled();

        if( mOdysseyPainterEditor->ToolSystem()->GetSelectedTool() )
        {
            //ReplyHUD can be changed in the function below
            mOdysseyPainterEditor->ToolSystem()->GetSelectedTool()->InputKey(iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, replyHUD);
        }
        if ( !mOdysseyPainterEditor->ToolSystem()->GetSelectedTool() && mOdysseyPainterEditor->GetGUISelectedTool() != eGUISelectedTool::kBrush && iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed)
        {
            mIsReadyToCreateTool = true;
            return true;
        }
        if (mOdysseyPainterEditor->ToolSystem()->GetSelectedTool() && mOdysseyPainterEditor->ToolSystem()->GetSelectedTool()->IsReadyToBeApplied())
        {
            ::ULIS::TArray<::ULIS::FVec2I> pointsGenerated = mOdysseyPainterEditor->ToolSystem()->GetSelectedTool()->GenerateToolPoints();
            if( pointsGenerated.Size() > 0 )
            {
                mOdysseyPainterEditor->PaintEngine()->BeginStroke( FOdysseyStrokePoint( pointsGenerated[0].x, pointsGenerated[0].y ), mCurrentPointInTexture );

                for (int i = 1; i < pointsGenerated.Size(); i++)
                {
                    mOdysseyPainterEditor->PaintEngine()->PushStroke( FOdysseyStrokePoint( pointsGenerated[i].x, pointsGenerated[i].y ) );
                }

                mOdysseyPainterEditor->PaintEngine()->EndStroke();
            }
            mOdysseyPainterEditor->ToolSystem()->SetSelectedTool( nullptr );
            //TODO: Need to change for the erasure of a single tool and its HUD, instead of refreshing the whole block
            mOdysseyPainterEditor->HUDSystem()->RefreshHUDSurface( FVector2D( iViewport->GetSizeXY().X, iViewport->GetSizeXY().Y ) );
        }
        if (replyHUD.IsEventHandled())
            return true;
    }*/
    //---

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();

    if( (mIsCapturedByStylus || delta < 500) && ( iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton ) )
        return true;

    FOdysseyPoint point_in_viewport( FOdysseyPoint::DefaultPoint() );
    point_in_viewport.x = iViewport->GetMouseX();
    point_in_viewport.y = iViewport->GetMouseY();
    return InputKeyWithStrokePoint( point_in_viewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad );
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();

    //ToolSystem CapturedMouseMove
    /*if (mCurrentToolState == eState::kIdle)
    {
        if (mIsReadyToCreateTool)
        {
            mIsReadyToCreateTool = false;
            if (mOdysseyPainterEditor->GetGUISelectedTool() != eGUISelectedTool::kBrush)
            {
                FVector2D posInTexture = GetLocalMousePosition( FVector2D( iX, iY ) );
                CreateTool(mOdysseyPainterEditor->GetGUISelectedTool(), posInTexture );
                return;
            }
        }
        if (mOdysseyPainterEditor->ToolSystem()->GetSelectedTool())
        {
            FVector2D posInTexture = GetLocalMousePosition(FVector2D(iX, iY));
            mOdysseyPainterEditor->ToolSystem()->GetSelectedTool()->CapturedMouseMove(iViewport, posInTexture.X, posInTexture.Y);
            return;
        }
    }*/
    //---

    if (mIsCapturedByStylus || delta < 500)
        return;

    FOdysseyPoint point_in_viewport( FOdysseyPoint::DefaultPoint() );
    point_in_viewport.x = iX;
    point_in_viewport.y = iY;
    CapturedMouseMoveWithStrokePoint( point_in_viewport );
}

void
FOdysseyPainterEditorViewportClient::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex )
{
    /*if (mOdysseyPainterEditor->GetGUISelectedTool() != eGUISelectedTool::kBrush)
        return;*/
        
    //If we don't have a surface, then we don't interact with anything
    IOdysseySurface* surface = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface();
    if (!surface)
        return;

    UTexture* texture       = surface->Texture();
    if (!texture)
        return;

    //---

    if( !iWidget.IsValid() )
        return;

    TSharedPtr< SViewport > viewport = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportWidget();
    const SWidget* widget = viewport.Get();

    if( iWidget.Pin().Get() != widget )
        return;

    //---

    /* UE_LOG( LogStylusInput, Log, TEXT("OnStylusStateChanged index:%d x:%f y:%f pressure:%f down:%d tilt:%f %f azimuth:%f altitude:%f"), iIndex, 
            iState.GetPosition().X, iState.GetPosition().Y, 
            iState.GetPressure(), iState.IsStylusDown(), 
            iState.GetTilt().X, iState.GetTilt().Y, 
            iState.GetAzimuth(), iState.GetAltitude() ); */

    float scale_dpi = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D position_in_viewport = widget->GetCachedGeometry().AbsoluteToLocal( iState.GetPosition() ) * scale_dpi;
	
    //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged AbsoluteToLocal: screen:%f %f -> local (ref widget): %f %f" ), iState.GetPosition().X, iState.GetPosition().Y, position_in_viewport.X, position_in_viewport.Y );

    FOdysseyPoint stroke_point( position_in_viewport.X
                                      , position_in_viewport.Y
                                      , iState.GetZ()
                                      , iState.GetPressure()
                                      , iState.GetTimer()
                                      , iState.GetAltitude()
                                      , iState.GetAzimuth()
                                      , iState.GetTwist()
                                      , 0 //iState.GetPitch()
                                      , 0 // iState.GetRoll()
                                      , 0 ); // iState.GetYaw() );

    stroke_point.keysDown = mKeysPressed;
  //---

    static TQueue< FOdysseyPoint > queue;
    static bool stylusWasDown = false;
    static bool is_dragging = false;

    bool isDownEvent = !stylusWasDown && iState.IsStylusDown();
    bool isUpEvent = stylusWasDown && !iState.IsStylusDown();
    bool isMoveEvent = stylusWasDown == iState.IsStylusDown();

    stylusWasDown = iState.IsStylusDown();

    //enqueue stroke points if stylus is down or was down in last event
    //queue will be emptied when a EInputEvent::IE_Released will be received
    /* if (iState.IsStylusDown() || stylusWasDown )
    {
        queue.Enqueue( stroke_point );
    } */

    

    /* if( !iState.IsStylusDown() )
        queue.Enqueue( stroke_point ); */

    
    if( isDownEvent )
    {
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Pressed" ) );
        // is_dragging = true;

        /* FOdysseyPoint point;
        queue.Dequeue( point ); */
        InputKeyWithStrokePoint( stroke_point, 0, EKeys::LeftMouseButton, EInputEvent::IE_Pressed );
        mIsCapturedByStylus = true;

        /* while( queue.Dequeue( point ) ) // Process all the down'd points which occur before having the ue pressed event
        {
            //If stylus is up, then we keep an event for the release
            if (!iState.IsStylusDown() && queue.IsEmpty())
            {
                queue.Enqueue( point );
                break;
            }
            CapturedMouseMoveWithStrokePoint( point );
        } */
    }
    else if( isUpEvent )
    {
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Released" ) );

        InputKeyWithStrokePoint( stroke_point, 0, EKeys::LeftMouseButton, EInputEvent::IE_Released );
        mIsCapturedByStylus = false;

        /* FOdysseyPoint point;
        while( queue.Dequeue( point ) )
        {
            //If stylus is up, then we keep an event for the release
            if (queue.IsEmpty())
            {
                InputKeyWithStrokePoint( point, 0, mLastKey, mLastEvent );
                break;
            }
            CapturedMouseMoveWithStrokePoint( point );
        }
	
		is_dragging = false;*/
    }
    else if( iState.IsStylusDown() ) // MUSTY BE the last, or at least after "is_dragging = false;"
    {
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Dragging" ) );

        CapturedMouseMoveWithStrokePoint( stroke_point );
        
        /* FOdysseyPoint point;
        while( queue.Dequeue( point ) )
        {
            //If stylus is up, then we keep an event for the release
            if (!iState.IsStylusDown() && queue.IsEmpty())
            {
                queue.Enqueue( point );
                break;
            }
            CapturedMouseMoveWithStrokePoint( point );
        } */

    }
    else
    {
		// Register the point in texture when hovering the canvas
        //Point In Viewport
        mCurrentPointInViewport = stroke_point;
        //Point In Texture
		mCurrentPointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Nothing" ) );
    }

    mStylusLastEventTime = std::chrono::steady_clock::now();
    mLastKey = EKeys::Invalid;
    mLastEvent = EInputEvent::IE_MAX;
}

FOdysseyPainterEditorViewportClient::eState
FOdysseyPainterEditorViewportClient::InputChordToState()
{
    if (mKeysPressed.Num() == 0)
        return eState::kIdle;

    FModifierKeysState ModifierKeysState = FSlateApplication::Get().GetModifierKeys();
    const FInputChord activeChord(mKeysPressed[0],
        EModifierKey::FromBools(
            ModifierKeysState.IsControlDown(),
            ModifierKeysState.IsAltDown(),
            ModifierKeysState.IsShiftDown(),
            ModifierKeysState.IsCommandDown()
        )
    );

    if (FOdysseyPainterEditorCommands::Get().PanViewport->HasActiveChord(activeChord))
    {
        return eState::kPan;
    }
    else if (FOdysseyPainterEditorCommands::Get().RotateViewport->HasActiveChord(activeChord))
    {
        return eState::kRotate;
    }
    else if (FOdysseyPainterEditorCommands::Get().ZoomViewport->HasActiveChord(activeChord))
    {
        return eState::kZoom;
    }
    else if (FOdysseyPainterEditorCommands::Get().PickColorInViewport->HasActiveChord(activeChord))
    {
        return eState::kPick;
    }
    return eState::kIdle;
}

// - OnInputEvent

bool
FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint(const FOdysseyPoint& iPointInViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad)
{
    //If we don't have a surface, then we don't interact with anything
    IOdysseySurface* surface = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface();
    if (!surface)
        return false;

    UTexture* texture = surface->Texture();
    if (!texture)
        return false;

    //Point In Viewport
    FOdysseyPoint pointInViewport = iPointInViewport;
    pointInViewport.keysDown = mKeysPressed;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(iPointInViewport);
    pointInTexture.keysDown = mKeysPressed;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    mCurrentPointInTexture = pointInTexture;

    if (mCurrentToolState == eState::kIdle)
    {
        if (mIsCurrentModeActive)
        {
            OnInputEventRaw(iPointInViewport, iKey, iEvent);
            return true;
        }
        
        mCurrentToolState = InputChordToState();

        //If no state becomes active
        if (mCurrentToolState == eState::kIdle)
        {
            OnInputEventRaw(iPointInViewport, iKey, iEvent);
            return true;
        }
        
        ForceKeysUp(iKey);
        OnInputEventWithState(iPointInViewport, iKey, iEvent);
        return true;
    }

    //If mCurrentToolState has an active state (pan, rotate, etc...)
    mCurrentToolState = InputChordToState();
    if (mCurrentToolState != eState::kIdle)
    {
        OnInputEventWithState(iPointInViewport, iKey, iEvent);
        return true;
    }

    //TODO: Force Down for previously pressed keys
    ForceKeysDown(iKey);
    OnInputEventRaw(iPointInViewport, iKey, iEvent);
    return true;
}

void
FOdysseyPainterEditorViewportClient::OnInputEventRaw(const FOdysseyPoint& iPointInViewport, FKey iKey, EInputEvent iEvent)
{
    if (iKey == EKeys::LeftMouseButton)
    {
        if (iEvent == EInputEvent::IE_Pressed)
        {
            //LeftMouse Down
            //mCurrentToolState = eState::kDrawing;
            //mKeysPressed.Contains(EKeys::LeftMouseButton);
            mIsCurrentModeActive = true;
            mOnMouseDown.Broadcast(mCurrentPointInTexture, iKey);
        }
        else if (iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
            mOnMouseUp.Broadcast(mCurrentPointInTexture, iKey);
        }
        return;
    }
    else if (iKey == EKeys::MouseScrollUp && iEvent == EInputEvent::IE_Pressed)
    {
        //ZoomIn
        FVector2D position_in_viewport(iPointInViewport.x, iPointInViewport.y);
        ZoomInInViewport(position_in_viewport);
        return;
    }
    else if (iKey == EKeys::MouseScrollDown && iEvent == EInputEvent::IE_Pressed)
    {
        //ZoomOut
        FVector2D position_in_viewport(iPointInViewport.x, iPointInViewport.y);
        ZoomOutInViewport(position_in_viewport);
        return;
    }
    else if (iEvent == EInputEvent::IE_Pressed)
    {
        //KeyDown
        mOnKeyDown.Broadcast(iKey);
        return;
    }
    else if (iEvent == EInputEvent::IE_Released)
    {
        //KeyUp
        mOnKeyUp.Broadcast(iKey);
        return;
    }
}

void
FOdysseyPainterEditorViewportClient::OnInputEventWithState(const FOdysseyPoint& iPointInViewport, FKey iKey, EInputEvent iEvent)
{
    if( mCurrentToolState == eState::kRotate )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mIsCurrentModeActive = true;

            FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();
            FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            FVector2D deltaCenter = position_in_viewport - center;
            mRotationReference = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
        }
    }
    else if( mCurrentToolState == eState::kPan )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mIsCurrentModeActive = true;

            mPanReference = FVector2D( iPointInViewport.x, iPointInViewport.y );
        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
        }
    }
    else if (mCurrentToolState == eState::kZoom)
    {
        if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed)
        {
            mIsCurrentModeActive = true;

            uint32 width;
            uint32 height;
            mOdysseyPainterEditorViewportPtr.Pin()->ComputeTextureDisplayDimensions(width, height);

            double zoom = mOdysseyPainterEditorViewportPtr.Pin()->GetZoom();
            mZoomReference = ::FMath::Loge(zoom);
            mZoomSizeReference = width;
            mZoomViewportPointReference = FVector2D(iPointInViewport.x, iPointInViewport.y);
            //mZoomTexturePointReference = mOdysseyPainterEditorViewportPtr.Pin()->ToLocal(mZoomViewportPointReference );

        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
        }
    }
    else if( mCurrentToolState == eState::kPick )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mIsCurrentModeActive = true;

            FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
            FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y );
			mOnPickColor.Broadcast(eOdysseyEventState::kAdjust, position_in_texture);

        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
            FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);
            mOnPickColor.Broadcast(eOdysseyEventState::kSet, position_in_texture);

            mIsCurrentModeActive = false;
        }
    }
}

void
FOdysseyPainterEditorViewportClient::ForceKeysUp(FKey iKey)
{
}

void
FOdysseyPainterEditorViewportClient::ForceKeysDown(FKey iKey)
{
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMoveWithStrokePoint( const FOdysseyPoint& iPointInViewport )
{
    //If we don't have a surface, then we don't interact with anything
    IOdysseySurface* surface = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface();
    if (!surface)
        return;

    UTexture* texture       = surface->Texture();
    if (!texture)
        return;

    //Point In Viewport
    FOdysseyPoint pointInViewport = iPointInViewport;
    pointInViewport.keysDown = mKeysPressed;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    pointInTexture.keysDown = mKeysPressed;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    bool hasMoved = long(mCurrentPointInTexture.x) != long(pointInTexture.x) || long(mCurrentPointInTexture.y) != long(pointInTexture.y);
    mCurrentPointInTexture = pointInTexture;

    if( mCurrentToolState == eState::kIdle && mIsCurrentModeActive )
    {
		if (!hasMoved)
			return;

        mOnMouseDrag.Broadcast(mCurrentPointInTexture);
        //mOdysseyPainterEditor->StrokeEngine()->To( mCurrentPointInTexture );
    }
    else if( mCurrentToolState == eState::kPan && mIsCurrentModeActive)
    {
        FVector2D deltaReference( iPointInViewport.x - mPanReference.X, iPointInViewport.y - mPanReference.Y );

        mOdysseyPainterEditorViewportPtr.Pin()->AddPan( deltaReference );
        mPanReference = FVector2D( iPointInViewport.x, iPointInViewport.y );
    }
    else if( mCurrentToolState == eState::kRotate && mIsCurrentModeActive)
    {
        FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

        FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
        FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
        FVector2D deltaCenter = position_in_viewport - center;
        float newRotation = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        float deltaRotation = mRotationReference - newRotation;

        mOdysseyPainterEditorViewportPtr.Pin()->SetRotation( mOdysseyPainterEditorViewportPtr.Pin()->GetRotation() + deltaRotation );

        mRotationReference = newRotation;
    }
    else if (mCurrentToolState == eState::kZoom && mIsCurrentModeActive)
    {   
        TSharedPtr<SOdysseySurfaceViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

        FVector2D viewportMousePosition(iPointInViewport.x, iPointInViewport.y);
        float dist = viewportMousePosition.X - mZoomViewportPointReference.X;
        float smoothness = 200.f; //TODO: do a Setting to let the user change it at will

        if (dist > KINDA_SMALL_NUMBER || dist < KINDA_SMALL_NUMBER)
        {
            float newDist = mZoomReference + (dist / smoothness);
            float zoom = ::FMath::Exp(newDist);
            viewportWidget->SetZoom(zoom, mZoomViewportPointReference - mOdysseyPainterEditorViewportPtr.Pin()->GetViewportCenter());
        }
    }
    else if( mCurrentToolState == eState::kPick && mIsCurrentModeActive)
    {
        FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
        FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);

        if( position_in_texture.X >= 0 && position_in_texture.X < texture->Source.GetSizeX() &&
            position_in_texture.Y >= 0 && position_in_texture.Y < texture->Source.GetSizeY())
        {
			mOnPickColor.Broadcast(eOdysseyEventState::kAdjust, position_in_texture);
        }
    }
}

void
FOdysseyPainterEditorViewportClient::MouseEnter( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mIsCurrentModeActive )
        return;

    mCurrentToolState = eState::kIdle;
}

void
FOdysseyPainterEditorViewportClient::MouseLeave( FViewport* iViewport )
{
    if( mIsCurrentModeActive )
        return;

    mCurrentToolState = eState::kIdle;
}


void
FOdysseyPainterEditorViewportClient::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    //If we don't have a surface, then we don't interact with anything
    IOdysseySurface* surface = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface();
    if (!surface)
        return;

    UTexture* texture       = surface->Texture();
    if (!texture)
        return;

    //Point In Viewport
    FOdysseyPoint pointInViewport = mCurrentPointInViewport;
    pointInViewport.x = iX;
    pointInViewport.y = iY;
    pointInViewport.keysDown = mKeysPressed;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    pointInTexture.keysDown = mKeysPressed;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    bool hasMoved = long(mCurrentPointInTexture.x) != long(pointInTexture.x) || long(mCurrentPointInTexture.y) != long(pointInTexture.y);
    mCurrentPointInTexture = pointInTexture;

    if (mCurrentToolState == eState::kIdle)
    {
        if (!hasMoved)
            return;

        //if (mOdysseyPainterEditor->StrokeEngine()->GetBrushInstance())
            //mOdysseyPainterEditor->StrokeEngine()->GetBrushInstance()->StrokeMoveTo(mCurrentPointInTexture);
        mOnMouseHover.Broadcast(mCurrentPointInTexture);

        //ToolSystem MouseMove
        /*if (mIsReadyToCreateTool)
        {
            mIsReadyToCreateTool = false;
            if( mOdysseyPainterEditor->GetGUISelectedTool() != eGUISelectedTool::kBrush )
            {
                CreateTool(mOdysseyPainterEditor->GetGUISelectedTool(), FVector2D(lastPointInTexture.x, lastPointInTexture.y));
            }
        }
        if (mOdysseyPainterEditor->ToolSystem()->GetSelectedTool())
        {
            FVector2D posInTexture = GetLocalMousePosition(FVector2D(iX, iY));
            mOdysseyPainterEditor->ToolSystem()->GetSelectedTool()->MouseMove(iViewport, posInTexture.X, posInTexture.Y);
        }*/


        //---
    }
}

EMouseCursor::Type
FOdysseyPainterEditorViewportClient::GetCursor( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mCurrentToolState == eState::kPan )
        mCurrentMouseCursor = EMouseCursor::GrabHand;
    else if( mCurrentToolState == eState::kPick )
        mCurrentMouseCursor = EMouseCursor::EyeDropper;
    else
        mCurrentMouseCursor = EMouseCursor::Crosshairs;

    return mCurrentMouseCursor;
}

TOptional< TSharedRef< SWidget > >
FOdysseyPainterEditorViewportClient::MapCursor( FViewport* iViewport, const FCursorReply& iCursorReply )
{
    return FViewportClient::MapCursor( iViewport, iCursorReply );
}

EMouseCaptureMode
FOdysseyPainterEditorViewportClient::GetMouseCaptureMode() const
{
    //No Capture, the capture is managed by FOdysseySceneViewport
    //Because this capture activates HighPrecisionMouseMovements, which is applying acceleration to the mouse (on Mac at least)
    //And we don't want that
    return EMouseCaptureMode::NoCapture;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ FGCObject API
void
FOdysseyPainterEditorViewportClient::AddReferencedObjects( FReferenceCollector& ioCollector )
{
    ioCollector.AddReferencedObject( mCheckerboardTexture );
}

FString FOdysseyPainterEditorViewportClient::GetReferencerName() const
{
    return TEXT("FOdysseyPainterEditorViewportClient");
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
void
FOdysseyPainterEditorViewportClient::ModifyCheckerboardTextureColors()
{
    DestroyCheckerboardTexture();

    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.CheckerColorOne, settings.CheckerColorTwo, settings.CheckerSize );
}

FText
FOdysseyPainterEditorViewportClient::GetDisplayedResolution() const
{
    uint32 height = 1;
    uint32 width = 1;
    mOdysseyPainterEditorViewportPtr.Pin()->ComputeTextureDisplayDimensions( width, height );
    return FText::Format( NSLOCTEXT( "OdysseyPainterEditor",
                                      "DisplayedResolution",
                                      "Displayed: {0}x{1}" ),
                           FText::AsNumber( FMath::Max( uint32( 1 ), width ) ),
                           FText::AsNumber( FMath::Max( uint32( 1 ), height ) ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API

void
FOdysseyPainterEditorViewportClient::DestroyCheckerboardTexture()
{
    if( mCheckerboardTexture )
    {
        if( mCheckerboardTexture->Resource )
        {
            mCheckerboardTexture->ReleaseResource();
        }
        mCheckerboardTexture->MarkPendingKill();
        mCheckerboardTexture = NULL;
    }
}

void
FOdysseyPainterEditorViewportClient::ZoomInInViewport( const FVector2D& iPositionInViewport )
{
    TSharedPtr<SOdysseySurfaceViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

    //If we don't have a surface, then we don't have a local mouse position
    IOdysseySurface* surface = viewportWidget->GetSurface();
    if (!surface)
        return;

    UTexture* texture = surface->Texture();
    if (!texture)
        return;

    int textureWidth = texture->GetSurfaceWidth();
    int textureHeight = texture->GetSurfaceHeight();

    FVector2D pos = iPositionInViewport - viewportWidget->GetViewportCenter();

    viewportWidget->ZoomExponential(viewportWidget->GetZoom(), 0.1, pos);
}

void
FOdysseyPainterEditorViewportClient::ZoomOutInViewport( const FVector2D& iPositionInViewport )
{
    TSharedPtr<SOdysseySurfaceViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

    //If we don't have a surface, then we don't have a local mouse position
    IOdysseySurface* surface = viewportWidget->GetSurface();
    if (!surface)
        return;

    UTexture* texture = surface->Texture();
    if (!texture)
        return;

    int textureWidth = texture->GetSurfaceWidth();
    int textureHeight = texture->GetSurfaceHeight();

    FVector2D pos = iPositionInViewport - viewportWidget->GetViewportCenter();

    viewportWidget->ZoomExponential(viewportWidget->GetZoom(), -0.1, pos);
}

double
FOdysseyPainterEditorViewportClient::GetZoom() const
{
    return mOdysseyPainterEditorViewportPtr.Pin()->GetZoom();
}

FVector2D
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FVector2D& iMouseInViewport ) const
{
    return mOdysseyPainterEditorViewportPtr.Pin()->GetTransformToDisplayedTexture().Inverse().TransformPoint(iMouseInViewport);
}

FOdysseyPoint
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FOdysseyPoint& iPointInViewport ) const
{
    FVector2D position_in_viewport(iPointInViewport.x, iPointInViewport.y);
    FVector2D position_in_texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTransformToSourceTexture().Inverse().TransformPoint(position_in_viewport);
    
    FOdysseyPoint point_in_texture(iPointInViewport);
    point_in_texture.x = position_in_texture.X;
    point_in_texture.y = position_in_texture.Y;

    return point_in_texture;
}

void 
FOdysseyPainterEditorViewportClient::DrawUVsOntoViewport( const FViewport* iViewport, FCanvas* ioCanvas, int32 iUVChannel, const FStaticMeshVertexBuffer& iVertexBuffer, const FIndexArrayView& iIndices )
{
    IOdysseySurface* surface = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface();
    if (!surface)
        return;

    UTexture* texture = surface->Texture();
    if (!texture)
        return;

    if (((uint32)iUVChannel < iVertexBuffer.GetNumTexCoords()))
    {
        TSharedPtr<SOdysseySurfaceViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

        uint32 width, height;
        viewportWidget->ComputeTextureDisplayDimensions(width, height);

        FVector vp1(0.f, 0.f, 0.f);
        FVector vp2(iViewport->GetSizeXY().X, 0.f, 0.f);
        FVector vp3(iViewport->GetSizeXY().X, iViewport->GetSizeXY().Y, 0.f);
        FVector vp4(0.f, iViewport->GetSizeXY().Y, 0.f);        

        FVector2D textureSurfaceSize(texture->GetSurfaceWidth(), texture->GetSurfaceHeight());
        FTransform2D transform = mOdysseyPainterEditorViewportPtr.Pin()->GetTransformToDisplayedTexture();

        //draw triangles
        uint32 numIndices = iIndices.Num();
        for (uint32 i = 0; i < numIndices - 2; i += 3)
        {
            FVector2D UVs[3];
            bool isOutOfBounds[3];

            float fudgeFactor = 1.0f / 1024.0f;
            for (int32 Corner = 0; Corner < 3; Corner++)
            {
                UVs[Corner] = (FVector2D)(iVertexBuffer.GetVertexUV(iIndices[i + Corner], iUVChannel));

                isOutOfBounds[Corner] = (UVs[Corner].X < -fudgeFactor || UVs[Corner].X >(1.0f + fudgeFactor)) || (UVs[Corner].Y < -fudgeFactor || UVs[Corner].Y >(1.0f + fudgeFactor));
            }

            // Clamp the UV triangle to the [0,1] range (with some fudge).
            int32 numUVs = 3;
            fudgeFactor = 0.1f;
            FVector2D bias(0.0f, 0.0f);

            float minU = UVs[0].X;
            float minV = UVs[0].Y;
            for (int32 j = 1; j < numUVs; ++j)
            {
                minU = FMath::Min(minU, UVs[j].X);
                minV = FMath::Min(minU, UVs[j].Y);
            }

            if (minU < -fudgeFactor || minU >(1.0f + fudgeFactor))
            {
                bias.X = FMath::FloorToFloat(minU);
            }
            if (minV < -fudgeFactor || minV >(1.0f + fudgeFactor))
            {
                bias.Y = FMath::FloorToFloat(minV);
            }

            for (int32 j = 0; j < numUVs; j++)
            {
                UVs[j] += bias;
            }

            for (int32 edge = 0; edge < 3; edge++)
            {
                int32 corner1 = edge;
                int32 corner2 = (edge + 1) % 3;
                FLinearColor lc = mMeshSelector->GetMeshColor();
                ::ULIS::FColor c = ::ULIS::FColor::RGBAF( lc.R, lc.G, lc.B, lc.A );

                FLinearColor color = ( isOutOfBounds[corner1] || isOutOfBounds[corner2] ) ? FLinearColor( 0.6f, 0.0f, 0.0f ) : FLinearColor( c.RedF(), c.GreenF(), c.BlueF(), c.AlphaF() );

                FVector pIntersect;
                FVector2D p1 = transform.TransformPoint(UVs[corner1] * textureSurfaceSize - textureSurfaceSize / 2.f);
                FVector2D p2 = transform.TransformPoint(UVs[corner2] * textureSurfaceSize - textureSurfaceSize / 2.f);

                #define V(p) FVector(p, 0.f)
                bool intersect = FMath::SegmentIntersection2D(V(p1), V(p2), vp1, vp2, pIntersect) ||
                                 FMath::SegmentIntersection2D(V(p1), V(p2), vp2, vp3, pIntersect) ||
                                 FMath::SegmentIntersection2D(V(p1), V(p2), vp3, vp4, pIntersect) ||
                                 FMath::SegmentIntersection2D(V(p1), V(p2), vp4, vp1, pIntersect);
                #undef V

                //Clipping. We don't even begin to calculate the coordinates of points we know won't be visible in the viewport
                if ( !intersect &&
                     !(p1.X >= vp1.X && p1.X < vp3.X && p1.Y >= vp1.Y && p1.Y < vp3.Y) &&
                     !(p2.X >= vp1.X && p2.X < vp3.X && p2.Y >= vp1.Y && p2.Y < vp3.Y) )
                {
                    continue;
                }

                FVector origin = FVector(p1, 0.f);
                FVector endPos = FVector(p2, 0.f);

                FBatchedElements* batchedElements = ioCanvas->GetBatchedElements(FCanvas::ET_Line);
                FHitProxyId hitProxyId = ioCanvas->GetHitProxyId();
                batchedElements->AddTranslucentLine(origin, endPos, color, hitProxyId, 1.f);
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE
