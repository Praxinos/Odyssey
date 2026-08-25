// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "ArianeEditorTool.h"
#include "ArianeEditorToolInputProcessor.h"
#include "ArianeEditor.h"
#include "ArianeEditorSettings.h"
// Ariane Headers
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianeSegmentCubic.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
// Odyssey
#include "IOdysseyStylusInputModule.h"
#include "OdysseyStylusInputSettings.h"
// Unreal
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Components/LineBatchComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "InputBehavior.h"
#include "InteractiveToolManager.h"
#include "Misc/TransactionObjectEvent.h"
#include "SEditorViewport.h"
#include "Slate/SceneViewport.h"
#include "StylusInput.h"
#include "StylusInputTabletContext.h"

UArianeEditorTool::~UArianeEditorTool()
{
    if( PointQuadTree )
    {
        delete PointQuadTree;
    }
}

UArianeEditorTool::UArianeEditorTool()
    : Editor (nullptr)
    , bHasContextMenu ( false )
    , bInited ( false )
    , PointQuadTree ( nullptr )
    , VertexTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Vertex_Full") ) )
    , VertexContourTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Vertex_Contour") ) )
    , HandleTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Handle_Full") ) )
    , LineOutlinedTexture( LoadObject<UTexture>( nullptr, TEXT("/Odyssey/HUD/T_HUD_Vector_Line_Outlined") ) )
    , Icon ( nullptr )
{
    // For global key events
    InputProcessor = MakeShared<FArianeEditorToolInputProcessor>(this);
}

void
UArianeEditorTool::Setup()
{
    UInteractiveTool::Setup();

    if( bInited == false )
    {
        UClickDragInputBehavior* LeftClickDragInputBehavior = NewObject<UClickDragInputBehavior>(this);
        UMouseHoverBehavior* MouseHoverBehavior = NewObject<UMouseHoverBehavior>(this);

        LeftClickDragInputBehavior->Initialize(this);
        MouseHoverBehavior->Initialize(this);

        AddInputBehavior( LeftClickDragInputBehavior );
        AddInputBehavior( MouseHoverBehavior );

        bInited = true;
    }

    Activate();
}

void
UArianeEditorTool::Shutdown( EToolShutdownType ShutdownType )
{
    Inactivate();
}

void
UArianeEditorTool::Init( FArianeEditor* InEditor )
{
    Editor = InEditor;
}

FArianeEditor*
UArianeEditorTool::GetEditor() const
{
    return Editor;
}

void
UArianeEditorTool::PostInitProperties()
{
    Super::PostInitProperties();
}

void UArianeEditorTool::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
}

void
UArianeEditorTool::Activate()
{
    FEditorViewportClient* viewportClient = GetActiveViewportClient();
    if( viewportClient )
    {
        TSharedPtr< SViewport > viewportWidget = viewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
        if (viewportWidget.IsValid())
        {
            RegisterWindow(viewportWidget.ToSharedRef());
        }
    }

    // register IInputProcessor interface for handling global key press
    FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);

/* Gary
    mCommandList = MakeShared<FUICommandList>();
    BindShortcuts(mCommandList);

    const TSharedRef<FUICommandList> toolkitCommandList = GetEditor()->GetToolkit()->GetToolkitCommands();
    toolkitCommandList->Append(mCommandList.ToSharedRef());
*/
}

void
UArianeEditorTool::Inactivate()
{
    UnregisterWindow();

    // unregister IInputProcessor interface
    FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);

/* Gary
    //mToolContext->OnChanged().RemoveAll(this);
    Flush(); //Finish everything

    mCommandList = nullptr;
*/
}

// Temp
void
UArianeEditorTool::FlushStylusInput()
{
    /*UOdysseyStylusInputSubsystem* InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    if( InputSubsystem )
        InputSubsystem->Flush();*/
}

// Temp
void
UArianeEditorTool::ListenStylusInput()
{
    /*UOdysseyStylusInputSubsystem* InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    if( InputSubsystem )
        InputSubsystem->AddMessageHandler( *this );*/
}

// Temp
void
UArianeEditorTool::IgnoreStylusInput()
{
    /*UOdysseyStylusInputSubsystem* InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    if( InputSubsystem )
        InputSubsystem->RemoveMessageHandler(*this);*/
}

bool
UArianeEditorTool::IsActivable() const
{
    return true;
}

bool
UArianeEditorTool::IsActivated() const
{
    return Editor->GetCurrentTool() == this;
}

bool
UArianeEditorTool::ProcessKeyUpGlobal(const FKeyEvent& InKeyEvent)
{
    return OnKeyUpGlobal(InKeyEvent);
}

bool
UArianeEditorTool::ProcessKeyDownGlobal(const FKeyEvent& InKeyEvent)
{
    return OnKeyDownGlobal(InKeyEvent);
}

bool
UArianeEditorTool::OnKeyDownGlobal(const FKeyEvent& InKeyEvent)
{
    return false; //false means Unreal will continue as if we did nothing
}

bool
UArianeEditorTool::OnKeyUpGlobal(const FKeyEvent& InKeyEvent)
{
    return false; //false means Unreal will continue as if we did nothing
}

bool UArianeEditorTool::OnMouseDown( FEditorViewportClient* iViewportClient
                                   , FSceneView* View
                                   , const FKey& Key
                                   , const FArianePointerState& State
                                   , bool iRepeat )
{
    return false;
}

void
UArianeEditorTool::OnMouseHover( FEditorViewportClient* iViewportClient
                               , FSceneView* View
                               , const FArianePointerState& State )
{
}

bool
UArianeEditorTool::OnMouseDrag( FEditorViewportClient* iViewportClient
                              , FSceneView* View
                              , const FKey& iKey
                              , const FArianePointerState& State )
{
    return false;
}

bool
UArianeEditorTool::OnMouseUp( FEditorViewportClient* iViewportClient
                            , FSceneView* View
                            , const FKey& Key
                            , const FArianePointerState& State )
{
    return false;
}

bool
UArianeEditorTool::OnMouseClick( FEditorViewportClient* iViewportClient
                               , FSceneView* View
                               , const FKey& Key
                               , const FArianePointerState& State )
{
    if( Key == EKeys::RightMouseButton )
    {
        if( bHasContextMenu )
        {
            PopupContextMenu();

            return true;
        }
    }

    return false;
}

bool UArianeEditorTool::OnMouseEnter( FEditorViewportClient* ViewportClient,
                                      FViewport* Viewport,
                                      int32 x,
                                      int32 y )
{
    if (bIsStylusDown)
        return false;

    bIsFocused = true;
    ClearQueue();

    return true;
}

bool UArianeEditorTool::OnMouseLeave( FEditorViewportClient* ViewportClient,
                                      FViewport* Viewport )
{
    if (bIsStylusDown)
        return false;

    bIsFocused = false;

    return true;
}

void
UArianeEditorTool::PopupContextMenu()
{
    TSharedPtr<SWidget> contextMenu = CreateContextMenu();

    TSharedPtr<SWindow> window = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (!window)
        return;

    FSlateApplication::Get().PushMenu( window.ToSharedRef(),
                                       FWidgetPath(),
                                       contextMenu.ToSharedRef(),
                                       FSlateApplication::Get().GetCursorPos(),
                                       FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
}

TSharedPtr<SWidget>
UArianeEditorTool::CreateContextMenu()
{
    FMenuBuilder menu( true, CommandList );

    menu.BeginSection("Context Menu");
    if( bHasContextMenu )
        ExtendContextMenu( menu );
    menu.EndSection();

    return menu.MakeWidget();
}

void
UArianeEditorTool::ExtendContextMenu( FMenuBuilder& menu )
{
}

FText
UArianeEditorTool::GetTooltip() const
{
    return FText();
}

void
UArianeEditorTool::PropertyChanged(const FName& iPropertyName)
{
}

void
UArianeEditorTool::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
}

void
UArianeEditorTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty( PropertyChangedEvent );

    PropertyChanged( PropertyChangedEvent.GetPropertyName()
                   , PropertyChangedEvent.GetMemberPropertyName()
                   , PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );

    PostPropertyChanged( PropertyChangedEvent.GetMemberPropertyName()
                       , PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );
}

void
UArianeEditorTool::PropertyChanged( const FName& iPropertyName
                                  , const FName& iMemberPropertyName
                                  , bool iIsInteractive )
{
    if (iIsInteractive)
        return;

    PropertyChanged(iPropertyName);
}

void
UArianeEditorTool::PostTransacted( const FTransactionObjectEvent& iTransactionEvent )
{
    Super::PostTransacted(iTransactionEvent);

/* Gary
    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName, propertyName, false);
        FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
            [this, propertyName](bool iIsRedo)
            {
                PostPropertyChanged(propertyName, false);
            }
        );
    }
*/
}

void
UArianeEditorTool::DrawLayerOrientationGrid( IToolsContextRenderAPI* RenderAPI, UArianeLayerDrawing* DrawingLayer )
{
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();
    double GridSize = Settings->GetGridSize();
    double GridOpacity = Settings->GetGridOpacity();
    FLinearColor GridColor = Settings->GetGridColor().CopyWithNewOpacity( GridOpacity );
    FLinearColor GridXAxisColor = Settings->GetGridXAxisColor().CopyWithNewOpacity( GridOpacity );
    FLinearColor GridYAxisColor = Settings->GetGridYAxisColor().CopyWithNewOpacity( GridOpacity );

    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    IToolsContextQueriesAPI* QueriesAPI = GetToolManager()->GetContextQueriesAPI();
    FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
    ULineBatchComponent* LineBatcher = GetWorld()->GetLineBatcher( UWorld::ELineBatcherType::World );
    FTransform LayerTransform = DrawingLayer->GetComponentTransform();
    FMatrix LayerMatrix = LayerTransform.ToMatrixWithScale();;
    FMatrix WorldMatrix;
    float AdjustedThickness = 2.0f;
    FViewCameraState CameraState;
    float OriX, OriY;
    float EndX, EndY;

    QueriesAPI->GetCurrentViewState( CameraState );

    // Adjust line thickness relative to camera distance
    float Distance = FVector::Dist( CameraState.Position, DrawingLayer->GetComponentLocation() );

    double GridRadius = GridSize * 0.5f;
    static uint32 StepCount = 20, StepCountHalf = StepCount / 2;
    double Step = ( GridSize ) / StepCount;

    AdjustedThickness = ( Distance / 1000.0f ) * AdjustedThickness;

    switch( Editor->GetLayerDrawingOrientation( DrawingLayer ) )
    {
        case EArianeLayerDrawingOrientation::LayerXY:
            WorldMatrix = LayerMatrix;
        break;

        case EArianeLayerDrawingOrientation::LayerYZ:
        {
            // Note: args are Pitch(Y) Yaw(Z) Roll(X)
            // but rotation order is Yaw (Z) Pitch (Y) Roll (X)
            FMatrix YZRotation = FRotationMatrix( FRotator(  0.f, 90.f, 90.f ) );

            WorldMatrix = YZRotation * LayerMatrix;
        }
        break;

        case EArianeLayerDrawingOrientation::LayerZX:
        {
            // Note: args are Pitch(Y) Yaw(Z) Roll(X)
            // but rotation order is Yaw (Z) Pitch (Y) Roll (X)
            FMatrix ZXRotation = FRotationMatrix( FRotator(  0.f,  0.f, 90.f ) );

            WorldMatrix = ZXRotation * LayerMatrix;
        }
        break;

        case EArianeLayerDrawingOrientation::View :
        {

/* Commented out: CameraState isn't up to date at first. We need to click at least once in the viewport, which is not
 * what we want. we use FSceneView instead
            FVector CamForward = CameraState.Orientation.GetForwardVector();
            FVector CamUp = CameraState.Orientation.GetUpVector();
            FVector CamRight = CameraState.Orientation.GetRightVector();
            FMatrix ViewAlignedRot = FMatrix( CamUp, CamRight, -CamForward, FVector::ZeroVector );
            FTransform LayerTranslationTransform;

            LayerTranslationTransform.SetTranslation( DrawingLayer->GetComponentLocation() );

            WorldMatrix = ViewAlignedRot * LayerTranslationTransform.ToMatrixNoScale();
*/
            FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                                    , ViewportClient->GetScene()
                                                                                    , ViewportClient->EngineShowFlags ) );
            // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
            FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );

            FVector CamForward = View->GetViewDirection();
            FVector CamUp = View->GetViewUp();
            FVector CamRight = View->GetViewRight();
            FMatrix ViewAlignedRot = FMatrix( CamUp, CamRight, -CamForward, FVector::ZeroVector );
            FTransform LayerTranslationTransform;

            LayerTranslationTransform.SetTranslation( DrawingLayer->GetComponentLocation() );

            WorldMatrix = ViewAlignedRot * LayerTranslationTransform.ToMatrixNoScale();
            // --- End
        }
        break;

        default :
        break;
    }

    // vertical lines
    OriX = -GridRadius;
    OriY = -GridRadius;
    EndY =  GridRadius;
    for ( uint32 i = 0; i <= StepCount; i++ )
    {
        EndX = OriX;

        FVector Origin = FVector( OriX, OriY, 0.0f );
        FVector EndPos = FVector( EndX, EndY, 0.0f );

        PDI->DrawTranslucentLine( WorldMatrix.TransformPosition( Origin )
                                , WorldMatrix.TransformPosition( EndPos )
                                , ( i == StepCountHalf ) ? GridXAxisColor : GridColor
                                , SDPG_Foreground // SDPG_World
                                , AdjustedThickness
                                , 0.0f ); // Lifetime 1 frame

        OriX += Step;
    }

    // horizontal lines
    OriX = -GridRadius;
    OriY = -GridRadius;
    EndX =  GridRadius;
    for ( uint32 i = 0; i <= StepCount; i++ )
    {
        EndY = OriY;

        FVector Origin = FVector( OriX, OriY, 0.0f );
        FVector EndPos = FVector( EndX, EndY, 0.0f );

        PDI->DrawTranslucentLine( WorldMatrix.TransformPosition( Origin )
                                , WorldMatrix.TransformPosition( EndPos )
                                , ( i == StepCountHalf ) ? GridYAxisColor : GridColor
                                , SDPG_Foreground // SDPG_World
                                , AdjustedThickness
                                , 0.0f ); // Lifetime 1 frame

        OriY += Step;
    }
}

void
UArianeEditorTool::Render(IToolsContextRenderAPI* RenderAPI)
{
}

void
UArianeEditorTool::DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
{
}

bool
UArianeEditorTool::SupportsColorType( EOdysseyPainterEditorColorType ColorType )
{
    return false;
}

bool
UArianeEditorTool::GetCursor( EMouseCursor::Type& OutCursor )
{
    if( CanDraw() == false )
    {
        OutCursor = EMouseCursor::Type::SlashedCircle;

        return true;
    }

    return false;
}

void UArianeEditorTool::OnTick(float DeltaTime)
{
    if (bIsRecordingStylus)
        ReadStylusInput(eStylusEventFence::kStylusUp);
}

void UArianeEditorTool::OnPacket( const UE::StylusInput::FStylusInputPacket& iPacket,
                                  UE::StylusInput::IStylusInputInstance* iInstance )
{
    StylusLastEventTime = std::chrono::steady_clock::now();

    // FIX: MOVE WINTAB COORDINATES WHEN MAIN SCREEN IS NOT ON THE (TOP) LEFT OF USER PHYSICAL DESKTOP - AWAITING FOR EPIC PULL REQUEST VALIDATION
#if PLATFORM_WINDOWS
    const UOdysseyStylusInputSettings* settings = GetDefault<UOdysseyStylusInputSettings>();
    FName selectedAPI = settings->StylusInputDriver;
    if (selectedAPI == "Wintab")
    {
        UE::StylusInput::FStylusInputPacket packetCopyWin = iPacket;

        if (iPacket.NormalPressure == 0)
        {
            mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
            if (mCurrentPacketType == UE::StylusInput::EPacketType::OnDigitizer)
                mCurrentPacketType = UE::StylusInput::EPacketType::StylusUp;
            else
                mCurrentPacketType = UE::StylusInput::EPacketType::AboveDigitizer;
        }

        if (iPacket.NormalPressure != 0)
        {
            mCurrentPenStatus = mCurrentPenStatus | UE::StylusInput::EPenStatus::CursorIsTouching;
            if (mCurrentPacketType != UE::StylusInput::EPacketType::OnDigitizer && mCurrentPacketType != UE::StylusInput::EPacketType::StylusDown)
                mCurrentPacketType = UE::StylusInput::EPacketType::StylusDown;
            else
                mCurrentPacketType = UE::StylusInput::EPacketType::OnDigitizer;
        }
        else
        {
            mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
        }

        packetCopyWin.PenStatus = mCurrentPenStatus;
        packetCopyWin.Type = mCurrentPacketType;

        if (packetCopyWin.Type == UE::StylusInput::EPacketType::StylusDown && mEventsConsumedSinceLastUp == 0)
            ClearQueue();

        mPacketQueue.Enqueue(packetCopyWin);
        return;
    }
#endif
    // FIX: MOVE WINTAB COORDINATES WHEN MAIN SCREEN IS NOT ON THE (TOP) LEFT OF USER PHYSICAL DESKTOP - AWAITING FOR EPIC PULL REQUEST VALIDATION

    // FIX: HAVE TO MANUALLY HANDLE UP AND DOWN UNTIL EPIC ACCEPT INTERNAL PULL REQUEST
#if PLATFORM_MAC
    UE::StylusInput::FStylusInputPacket packetCopyMac = iPacket;

    if (iPacket.NormalPressure == 0)
    {
        mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
        if (mCurrentPacketType == UE::StylusInput::EPacketType::OnDigitizer)
            mCurrentPacketType = UE::StylusInput::EPacketType::StylusUp;
        else
            mCurrentPacketType = UE::StylusInput::EPacketType::AboveDigitizer;
    }

    if (iPacket.NormalPressure != 0)
    {
        mCurrentPenStatus = mCurrentPenStatus | UE::StylusInput::EPenStatus::CursorIsTouching;
        if (mCurrentPacketType != UE::StylusInput::EPacketType::OnDigitizer && mCurrentPacketType != UE::StylusInput::EPacketType::StylusDown)
            mCurrentPacketType = UE::StylusInput::EPacketType::StylusDown;
        else
            mCurrentPacketType = UE::StylusInput::EPacketType::OnDigitizer;
    }
    else
    {
        mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
    }

    packetCopyMac.PenStatus = mCurrentPenStatus;
    packetCopyMac.Type = mCurrentPacketType;

    if (packetCopyMac.Type == UE::StylusInput::EPacketType::StylusDown && mEventsConsumedSinceLastUp == 0)
        ClearQueue();

    mPacketQueue.Enqueue(packetCopyMac);
    return;
#else// FIX: HAVE TO MANUALLY HANDLE UP AND DOWN UNTIL EPIC ACCEPT INTERNAL PULL REQUEST

    if (iPacket.Type == UE::StylusInput::EPacketType::StylusDown && mEventsConsumedSinceLastUp == 0)
    {
        ClearQueue();
    }
    mPacketQueue.Enqueue(iPacket);
#endif
}

void UArianeEditorTool::StartStylusInputRecord(const FKey& iMouseButton)
{
    if (bIsRecordingStylus)
        return;

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - StylusLastEventTime).count();
    if (delta > 500)
        return;

    bIsRecordingStylus = true;
    PressedKey = iMouseButton;
}

void UArianeEditorTool::StopStylusInputRecord()
{
    if (!bIsRecordingStylus)
        return;

    ClearQueue();

    bIsRecordingStylus = false;
    PressedKey = FKey();
}

void
UArianeEditorTool::ReadStylusInput( eStylusEventFence iUntilEventType )
{
    if (!bIsFocused || mPacketQueue.Num() == 0)
    {
        ClearQueue();
        return;
    }

    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    UE::StylusInput::FStylusInputPacket packet;

    while (mPacketQueue.Dequeue(packet))
    {
        FArianePointerState arianePointerState = StylusPacketToArianePointerState(packet);

        if (packet.Type == UE::StylusInput::EPacketType::StylusDown && CanDraw())
        {
            //MouseDown
            bIsStylusDown = true;
            OnMouseDown(ViewportClient, View, PressedKey, arianePointerState);
            mEventsConsumedSinceLastUp++;

            if (iUntilEventType == eStylusEventFence::kStylusDown)
                return;
        }
        else if (packet.Type == UE::StylusInput::EPacketType::StylusUp && CanDraw())
        {
            //MouseUp
            OnMouseUp(ViewportClient, View, PressedKey, arianePointerState);
            bIsStylusDown = false;
            mEventsConsumedSinceLastUp = 0;
            StopStylusInputRecord();

            if (iUntilEventType == eStylusEventFence::kStylusUp)
                return;
        }
        else if (bIsStylusDown && CanDraw())
        {
            OnMouseDrag(ViewportClient, View, PressedKey, arianePointerState);
            mEventsConsumedSinceLastUp++;
        }
    }
    return;
}

FArianePointerState
UArianeEditorTool::StylusPacketToArianePointerState(const UE::StylusInput::FStylusInputPacket& iPacket)
{
    FEditorViewportClient* viewportClient = GetActiveViewportClient();
    if (!viewportClient)
        return FArianePointerState();

    TSharedPtr< SViewport > viewportWidget = viewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
    if (!viewportWidget)
        return FArianePointerState();

    TSharedPtr<SWindow> Window = mStylusInputWindow.Pin();

    //Init our ArianePointer, having all the basic info to draw
    float scaleDPI = viewportWidget->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D positionInViewport = viewportWidget->GetCachedGeometry().AbsoluteToLocal(FVector2D(iPacket.X, iPacket.Y)) * scaleDPI;
    positionInViewport += Window->GetRectInScreen().GetTopLeft();

    const UE::StylusInput::IStylusInputTabletContext* tabletContext = GetTabletContext(mStylusInputInstance, iPacket.TabletContextID);
    FArianePointerState arianePointerState;

    if (tabletContext)
    {
        UE::StylusInput::ETabletSupportedProperties capabilities = tabletContext->GetSupportedProperties();
        arianePointerState.ViewportX = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::X) ? positionInViewport.X : 0.f;
        arianePointerState.ViewportY = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::Y) ? positionInViewport.Y : 0.f;
        arianePointerState.Z = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::Z) ? iPacket.Z : 0.f;
        arianePointerState.Pressure = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::NormalPressure) ? iPacket.NormalPressure : 1.f;
        arianePointerState.Timer = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::TimerTick) ? iPacket.TimerTick : 1.f;
        arianePointerState.Altitude = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::AltitudeOrientation) ? iPacket.AltitudeOrientation : 1.f;
        arianePointerState.Azimuth = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::AzimuthOrientation) ? iPacket.AzimuthOrientation : 1.f;
        arianePointerState.Twist = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::TwistOrientation) ? iPacket.TwistOrientation : 1.f;
        //arianePointerState.mPoint.pitch = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::PitchRotation) ? iPacket.PitchRotation : 1.f;
        //arianePointerState.mPoint.roll = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::RollRotation) ? iPacket.RollRotation : 1.f;
        //arianePointerState.mPoint.yaw = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::YawRotation) ? iPacket.YawRotation : 1.f;
    }

    return arianePointerState;
}

FEditorViewportClient*
UArianeEditorTool::GetActiveViewportClient()
{
    FViewport* ActiveViewport = GEditor->GetActiveViewport();

    // ActiveViewport can be nullptr when closing the editor
    return ActiveViewport ? static_cast<FEditorViewportClient*>(ActiveViewport->GetClient()) : nullptr;
}

// IHoverBehaviorTarget interface override
FInputRayHit
UArianeEditorTool::BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos)
{
    FInputRayHit DummyHit;

    DummyHit.bHit = true;
    DummyHit.HitDepth = 1000000.0f;

    return DummyHit;
}

// IHoverBehaviorTarget interface override
void
UArianeEditorTool::OnBeginHover( const FInputDeviceRay& DevicePos )
{
    //FEditorViewportClient* ViewportClient = GetActiveViewportClient();
}

// IHoverBehaviorTarget interface override
bool
UArianeEditorTool::OnUpdateHover( const FInputDeviceRay& DevicePos )
{
    bIsFocused = true;

    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );

    OnMouseHover( ViewportClient
                , View
                , FArianePointerState( DevicePos.ScreenPosition.X
                                     , DevicePos.ScreenPosition.Y ) );

    return true;
}

// IHoverBehaviorTarget interface override
void
UArianeEditorTool::OnEndHover()
{
    //FEditorViewportClient* ViewportClient = GetActiveViewportClient();
}

// Implements IClickDragBehaviorTarget::CanBeginClickSequence
FInputRayHit
UArianeEditorTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
    FInputRayHit DummyHit;

    DummyHit.bHit = true;
    DummyHit.HitDepth = 1000000.0f;

    // On mémorise quel bouton commence le drag
    if ( FSlateApplication::Get().GetPressedMouseButtons().Contains(EKeys::LeftMouseButton))
    {
        PressedKey = EKeys::LeftMouseButton;
    }

    if ( FSlateApplication::Get().GetPressedMouseButtons().Contains(EKeys::MiddleMouseButton))
    {
        PressedKey = EKeys::MiddleMouseButton;
    }

    if (FSlateApplication::Get().GetPressedMouseButtons().Contains(EKeys::RightMouseButton))
    {
        PressedKey = EKeys::RightMouseButton;
    }

    return DummyHit;
}

FPlane
UArianeEditorTool::GetDrawingPlane( FEditorViewportClient* ViewportClient, UArianeLayerDrawing* DrawingLayer )
{
    IToolsContextQueriesAPI* QueriesAPI = GetToolManager()->GetContextQueriesAPI();
    const FTransform& LayerWorldTransform = DrawingLayer->GetComponentTransform();
    FVector LayerWorldPosition = DrawingLayer->GetComponentLocation();
    FViewCameraState CameraState;

    QueriesAPI->GetCurrentViewState( CameraState );

    FVector CameraLocation = CameraState.Position;
    FVector CameraDirection = CameraState.Orientation.GetForwardVector();
    FVector PlaneWorldDirection = FVector( 0.0f, 0.0f, 0.0f ) ;

    switch ( Editor->GetLayerDrawingOrientation( DrawingLayer ) )
    {
        case EArianeLayerDrawingOrientation::LayerXY :
        PlaneWorldDirection = LayerWorldTransform.TransformVector( FVector( 0.0f, 0.0f, 1.0f ) );

        break;

        case EArianeLayerDrawingOrientation::LayerYZ :
        PlaneWorldDirection = LayerWorldTransform.TransformVector( FVector( 1.0f, 0.0f, 0.0f ) );

        break;

        case EArianeLayerDrawingOrientation::LayerZX :
        PlaneWorldDirection = LayerWorldTransform.TransformVector( FVector( 0.0f, 1.0f, 0.0f ) );

        break;

        default : // View
            PlaneWorldDirection = -CameraDirection;
        break;
    }

    return FPlane( LayerWorldPosition, PlaneWorldDirection );
}

// Helper function
UArianeLayer*
UArianeEditorTool::GetCurrentLayer()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    UArianeLayerStack* LayerStack = Painting3DComponent ? Painting3DComponent->GetLayerStack()
                                                        : nullptr;
    UArianeLayer* CurrentLayer = LayerStack ? LayerStack->GetCurrentLayer()
                                            : nullptr;

    return CurrentLayer;
}

bool
UArianeEditorTool::CanDraw()
{
    UArianeLayer* CurrentLayer = GetCurrentLayer();

    return ( CurrentLayer && ( CurrentLayer->IsLocked( true ) == false ) ) ? true
                                                                           : false;
};

// Implements IClickDragBehaviorTarget::OnClickPress
void
UArianeEditorTool::OnClickPress( const FInputDeviceRay& PressPos )
{
    StartStylusInputRecord(PressedKey);

    if (bIsRecordingStylus)
    {
        ReadStylusInput();
    }
    else
    {
        FEditorViewportClient* ViewportClient = GetActiveViewportClient();
        FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                                , ViewportClient->GetScene()
                                                                                , ViewportClient->EngineShowFlags ) );
        // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
        FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
        FArianePointerState Pointerstate = FArianePointerState( PressPos.ScreenPosition.X
                                                              , PressPos.ScreenPosition.Y );

        if (CanDraw())
            OnMouseDown( ViewportClient
                       , View
                       , PressedKey
                       , Pointerstate);
    }
}

// Implements IClickDragBehaviorTarget::OnClickDrag
void
UArianeEditorTool::OnClickDrag( const FInputDeviceRay& DragPos )
{
    if (bIsRecordingStylus)
    {
        ReadStylusInput();
    }
    else
    {
        FEditorViewportClient* ViewportClient = GetActiveViewportClient();
        FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                                , ViewportClient->GetScene()
                                                                                , ViewportClient->EngineShowFlags ) );
        // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
        FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
        FArianePointerState Pointerstate = FArianePointerState( DragPos.ScreenPosition.X
                                                              , DragPos.ScreenPosition.Y);

        if (CanDraw())
            OnMouseDrag( ViewportClient
                       , View
                       , PressedKey
                       , Pointerstate );
    }
}

// Implements IClickDragBehaviorTarget::OnClickRelease
void
UArianeEditorTool::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
    if (bIsRecordingStylus)
    {
        ReadStylusInput(eStylusEventFence::kStylusUp);
    }
    else
    {
        FEditorViewportClient* ViewportClient = GetActiveViewportClient();
        FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                                , ViewportClient->GetScene()
                                                                                , ViewportClient->EngineShowFlags ) );
        // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
        FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
        FArianePointerState Pointerstate = FArianePointerState( ReleasePos.ScreenPosition.X
                                                              , ReleasePos.ScreenPosition.Y );

        if (CanDraw())
            OnMouseUp( ViewportClient
                     , View
                     , PressedKey
                     , Pointerstate);
    }
}

FVector2D
UArianeEditorTool::WorldToPlane( const FVector& WorldPosition, const FPlane& ProjectionPlane )
{
    FVector ProjectionPlaneOrigin = ProjectionPlane.GetOrigin();
    // find the angle between the Z axis and the plane's normal vector in order to find the rotation matrix.
    // we will then use it to convert 3D points coordinates in a 2D coordinate system (with Z = 0).
    FQuat WorldToPlaneRotationQuat = FQuat::FindBetweenNormals( ProjectionPlane.GetNormal(), FVector::UpVector );

    return FVector2D( WorldToPlaneRotationQuat * ( WorldPosition - ProjectionPlaneOrigin ) );
}

// static
bool
UArianeEditorTool::WorldToHUD( FEditorViewportClient* ViewportClient
                             , FSceneView* View
                             , const FVector& WorldPosition
                             , FVector2D& OutHUDPosition )
{
    FVector2D ScreenPosition;
    float Dot = ( WorldPosition - ViewportClient->GetViewLocation() ).Dot( View->GetViewDirection() );

    if( Dot > 0.0f ) // in front of the camera
    {
        if( WorldToPixel( View, WorldPosition, ScreenPosition ) )
        {
            OutHUDPosition = ScreenToHUD ( ViewportClient, ScreenPosition );

            return true;
        }
    }

    return false;
}

// static
bool
UArianeEditorTool::WorldToPixel( FSceneView* View, const FVector& WorldPosition, FVector2D& OutHUDPosition )
{
    return View->WorldToPixel( WorldPosition, OutHUDPosition );
}

// static
FVector2D
UArianeEditorTool::ScreenToHUD( FEditorViewportClient* ViewportClient, const FVector2D& ScreenPosition )
{
    float DPIScale = ViewportClient->GetDPIScale();
    double X = ScreenPosition.X / DPIScale;
    double Y = ScreenPosition.Y / DPIScale;

    return FVector2D( X, Y );
}

void
UArianeEditorTool::OnTerminateDragSequence()
{
}

// Picking
UArianeEditorTool::FPointQuadTree::~FPointQuadTree()
{
    for( int i = 0 ; i < 4; i++ )
    {
        if( Children[i] )
        {
            delete Children[i];
        }
    }
}

UArianeEditorTool::FPointQuadTree::FPointQuadTree( const FIntRect& InRect
                                                 , uint32 MaxPointsPerQuad
                                                 , TArray<FPointQuadTreeEntry>& PointQuadTreeEntries
                                                 , uint32 Depth
                                                 , uint32 MaxDepth )
    : Children { nullptr, nullptr, nullptr, nullptr }
    , Rect( InRect )
{
    Build( MaxPointsPerQuad, PointQuadTreeEntries, Depth, MaxDepth );
}

/*
void
UArianeEditorTool::FPointQuadTree::Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene, uint64 iFlags )
{
    iBLContext->save();
    iBLContext->reset_transform();
    iBLContext->set_stroke_style( BLRgba32( 0xFF0000FF )  );
    iBLContext->set_stroke_width( 1.0f );
    iBLContext->stroke_rect( mRect.x, mRect.y, mRect.w, mRect.h );
    iBLContext->restore();

    for( int i = 0 ; i < 4; i++ )
    {
        if( mChildren[i] )
        {
            mChildren[i]->Draw( iBLContext, iScene, iFlags );
        }
    }
}
*/

void
UArianeEditorTool::FPointQuadTree::Build( uint32 MaxPointsPerQuad
                                        , TArray<FPointQuadTreeEntry>& ParentPointQuadTreeEntries
                                        , uint32 Depth
                                        , uint32 MaxDepth )
{
    PointQuadTreeEntries.Reserve( ParentPointQuadTreeEntries.Num() );

    for( int i = 0; i < ParentPointQuadTreeEntries.Num(); i++ )
    {
        if( Rect.Contains( FIntPoint( ParentPointQuadTreeEntries[i].HUDPosition.X
                                    , ParentPointQuadTreeEntries[i].HUDPosition.Y ) ) )
        {
            PointQuadTreeEntries.Add( ParentPointQuadTreeEntries[i] );
        }
    }

    if( ( PointQuadTreeEntries.Num() > (int32) MaxPointsPerQuad ) && ( Depth < MaxDepth ) )
    {
        double MinX =   Rect.Min.X;
        double MinY =   Rect.Min.Y;
        double MaxX =   Rect.Max.X;
        double MaxY =   Rect.Max.Y;
        double AvgX = ( Rect.Min.X + Rect.Max.X ) * 0.5f;
        double AvgY = ( Rect.Min.Y + Rect.Max.Y ) * 0.5f;

        Children[0] = new FPointQuadTree( FIntRect( MinX, MinY, AvgX, AvgY ), MaxPointsPerQuad, PointQuadTreeEntries, Depth + 1, MaxDepth );
        Children[1] = new FPointQuadTree( FIntRect( AvgX, MinY, MaxX, AvgY ), MaxPointsPerQuad, PointQuadTreeEntries, Depth + 1, MaxDepth );
        Children[2] = new FPointQuadTree( FIntRect( AvgX, AvgY, MaxX, MaxY ), MaxPointsPerQuad, PointQuadTreeEntries, Depth + 1, MaxDepth );
        Children[3] = new FPointQuadTree( FIntRect( MinX, AvgY, AvgX, MaxY ), MaxPointsPerQuad, PointQuadTreeEntries, Depth + 1, MaxDepth );

        PointQuadTreeEntries.Empty();
    }
}

void
UArianeEditorTool::FPointQuadTree::PickPoints( const FVector2D& HUDPosition
                                             , double SelectionRadius
                                             , TArray<FArianePoint*>& OutPickedPoints )
{
   FIntRect LargeRect = FIntRect( Rect.Min.X - SelectionRadius
                                , Rect.Min.Y - SelectionRadius
                                , Rect.Max.X + SelectionRadius
                                , Rect.Max.Y + SelectionRadius );

    if( LargeRect.Contains( FIntPoint( HUDPosition.X, HUDPosition.Y ) ) )
    {
        if( Children[0] == nullptr )
        {
            for( int i = 0; i < PointQuadTreeEntries.Num(); i++ )
            {
                FVector2D Dif = FVector2D( PointQuadTreeEntries[i].HUDPosition.X - HUDPosition.X
                                         , PointQuadTreeEntries[i].HUDPosition.Y - HUDPosition.Y );

                if( Dif.Length() <= SelectionRadius )
                {
                    OutPickedPoints.Add( PointQuadTreeEntries[i].Point );
                }
            }
        }
        else
        {
            for( int i = 0; i < 4; i++ )
            {
                Children[i]->PickPoints( HUDPosition, SelectionRadius, OutPickedPoints );
            }
        }
    }
}

//static
void
UArianeEditorTool::MapPath( FEditorViewportClient* ViewportClient
                          , FSceneView* View
                          , FArianePath* Path
                          , const FIntRect& Rect
                          , TArray<FPointQuadTreeEntry>& OutPointQuadTreeEntries )
{
    const FTransform& PathTransform = Path->GetTransform();

    for( FArianeVertexID& VertexID : Path->GetVertices() )
    {
        FArianeVertex* Vertex = VertexID.GetVertex();

        FVector LocalVertexPosition = Vertex->GetPosition();
        FVector WorldVertexPosition = PathTransform.TransformPosition( Vertex->GetPosition() );
        FVector2D HUDVertexPosition;

        if( WorldToHUD( ViewportClient, View, WorldVertexPosition, HUDVertexPosition ) )
        {
            if( Rect.Contains( FIntPoint( HUDVertexPosition.X, HUDVertexPosition.Y ) ) )
            {
                OutPointQuadTreeEntries.Emplace( Vertex, HUDVertexPosition );
            }
        }
    }
}

//static
void
UArianeEditorTool::MapPoints( FEditorViewportClient* ViewportClient
                            , FSceneView* View
                            , FArianeObject* Object
                            , const FIntRect& Rect
                            , TArray<FPointQuadTreeEntry>& OutPointQuadTreeEntries )
{
    if( Object->HasBaseClass( FArianePath::StaticClass() ) )
    {
        FArianePath* Path = static_cast<FArianePath*>( Object );

        MapPath( ViewportClient, View, Path, Rect, OutPointQuadTreeEntries );
    }
}

void
UArianeEditorTool::MakePointQuadTree( FEditorViewportClient* ViewportClient
                                    , FSceneView* View
                                    , TArray<UArianeLayerDrawing*> DrawingLayers
                                    , bool bFocusedObjectsOnly )
{
    FIntRect ScreenRect = FIntRect( 0, 0, ViewportClient->Viewport->GetSizeXY().X, ViewportClient->Viewport->GetSizeXY().Y );
    TArray<FPointQuadTreeEntry> PointQuadTreeEntries;

    for( UArianeLayerDrawing* DrawingLayer : DrawingLayers )
    {
        FArianeGroup* RootGroup = DrawingLayer->GetRootGroup();

        PointQuadTreeEntries.Reserve( 200 );

        FArianeObject::Traverse( RootGroup
                               , [ ViewportClient
                                 , View
                                 , &ScreenRect
                                 , &PointQuadTreeEntries ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            if( /*( iFocusedObjectsOnly == false ) || scene->GetCell()->ObjectHasFocus( object, traverseFlags )*/1 )
            {
                MapPoints( ViewportClient, View, Object, ScreenRect, PointQuadTreeEntries );

                return FArianeObject::ETraversalReturnValue::Continue;
            }
        } );

        if( PointQuadTree )
        {
            delete PointQuadTree;
        }

        PointQuadTree = new FPointQuadTree( ScreenRect, 100, PointQuadTreeEntries, 0, 8 );
    }
}

bool
UArianeEditorTool::PickPathPoints( FEditorViewportClient* ViewportClient
                                 , FSceneView* View
                                 , FArianePath* Path
                                 , double ViewportX
                                 , double ViewportY
                                 , double PickingRadius
                                 , TArray<FArianeVertex*>& OutPickedVertices
                                 , TArray<FArianeHandleSegment*>& OutPickedHandles
                                 , const FPickingFlags& PickingFlags )
{
    const FTransform& PathTransform = Path->GetTransform();
    bool bAnythingPicked = false;
    FVector2D HUDMousePosition = FVector2D( ViewportX, ViewportY );

    // Pick segment handles
    if( PickingFlags.PathSegmentHandle )
    {
        for( const FArianeSegmentID& SegmentID : Path->GetSegments() )
        {
            FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment();

            if( Segment->GetClass() == FArianeSegmentCubic::StaticClass() )
            {
                FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);

                for( uint32 i = 0; i < 2; i++ )
                {
                    // TODO: hit-test with segment's bounding box.
                    FArianeHandleSegment* Handle = CubicSegment->GetHandle( (uint32) i );
                    const FVector& LocalHandlePosition = Handle->GetPosition();
                    // convert handles coordinates to world coordinates. Easier to detect collision inside
                    // the picking circle.
                    FVector WorldHandlePosition = PathTransform.TransformPosition( LocalHandlePosition );
                    FVector2D HUDHandlePosition;

                    if( WorldToHUD( ViewportClient, View, WorldHandlePosition, HUDHandlePosition ) )
                    {
                        FVector2D Dif0 = HUDHandlePosition - HUDMousePosition;

                        if( Dif0.Length() <= PickingRadius )
                        {
                            OutPickedHandles.Add( Handle );

                            bAnythingPicked = true;

                            break; // forbid multiple selection
                        }
                    }
                }

                if( bAnythingPicked )
                {
                   break; // forbid multiple selection
                }
            }
        }
    }

    // executed after the segment handles because we want the segment handles to have priority.
    for( const FArianeVertexID& VertexID : Path->GetVertices() )
    {
        FArianeVertex* Vertex = const_cast<FArianeVertexID&>(VertexID).GetVertex();

        // Pick vertex
        if ( PickingFlags.PathVertex )
        {
            const FVector& LocalVertexPosition = Vertex->GetPosition();
            const FVector& WorldVertexPosition = PathTransform.TransformPosition( LocalVertexPosition );
            FVector2D HUDVertexPosition;

            if( WorldToHUD( ViewportClient, View, WorldVertexPosition, HUDVertexPosition ) )
            {
                FVector2D Dif = HUDVertexPosition - HUDMousePosition;

                if( Dif.Length() < PickingRadius )
                {
                    OutPickedVertices.Add( Vertex );

                    bAnythingPicked = true;
                }
            }
        }

        // Pick vertex handle
        if( PickingFlags.PathVertexHandle )
        {
            FVector LocalHandlePosition[2];

            GetVertexHandlePositions( Vertex, LocalHandlePosition );

            for( int i = 0; i < 2; i++ )
            {
                FVector WorldHandlePosition = PathTransform.TransformPosition( LocalHandlePosition[i] );
                FVector2D HUDHandlePosition;

                if( WorldToHUD( ViewportClient, View, WorldHandlePosition, HUDHandlePosition ) )
                {
                    FVector2D Dif = HUDHandlePosition - HUDMousePosition;

                    if( Dif.Length() <= PickingRadius )
                    {
                        OutPickedVertices.Add( Vertex );

                        bAnythingPicked = true;

                        break; // forbid multiple selection
                    }
                }
            }
        }
    }

    return bAnythingPicked;
}

// HUD
FLinearColor
UArianeEditorTool::GetForegroundColor()
{
    return FLinearColor( FColor( 0, 169, 157, 255 ) ); // Odyssey's teal
}

FLinearColor
UArianeEditorTool::GetBackgroundColor()
{
    return FLinearColor::Black;
}

FLinearColor
UArianeEditorTool::GetHighlightColor()
{
    return FLinearColor::Red;
}

void
UArianeEditorTool::GetVertexHandlePositions( FArianeVertex* Vertex, FVector OutVertexHandlePositions[2] )
{
    switch ( Vertex->GetSegments().Num() )
    {
        case 1:
        {
            FArianeSegment* FirstSegment = Vertex->GetFirstSegment();
            FVector Cross = Vertex->GetNormal().Cross( FirstSegment->GetVectorLeavingFromVertex( Vertex, false ) ).GetSafeNormal();

            OutVertexHandlePositions[0] = ( Vertex->GetPosition() + ( Cross * Vertex->GetRadius() ) );
            OutVertexHandlePositions[1] = ( Vertex->GetPosition() - ( Cross * Vertex->GetRadius() ) );
        }
        break;

        case 2 :
        {
            FArianeSegment* FirstSegment = Vertex->GetFirstSegment();
            FArianeSegment* OtherSegment = Vertex->GetOtherSegment( FirstSegment );
            FVector Average = ( - FirstSegment->GetVectorLeavingFromVertex( Vertex, true )
                                + OtherSegment->GetVectorLeavingFromVertex( Vertex, true ) ).GetSafeNormal();
            FVector Cross = Vertex->GetNormal().Cross( Average ).GetSafeNormal();

            OutVertexHandlePositions[0] = ( Vertex->GetPosition() + ( Cross * Vertex->GetRadius() ) );
            OutVertexHandlePositions[1] = ( Vertex->GetPosition() - ( Cross * Vertex->GetRadius() ) );
        }
        break;

        default :
        break;
    }
}

void
UArianeEditorTool::DrawCircleHUD( FCanvas* Canvas
                                , FEditorViewportClient* ViewportClient
                                , FSceneView* View
                                , const FVector2D& HUDCoords
                                , double Radius
                                , const FLinearColor& Color
                                , float Thickness )
{
    uint32 Steps = 64;
    double Point0Angle = 0.0f;
    double AngleStep = ( 2.0f * PI ) / Steps;
    FVector Point0 =  FVector ( HUDCoords.X + FMath::Cos( Point0Angle ) * Radius,
                                HUDCoords.Y + FMath::Sin( Point0Angle ) * Radius,
                                0.0f );

    for ( uint32 i = 0; i < Steps; i++ )
    {
        double Point1Angle = Point0Angle + AngleStep;
        FVector Point1 =  FVector ( HUDCoords.X + FMath::Cos( Point1Angle ) * Radius,
                                    HUDCoords.Y + FMath::Sin( Point1Angle ) * Radius,
                                    0.0f );

        DrawLineHUD( Canvas, ViewportClient, View, FVector2D( Point0 ), FVector2D( Point1 ), Color, Thickness );

        Point0 = Point1;

        Point0Angle = Point1Angle;
    }
}

void
UArianeEditorTool::DrawLineHUD( FCanvas* Canvas
                              , FEditorViewportClient* ViewportClient
                              , FSceneView* View
                              , const FVector2D& HUDCoordsP0
                              , const FVector2D& HUDCoordsP1
                              , const FLinearColor& Color
                              , float Thickness )
{
    // use FCanvasLineItem if opaque, as it is faster
    if( Color.A == 1.0f )
    {
        FCanvasLineItem line = FCanvasLineItem( HUDCoordsP0, HUDCoordsP1 );

        line.LineThickness = Thickness;
        line.SetColor( Color );

        Canvas->DrawItem( line );
    }
    else
    {
        FBatchedElements* BatchedElements = Canvas->GetBatchedElements(FCanvas::ET_Line);

        BatchedElements->AddTranslucentLine( FVector( HUDCoordsP0, 0.f)
                                           , FVector( HUDCoordsP1, 0.f)
                                           , Color
                                           , FHitProxyId::InvisibleHitProxyId
                                           , Thickness
                                           , 0.f
                                           , true );
    }
}

void
UArianeEditorTool::DraweOutlinedLineHUD( FCanvas* Canvas
                                       , FEditorViewportClient* ViewportClient
                                       , FSceneView* View
                                       , const FVector2D& HUDCoordsP0
                                       , const FVector2D& HUDCoordsP1
                                       , const FLinearColor& Color
                                       , float Thickness )
{
    DrawLineHUD( Canvas, ViewportClient, View, HUDCoordsP0, HUDCoordsP1, Color, Thickness );

/*
    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Triangle);
    FVector2D vec = ( iHUDCoordsP1 - iHUDCoordsP0 );
    FVector2D perpendicular = FVector2D( -vec.Y, vec.X );
    double radius = ( iThickness + 1.0f ) * 0.5f;

    if( perpendicular.SizeSquared() )
    {
        perpendicular.Normalize();

        FVector4 vertex[4] = { FVector4( iHUDCoordsP0.X + ( perpendicular.X * radius )
                                       , iHUDCoordsP0.Y + ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( iHUDCoordsP1.X + ( perpendicular.X * radius )
                                       , iHUDCoordsP1.Y + ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( iHUDCoordsP1.X - ( perpendicular.X * radius )
                                       , iHUDCoordsP1.Y - ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f )
                             , FVector4( iHUDCoordsP0.X - ( perpendicular.X * radius )
                                       , iHUDCoordsP0.Y - ( perpendicular.Y * radius )
                                       , 0.0f
                                       , 1.0f ) };
        int32 idx0, idx1, idx2, idx3;

        idx0 = batchedElements->AddVertex( vertex[0], FVector2D(0, 0), iColor, iParams.mCanvas->GetHitProxyId() );
        idx1 = batchedElements->AddVertex( vertex[1], FVector2D(1, 0), iColor, iParams.mCanvas->GetHitProxyId() );
        idx2 = batchedElements->AddVertex( vertex[2], FVector2D(1, 1), iColor, iParams.mCanvas->GetHitProxyId() );
        idx3 = batchedElements->AddVertex( vertex[3], FVector2D(0, 1), iColor, iParams.mCanvas->GetHitProxyId() );

        batchedElements->AddTriangle( idx0, idx1, idx2, mLineOutlinedTexture->GetResource(), BLEND_Translucent );
        batchedElements->AddTriangle( idx2, idx3, idx0, mLineOutlinedTexture->GetResource(), BLEND_Translucent );
    }
*/
}

void
UArianeEditorTool::DrawVertexHUD( FCanvas* Canvas
                                , FEditorViewportClient* ViewportClient
                                , FSceneView* View
                                , FArianeVertex* Vertex
                                , const FLinearColor& FgColor
                                , const FLinearColor& BgColor
                                , const FLinearColor& HcColor
                                , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags )
{
    FIntRect Screen = FIntRect( FIntPoint(0,0), ViewportClient->Viewport->GetSizeXY() );
    const FTransform& PathTransform = static_cast<FArianePath*>(Vertex->GetOwner())->GetTransform();
    // TODO: compute that once and pass it as parameter for all vertices
    FVector WorldVertexPoint = PathTransform.TransformPosition( Vertex->GetPosition() );
    FVector2D HUDVertexPoint;

    if( WorldToHUD( ViewportClient, View, WorldVertexPoint, HUDVertexPoint ) )
    {
        static FLinearColor GreenColor = FLinearColor::Green;
        static FLinearColor LightGrayColor = FLinearColor(  0.5f,  0.5f,  0.5f, 1.0f );
        static FLinearColor DarkGrayColor = FLinearColor( 0.25f, 0.25f, 0.25f, 1.0f );
        static FLinearColor RedColor = FLinearColor::Red;
        double HUDVertexRadius = FArianeEditorHUD::VERTEXRADIUS;
        double HUDHandleRadius = FArianeEditorHUD::HANDLERADIUS;
        FIntRect HUDVertexBBox = FIntRect( HUDVertexPoint.X - HUDVertexRadius
                                         , HUDVertexPoint.Y - HUDVertexRadius
                                         , HUDVertexPoint.X + HUDVertexRadius
                                         , HUDVertexPoint.Y + HUDVertexRadius );

        if ( HUDDrawingFlags.PathVertexHandle )
        {
            static FLinearColor WhiteColor = FLinearColor::White;
            static FLinearColor BlackColor = FLinearColor::Black;
            FVector LocalHandlePosition[2];

            GetVertexHandlePositions( Vertex, LocalHandlePosition );

            for( uint32 i = 0; i < 2; i++ )
            {
                FVector WorldHandlePosition = PathTransform.TransformPosition( LocalHandlePosition[i] );
                FVector2D HUDHandlePosition;

                if ( WorldToHUD( ViewportClient, View, WorldHandlePosition, HUDHandlePosition ) )
                {
                    FCanvasLineItem HudHandleLine = FCanvasLineItem( HUDVertexPoint, HUDHandlePosition );
                    FIntRect HUDHandle0BBox = FIntRect( FMath::Min( HUDVertexPoint.X, HUDHandlePosition.X ) - HUDHandleRadius
                                                      , FMath::Min( HUDVertexPoint.Y, HUDHandlePosition.Y ) - HUDHandleRadius
                                                      , FMath::Max( HUDVertexPoint.X, HUDHandlePosition.X ) + HUDHandleRadius
                                                      , FMath::Max( HUDVertexPoint.Y, HUDHandlePosition.Y ) + HUDHandleRadius );

                    if( Screen.Contains( HUDHandle0BBox ) )
                    {
                        // Line to handle
                        DraweOutlinedLineHUD( Canvas
                                            , ViewportClient
                                            , View
                                            , HUDVertexPoint
                                            , HUDHandlePosition
                                            , WhiteColor
                                            , 1.0f );

                        // handle
                        Canvas->DrawTile( HUDHandlePosition.X - HUDHandleRadius
                                        , HUDHandlePosition.Y - HUDHandleRadius
                                        , HUDHandleRadius * 2.0f
                                        , HUDHandleRadius * 2.0f
                                        , 0.0f
                                        , 0.0f
                                        , 1.0f
                                        , 1.0f
                                        , WhiteColor
                                        , HandleTexture->GetResource()
                                        , ESimpleElementBlendMode::SE_BLEND_Masked );
                    }
                }
            }
        }

        if( Screen.Contains( HUDVertexBBox ) )
        {
            if( Vertex->IsLocked() == false )
            {
                Canvas->DrawTile( HUDVertexPoint.X - HUDVertexRadius
                                , HUDVertexPoint.Y - HUDVertexRadius
                                , HUDVertexRadius * 2.0f
                                , HUDVertexRadius * 2.0f
                                , 0.0f
                                , 0.0f
                                , 1.0f
                                , 1.0f
                                , Vertex->IsSelected() && ( HUDDrawingFlags.Mode == FArianeEditorHUD::EMode::Vertex ) ? HcColor
                                                                                                                      : FgColor
                                , VertexTexture->GetResource()
                                , ESimpleElementBlendMode::SE_BLEND_Masked );
            }
            else
            {
                Canvas->DrawTile( HUDVertexPoint.X - HUDVertexRadius
                                , HUDVertexPoint.Y - HUDVertexRadius
                                , HUDVertexRadius * 2.0f
                                , HUDVertexRadius * 2.0f
                                , 0.0f
                                , 0.0f
                                , 1.0f
                                , 1.0f
                                , Vertex->IsSelected() && ( HUDDrawingFlags.Mode == FArianeEditorHUD::EMode::Vertex ) ? ( HcColor * 0.5f )
                                                                                                                      : LightGrayColor
                                , VertexTexture->GetResource()
                                , ESimpleElementBlendMode::SE_BLEND_Masked );
            }
        }
    }
}

void
UArianeEditorTool::DrawSegmentHUD( FCanvas* Canvas
                                 , FEditorViewportClient* ViewportClient
                                 , FSceneView* View
                                 , FArianeSegment* Segment
                                 , const FLinearColor& FgColor
                                 , const FLinearColor& BgColor
                                 , const FLinearColor& HcColor
                                 , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags )
{
    const FTransform& PathTransform = static_cast<FArianePath*>(Segment->GetOwner())->GetTransform();

    // Draw structure
    {
        FCanvasLineItem LineItem;

        LineItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;

        LineItem.SetColor( FgColor );

        for( const FArianeSegment::FFraction& Fraction : Segment->GetFractions() )
        {
            FVector P0 = PathTransform.TransformPosition( Fraction.Steps[0]->Point->GetPosition() );
            FVector P1 = PathTransform.TransformPosition( Fraction.Steps[1]->Point->GetPosition() );
            FVector2D HUDP0;
            FVector2D HUDP1;

            if( WorldToHUD( ViewportClient, View, P0, HUDP0 ) && WorldToHUD( ViewportClient, View, P1, HUDP1 ) )
            {
                LineItem.Origin = FVector( HUDP0.X, HUDP0.Y, 0.0f );
                LineItem.EndPos = FVector( HUDP1.X, HUDP1.Y, 0.0f );

                Canvas->DrawItem( LineItem );
            }
        }
    }
}

void
UArianeEditorTool::DrawCubicSegmentHUD( FCanvas* Canvas
                                      , FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , FArianeSegmentCubic* CubicSegment
                                      , const FLinearColor& FgColor
                                      , const FLinearColor& BgColor
                                      , const FLinearColor& HcColor
                                      , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags )
{
    const FTransform& PathTransform = static_cast<FArianePath*>(CubicSegment->GetOwner())->GetTransform();
    FArianeVertex* Vertex0 = CubicSegment->GetVertex(0);
    FArianeVertex* Vertex1 = CubicSegment->GetVertex(1);
    FVector V0 = PathTransform.TransformPosition( Vertex0->GetPosition() );
    FVector V1 = PathTransform.TransformPosition( Vertex1->GetPosition() );
    double HUDHandleRadius = FArianeEditorHUD::HANDLERADIUS;
    FCanvasLineItem LineItem;

    DrawSegmentHUD( Canvas, ViewportClient, View, CubicSegment, FgColor, BgColor, HcColor, HUDDrawingFlags );

    LineItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;

    if( HUDDrawingFlags.PathSegmentHandle)
    {
        for( uint32 i = 0; i < 2; i++ )
        {
            FArianeVertex* Vertex = CubicSegment->GetVertex((uint32)i);
            FVector WorldVertexPosition = PathTransform.TransformPosition( CubicSegment->GetVertex((uint32)i)->GetPosition() );
            FVector WorldHandlePosition = PathTransform.TransformPosition( CubicSegment->GetHandle((uint32)i)->GetPosition() );
            FVector2D HUDVertexPosition;
            FVector2D HUDHandlePosition;

            WorldToHUD( ViewportClient, View, WorldVertexPosition, HUDVertexPosition );
            WorldToHUD( ViewportClient, View, WorldHandlePosition, HUDHandlePosition );

            LineItem.SetColor( Vertex->IsHandleAligned() ? FLinearColor::Green : FLinearColor::White );

            LineItem.Origin = FVector( HUDVertexPosition.X, HUDVertexPosition.Y, 0.0f );
            LineItem.EndPos = FVector( HUDHandlePosition.X, HUDHandlePosition.Y, 0.0f );

            Canvas->DrawItem( LineItem );
            Canvas->DrawTile( HUDHandlePosition.X - HUDHandleRadius
                            , HUDHandlePosition.Y - HUDHandleRadius
                            , HUDHandleRadius * 2.0f
                            , HUDHandleRadius * 2.0f
                            , 0.0f
                            , 0.0f
                            , 1.0f
                            , 1.0f
                            , FLinearColor::White
                            , HandleTexture->GetResource()
                            , ESimpleElementBlendMode::SE_BLEND_Masked );
        }
    }
}

void
UArianeEditorTool::DrawPathHUD( FCanvas* Canvas
                              , FEditorViewportClient* ViewportClient
                              , FSceneView* View
                              , FArianePath* Path
                              , const FLinearColor& FgColor
                              , const FLinearColor& BgColor
                              , const FLinearColor& HcColor
                              , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags )
{
    if( HUDDrawingFlags.PathSegment )
    {
        for( const FArianeSegmentID& SegmentID : Path->GetSegments() )
        {
            FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment();

            if( Segment->GetClass() == FArianeSegmentCubic::StaticClass() )
            {
                FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);

                DrawCubicSegmentHUD( Canvas, ViewportClient, View, CubicSegment, FgColor, BgColor, HcColor, HUDDrawingFlags );
            }

            if( Segment->GetClass() == FArianeSegment::StaticClass() )
            {
                DrawSegmentHUD( Canvas, ViewportClient, View, Segment, FgColor, BgColor, HcColor, HUDDrawingFlags );
            }
        }
    }

    // Points and Point size handles
    for( const FArianeVertexID& VertexID : Path->GetVertices() )
    {
        FArianeVertex* Vertex = const_cast<FArianeVertexID&>(VertexID).GetVertex();
        uint32 Valence = Vertex->GetSegments().Num();

        if( HUDDrawingFlags.PathVertex || ( ( Valence == 0 ) && HUDDrawingFlags.PathVertexValence0 ) )
        {
            DrawVertexHUD( Canvas, ViewportClient, View, Vertex, FgColor, BgColor, HcColor, HUDDrawingFlags );
        }
        else
        if( HUDDrawingFlags.PathVertex || ( ( Valence == 1 ) && HUDDrawingFlags.PathVertexValence1 ) )
        {
            DrawVertexHUD( Canvas, ViewportClient, View, Vertex, FgColor, BgColor, HcColor, HUDDrawingFlags );
        }
        else
        if( HUDDrawingFlags.PathVertex || ( ( Valence == 2 ) && HUDDrawingFlags.PathVertexValence2 ) )
        {
            DrawVertexHUD( Canvas, ViewportClient, View, Vertex, FgColor, BgColor, HcColor, HUDDrawingFlags );
        }
    }
}

void
UArianeEditorTool::ExtendToolbar( UToolMenu* iToolMenu )
{
}
