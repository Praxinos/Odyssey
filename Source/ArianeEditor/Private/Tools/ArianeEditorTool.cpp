// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditor.h"
// Unreal
#include "Framework/Application/SlateApplication.h"
#include "Misc/TransactionObjectEvent.h"
#include "InputBehavior.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "InteractiveToolManager.h"

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
    // unregister IInputProcessor interface
/* Gary
    FSlateApplication::Get().UnregisterInputPreProcessor(mInputProcessor);


    //mToolContext->OnChanged().RemoveAll(this);
    Flush(); //Finish everything

    mCommandList = nullptr;
*/
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
UArianeEditorTool::DrawHUD ( FEditorViewportClient* ViewportClient
                           , FViewport* Viewport
                           , const FSceneView* View
                           , FCanvas* Canvas )
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

    OnMouseDown( ViewportClient
               , PressedKey
               , FArianePointerState( PressPos.ScreenPosition.X
                                    , PressPos.ScreenPosition.Y ) );
}

// Implements IClickDragBehaviorTarget::OnClickDrag
void
UArianeEditorTool::OnClickDrag( const FInputDeviceRay& DragPos )
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    OnMouseDrag( ViewportClient
               , PressedKey
               , FArianePointerState( DragPos.ScreenPosition.X
                                    , DragPos.ScreenPosition.Y ) );
}

// Implements IClickDragBehaviorTarget::OnClickRelease
void
UArianeEditorTool::OnClickRelease( const FInputDeviceRay& ReleasePos )
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    OnMouseUp( ViewportClient
             , PressedKey
             , FArianePointerState( ReleasePos.ScreenPosition.X
                                  , ReleasePos.ScreenPosition.Y ) );
}

void
UArianeEditorTool::OnTerminateDragSequence()
{
}
