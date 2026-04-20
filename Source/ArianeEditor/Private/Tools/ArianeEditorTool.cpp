// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
// Odyssey
#include "IOdysseyStylusInputModule.h"
// Unreal
#include "Framework/Application/SlateApplication.h"
#include "Misc/TransactionObjectEvent.h"
#include "InputBehavior.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "InteractiveToolManager.h"
#include "StylusInput.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Components/LineBatchComponent.h"

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
        UClickDragInputBehavior* ClickDragInputBehavior = NewObject<UClickDragInputBehavior>(this);
        UMouseHoverBehavior* MouseHoverBehavior = NewObject<UMouseHoverBehavior>(this);

        ClickDragInputBehavior->Initialize(this);
        MouseHoverBehavior->Initialize(this);

        AddInputBehavior( ClickDragInputBehavior );
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
    ListenStylusInput();
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
    IgnoreStylusInput();
    // unregister IInputProcessor interface
/* Gary
    FSlateApplication::Get().UnregisterInputPreProcessor(mInputProcessor);


    //mToolContext->OnChanged().RemoveAll(this);
    Flush(); //Finish everything

    mCommandList = nullptr;
*/
}

void
UArianeEditorTool::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget
                                       , const TArray<FStylusState>& NewStates
                                       , int32 StylusIndex )
{
    for ( const FStylusState& State : NewStates )
    {
        Pressure = State.GetPressure();
    }
}

// Temp
void
UArianeEditorTool::FlushStylusInput()
{
    UOdysseyStylusInputSubsystem* InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    if( InputSubsystem )
        InputSubsystem->Flush();
}

// Temp
void
UArianeEditorTool::ListenStylusInput()
{
    UOdysseyStylusInputSubsystem* InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    if( InputSubsystem )
        InputSubsystem->AddMessageHandler( *this );
}

// Temp
void
UArianeEditorTool::IgnoreStylusInput()
{
    UOdysseyStylusInputSubsystem* InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    if( InputSubsystem )
        InputSubsystem->RemoveMessageHandler(*this);
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

FSceneView*
UArianeEditorTool::GetSceneView( FEditorViewportClient* iViewportClient )
{
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( iViewportClient->Viewport
                                                                            , iViewportClient->GetScene()
                                                                            , iViewportClient->EngineShowFlags )
                                                                            .SetRealtimeUpdate( iViewportClient->IsRealtime() ) );
    return iViewportClient->CalcSceneView( &ViewFamily );
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

/* Gary

bool
UArianeEditorTool::OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    return false;
}

bool
UArianeEditorTool::OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
UArianeEditorTool::OnKeyDown(const FKey& iKey)
{
    return false;
}

bool
UArianeEditorTool::OnKeyUp(const FKey& iKey)
{
    return false;
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

void UArianeEditorTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
}

void
UArianeEditorTool::ExtendMenu( TSharedRef<FExtender> iExtender )
{

}

void
UArianeEditorTool::ExtendToolbar( UToolMenu* iToolMenu )
{
}
*/

EMouseCursor::Type UArianeEditorTool::GetMouseCursor() const
{
    return EMouseCursor::Crosshairs;
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
    IToolsContextQueriesAPI* QueriesAPI = GetToolManager()->GetContextQueriesAPI();
    FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
    ULineBatchComponent* LineBatcher = GetWorld()->GetLineBatcher( UWorld::ELineBatcherType::World );
    float OriX, OriY;
    float EndX, EndY;
    FLinearColor Color = FLinearColor::Gray.CopyWithNewOpacity( 0.125f );
    FTransform LayerTransform = DrawingLayer->GetComponentTransform();
    FMatrix LayerMatrix = LayerTransform.ToMatrixWithScale();;
    FMatrix WorldMatrix;
    float AdjustedThickness = 2.0f;
    FViewCameraState CameraState;

    QueriesAPI->GetCurrentViewState( CameraState );

    // Adjust line thickness relative to camera distance
    float Distance = FVector::Dist( CameraState.Position, DrawingLayer->GetComponentLocation() );
    AdjustedThickness = ( Distance / 1000.0f ) * AdjustedThickness;
    double GridRadius = 1000;
    static uint32 StepCount = 20;
    double Step = ( GridRadius * 2 ) / StepCount;

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
/*
            FVector ZVector = FVector( 0.0f, 0.0f, 1.0f );
            IToolsContextQueriesAPI* QueriesAPI = GetToolManager()->GetContextQueriesAPI();
            FViewCameraState CameraState;

            QueriesAPI->GetCurrentViewState( CameraState );

            FVector CameraDirection = CameraState.Orientation.GetForwardVector();
            FQuat RotationQuat = FQuat::FindBetweenVectors( ZVector, -CameraDirection );

            WorldMatrix = RotationQuat.ToMatrix() * LayerMatrix ;

            FVector PlaneWorldYVector = WorldMatrix.TransformVector( FVector( 0.0f, 1.0f, 0.0f ) );
            FVector CameraRight = CameraState.Orientation.GetRightVector();
            FQuat ZRotationQuat = FQuat::FindBetweenVectors( PlaneWorldYVector, CameraRight );

            WorldMatrix = ZRotationQuat.ToMatrix() * WorldMatrix ;
*/
            // --- Begin this was AI generated because the code above did not work and I did not understand why.
            FVector CamForward = CameraState.Orientation.GetForwardVector();
            FVector CamUp = CameraState.Orientation.GetUpVector();
            FVector CamRight = CameraState.Orientation.GetRightVector();
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

        PDI->DrawLine( WorldMatrix.TransformPosition( Origin )
                     , WorldMatrix.TransformPosition( EndPos )
                     , Color
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

        PDI->DrawLine( WorldMatrix.TransformPosition( Origin )
                     , WorldMatrix.TransformPosition( EndPos )
                     , Color
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
    FSceneView* View = GetSceneView( GetActiveViewportClient() );
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
//UArianeEditorTool::OnTick(float DeltaTime)
UArianeEditorTool::Render(IToolsContextRenderAPI* RenderAPI)
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    //Super::OnTick( DeltaTime );

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( LayerStack->GetCurrentLayer() );

        if( DrawingLayer )
        {
            DrawLayerOrientationGrid( RenderAPI, DrawingLayer );
        }
    }
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


FEditorViewportClient*
UArianeEditorTool::GetActiveViewportClient()
{
    FViewport* ActiveViewport = GEditor->GetActiveViewport();

    return static_cast<FEditorViewportClient*>(ActiveViewport->GetClient());
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

// Implements IClickDragBehaviorTarget::OnClickPress
void
UArianeEditorTool::OnClickPress( const FInputDeviceRay& PressPos )
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FArianePointerState Pointerstate = FArianePointerState( PressPos.ScreenPosition.X
                                                          , PressPos.ScreenPosition.Y );

    Pressure = 1.0f;

    FlushStylusInput(); // will fill Pressure if any

    Pointerstate.Pressure  = Pressure;

    OnMouseDown( ViewportClient
               , PressedKey
               , Pointerstate );
}

// Implements IClickDragBehaviorTarget::OnClickDrag
void
UArianeEditorTool::OnClickDrag( const FInputDeviceRay& DragPos )
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FArianePointerState Pointerstate = FArianePointerState( DragPos.ScreenPosition.X
                                                          , DragPos.ScreenPosition.Y );

    Pressure = 1.0f;

    FlushStylusInput(); // will fill Pressure if any

    Pointerstate.Pressure  = Pressure;

    OnMouseDrag( ViewportClient
               , PressedKey
               , Pointerstate );
}

// Implements IClickDragBehaviorTarget::OnClickRelease
void
UArianeEditorTool::OnClickRelease( const FInputDeviceRay& ReleasePos )
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FArianePointerState Pointerstate = FArianePointerState( ReleasePos.ScreenPosition.X
                                                          , ReleasePos.ScreenPosition.Y );

    Pressure = 1.0f;

    FlushStylusInput(); // will fill Pressure if any

    Pointerstate.Pressure  = Pressure;

    OnMouseUp( ViewportClient
             , PressedKey
             , Pointerstate );
}

void
UArianeEditorTool::OnTerminateDragSequence()
{
}
