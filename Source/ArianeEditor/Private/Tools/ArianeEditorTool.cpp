// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorSettings.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
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
}

UArianeEditorTool::UArianeEditorTool()
    : Editor (nullptr)
    , bHasContextMenu ( false )
    , bInited ( false )
    , Icon ( nullptr )
{
/* Gary
    mInputProcessor = MakeShared<FArianeEditorToolInputProcessor>(this);
*/
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
/* Gary
    FSlateApplication::Get().RegisterInputPreProcessor(mInputProcessor);


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
/* Gary
    FSlateApplication::Get().UnregisterInputPreProcessor(mInputProcessor);


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

bool UArianeEditorTool::OnMouseDown( FEditorViewportClient* iViewportClient
                                   , const FKey& Key
                                   , const FArianePointerState& State
                                   , bool iRepeat )
{
    return false;
}

void
UArianeEditorTool::OnMouseHover( FEditorViewportClient* iViewportClient
                               , const FArianePointerState& State )
{
}

bool
UArianeEditorTool::OnMouseDrag( FEditorViewportClient* iViewportClient
                              , const FKey& iKey
                              , const FArianePointerState& State )
{
    return false;
}

bool
UArianeEditorTool::OnMouseUp( FEditorViewportClient* iViewportClient
                            , const FKey& Key
                            , const FArianePointerState& State )
{
    return false;
}

bool
UArianeEditorTool::OnMouseClick( FEditorViewportClient* iViewportClient
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
    while (mPacketQueue.Dequeue());

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

    switch( DrawingLayer->GetDrawingOrientation() )
    {
        case EArianeLayerDrawingOrientation::LayerXY:
            WorldMatrix = LayerMatrix;
        break;

        case EArianeLayerDrawingOrientation::LayerYZ:
            // Note: args are Pitch(Y) Yaw(Z) Roll(X)
            // but rotation order is Yaw (Z) Pitch (Y) Roll (X)
            FMatrix YZRotation = FRotationMatrix( FRotator(  0.f, 90.f, 90.f ) );

            WorldMatrix = YZRotation * LayerMatrix;
        break;

        case EArianeLayerDrawingOrientation::LayerZX:
            // Note: args are Pitch(Y) Yaw(Z) Roll(X)
            // but rotation order is Yaw (Z) Pitch (Y) Roll (X)
            FMatrix ZXRotation = FRotationMatrix( FRotator(  0.f,  0.f, 90.f ) );

            WorldMatrix = ZXRotation * LayerMatrix;
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
UArianeEditorTool::DrawHUDCircle(  FCanvas* Canvas, double X, double Y, double Radius, uint32 Steps )
{
    double Point0Angle = 0.0f;
    double AngleStep = ( 2.0f * PI ) / Steps;
    FVector Point0 =  FVector ( X + FMath::Cos( Point0Angle ) * Radius,
                                Y + FMath::Sin( Point0Angle ) * Radius,
                                0.0f );
    FCanvasLineItem Line;

    Line.LineThickness = 1.0f;
    Line.SetColor( FColor::Red );

    for ( uint32 i = 0; i < Steps; i++ )
    {
        double Point1Angle = Point0Angle + AngleStep;
        FVector Point1 =  FVector ( X + FMath::Cos( Point1Angle ) * Radius,
                                    Y + FMath::Sin( Point1Angle ) * Radius,
                                    0.0f );

        Line.Origin = Point0;
        Line.EndPos = Point1;

        Canvas->DrawItem( Line );

        Point0 = Point1;

        Point0Angle = Point1Angle;
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
        ConvertWintabToWindowCoordinates(packetCopyWin.X, packetCopyWin.Y);
        mPacketQueue.Enqueue(packetCopyWin);
        return;
    }
#endif
    // FIX: MOVE WINTAB COORDINATES WHEN MAIN SCREEN IS NOT ON THE (TOP) LEFT OF USER PHYSICAL DESKTOP - AWAITING FOR EPIC PULL REQUEST VALIDATION

    // FIX: HAVE TO MANUALLY HANDLE UP AND DOWN UNTIL EPIC ACCEPT INTERNAL PULL REQUEST
#if PLATFORM_MAC
    UE::StylusInput::FStylusInputPacket packetCopy = iPacket;

    static UE::StylusInput::EPenStatus currentPenStatus = UE::StylusInput::EPenStatus::None;
    static UE::StylusInput::EPacketType currentPacketType = UE::StylusInput::EPacketType::Invalid;

    if (iPacket.NormalPressure == 0)
    {
        currentPenStatus = currentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
        if (currentPacketType == UE::StylusInput::EPacketType::OnDigitizer)
            currentPacketType = UE::StylusInput::EPacketType::StylusUp;
        else
            currentPacketType = UE::StylusInput::EPacketType::AboveDigitizer;
    }

    if (iPacket.NormalPressure != 0)
    {
        currentPenStatus = currentPenStatus | UE::StylusInput::EPenStatus::CursorIsTouching;
        if (currentPacketType != UE::StylusInput::EPacketType::OnDigitizer && currentPacketType != UE::StylusInput::EPacketType::StylusDown)
            currentPacketType = UE::StylusInput::EPacketType::StylusDown;
        else
            currentPacketType = UE::StylusInput::EPacketType::OnDigitizer;
    }
    else
    {
        currentPenStatus = currentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
    }

    packetCopy.PenStatus = currentPenStatus;
    packetCopy.Type = currentPacketType;

    mPacketQueue.Enqueue(packetCopy);
    return;
#endif
    // FIX: HAVE TO MANUALLY HANDLE UP AND DOWN UNTIL EPIC ACCEPT INTERNAL PULL REQUEST

    mPacketQueue.Enqueue(iPacket);
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

    while (mPacketQueue.Dequeue()) {}

    bIsRecordingStylus = false;
}

bool
UArianeEditorTool::ReadStylusInput( eStylusEventFence iUntilEventType )
{
    if (!bIsFocused || mPacketQueue.Num() == 0)
    {
        while (mPacketQueue.Dequeue()) {}
        return false;
    }

    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    UE::StylusInput::FStylusInputPacket packet;

    while (mPacketQueue.Dequeue(packet))
    {
        FArianePointerState arianePointerState = StylusPacketToArianePointerState(packet);

        if (packet.Type == UE::StylusInput::EPacketType::StylusDown && CanDraw())
        {
            //MouseDown
            bIsStylusDown = true;
            OnMouseDown(ViewportClient, PressedKey, arianePointerState);

            if (iUntilEventType == eStylusEventFence::kStylusDown)
                return true;
        }
        else if (packet.Type == UE::StylusInput::EPacketType::StylusUp && CanDraw())
        {
            //MouseUp
            OnMouseUp(ViewportClient, PressedKey, arianePointerState);
            bIsStylusDown = false;

            if (iUntilEventType == eStylusEventFence::kStylusUp && CanDraw())
            {
                StopStylusInputRecord();
                return true;
            }
        }
        else if (bIsStylusDown && CanDraw())
        {
            OnMouseDrag(ViewportClient, PressedKey, arianePointerState);
        }
    }
    return true;
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

    OnMouseHover( ViewportClient
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
        FArianePointerState Pointerstate = FArianePointerState(PressPos.ScreenPosition.X
            , PressPos.ScreenPosition.Y);

        if (CanDraw())
            OnMouseDown(ViewportClient
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
        FArianePointerState Pointerstate = FArianePointerState(DragPos.ScreenPosition.X
            , DragPos.ScreenPosition.Y);

        if (CanDraw())
            OnMouseDrag(ViewportClient
                , PressedKey
                , Pointerstate);
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
        FArianePointerState Pointerstate = FArianePointerState(ReleasePos.ScreenPosition.X
            , ReleasePos.ScreenPosition.Y);

        if (CanDraw())
            OnMouseUp(ViewportClient
                , PressedKey
                , Pointerstate);
    }
}

FVector2D
UArianeEditorTool::ScreenToHUD( const FVector2D& ScreenPosition )
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    float DPIScale = ViewportClient->GetDPIScale();
    double X = ScreenPosition.X / DPIScale;
    double Y = ScreenPosition.Y / DPIScale;

    return FVector2D( X, Y );
}

void
UArianeEditorTool::OnTerminateDragSequence()
{
}
