// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "InteractiveTool.h"
#include "BaseBehaviors/BehaviorTargetInterfaces.h"
// Ariane
#include "ArianePointerState.h"
// OdysseyHeaders
#include "OdysseyPainterEditorColorType.h"
#include "IStylusState.h"

#include "ArianeEditorTool.generated.h"

class FArianeEditor;
class FSceneView;
class FStylusState;
struct FSlateBrush;
class UArianeLayerDrawing;
/* Gary
class FArianeEditorToolInputProcessor;
*/

UCLASS(Abstract)
class ARIANEEDITOR_API UArianeEditorTool : public UInteractiveTool
                                         , public IClickDragBehaviorTarget
                                         , public IHoverBehaviorTarget
                                         , public IStylusMessageHandler
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UArianeEditorTool();

    //Constructor
    UArianeEditorTool();

public:
    virtual FString GetType() { return FString(); };

public:
    // UInteractiveTool::Setup
    virtual void Setup() override;
    // UInteractiveTool::Shutdown
    virtual void Shutdown( EToolShutdownType ShutdownType ) override;

    // Implements IClickDragBehaviorTarget::CanBeginClickSequence
    virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
    // Implements IClickDragBehaviorTarget::OnClickPress
    virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
    // Implements IClickDragBehaviorTarget::OnClickDrag
    virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
    // Implements IClickDragBehaviorTarget::OnClickRelease
    virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
    // Implements IClickDragBehaviorTarget::OnTerminateDragSequence
    virtual void OnTerminateDragSequence()override;

    // Implements IHoverBehaviorTarget::FInputRayHit
    virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
    // Implements IHoverBehaviorTarget::OnBeginHover
    virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override;
    // Implements IHoverBehaviorTarget::OnUpdateHover
    virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;
    // Implements IHoverBehaviorTarget::OnEndHover
    virtual void OnEndHover() override;

    //virtual void OnTick(float DeltaTime) override;
    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

    //Mouse events
    virtual bool OnMouseDown( FEditorViewportClient* iViewportClient
                            , const FKey& iKey
                            , const FArianePointerState& State
                            , bool iRepeat = false );
    virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                                , const FArianePointerState& State );
    virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                            , const FKey& iKey
                            , const FArianePointerState& State );
    virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                            , const FKey& iKey
                            , const FArianePointerState& State );
    virtual bool OnMouseClick( FEditorViewportClient* iViewportClient
                             , const FKey& iKey
                             , const FArianePointerState& State );

    virtual void DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI ) override;

    /** Get the tool's tooltip */
    virtual FText GetTooltip() const;
    /** Is the tool activable ? */
    virtual bool IsActivable() const;
    /** Is the tool activated ? */
    virtual bool IsActivated() const;

    /** Callback for when the tool is activated */
    virtual void Activate();
    /** Callback for when the tool is inactivated */
    virtual void Inactivate();

    virtual bool SupportsColorType( EOdysseyPainterEditorColorType ColorType );

    virtual void Init( FArianeEditor* InEditor );

    virtual void OnStylusStateChanged( const TWeakPtr<SWidget> iWidget
                                     , const TArray<FStylusState>& NewStates
                                     , int32 StylusIndex );

protected:
    void PopupContextMenu();
    TSharedPtr<SWidget> CreateContextMenu();
    virtual void ExtendContextMenu( FMenuBuilder& menu );
    FSceneView* GetSceneView( FEditorViewportClient* iViewportClient );
    FEditorViewportClient* GetActiveViewportClient();
    void DrawLayerOrientationGrid( IToolsContextRenderAPI* RenderAPI, UArianeLayerDrawing* DrawingLayer );
/* Gary

    virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    virtual bool OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);


    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);
    // For global key press events
    virtual bool OnKeyUpGlobal(const FKeyEvent& InKeyEvent);
    virtual bool OnKeyDownGlobal(const FKeyEvent& InKeyEvent);

    // Tick

    //Finishes any action currently running, does not validate the action (example, ensure any drawing in queue is done)
    virtual void Flush();

    //Validates any action that finished. (example, any drawing in queue is finished and validated so that it creates an undoable state)
    virtual void Commit();
*/



public:
    // Interface
    virtual EMouseCursor::Type GetMouseCursor() const;
/* Gary
    virtual void BindShortcuts(TSharedPtr<FUICommandList> iCommandList);
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender );
    virtual void ExtendToolbar( UToolMenu* iToolMenu );
    virtual TSharedPtr<FOdysseyHUDElement> GetHUD();

    virtual bool IsHUDVisible() const;

    virtual bool SupportsColorType(EArianeEditorColorType iType);
*/

protected:
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive);
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive);

protected:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PostInitProperties() override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

protected:
    // Temp
    void FlushStylusInput();
    void ListenStylusInput();
    void IgnoreStylusInput();

// HUD Primitives
protected:
    void DrawHUDCircle( FCanvas* Canvas, double X, double Y, double Radius, uint32 Steps );

public:
    FArianeEditor* GetEditor() const;
/* Gary
    //template<class T> T* GetEditorAs() const { return static_cast<T*>(mEditor); };
*/

protected:
/*
    TSharedPtr<FArianeEditorToolInputProcessor> mInputProcessor;
    TSharedPtr<FOdysseyHUDElement>      mHUD;

*/
    FArianeEditor* Editor;
    TSharedPtr<FUICommandList> CommandList;
    bool bHasContextMenu;
    bool bInited;
    FKey PressedKey;
    // Temp
    double Pressure;

public:
    const FSlateBrush* Icon;
};
