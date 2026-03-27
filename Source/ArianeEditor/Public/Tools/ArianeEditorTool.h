// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
// Ariane
#include "ArianePointerState.h"
// OdysseyHeaders
#include "OdysseyPainterEditorColorType.h"

#include "ArianeEditorTool.generated.h"

class FArianeEditor;
class FSceneView;
class FStylusState;

/* Gary
class FArianeEditorToolInputProcessor;
*/

UCLASS()
class ARIANEEDITOR_API UArianeEditorTool : public UObject
{
    GENERATED_BODY()

    public:
        // Destructor
        virtual ~UArianeEditorTool();

        //Constructor
        UArianeEditorTool();

    public:
        //Mouse events
        virtual bool OnMouseDown( FEditorViewportClient* iViewportClient
                                , const FKey& iKey
                                , const FArianePointerState& State
                                , bool iRepeat = false );
        virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                                 , const FArianePointerState& State );
        virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                                , const FArianePointerState& State );
        virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                              , const FKey& iKey
                              , const FArianePointerState& State );
        virtual bool OnMouseClick( FEditorViewportClient* iViewportClient
                                 , const FKey& iKey
                                 , const FArianePointerState& State );

        virtual void Tick( float iDeltaTime );

        virtual void DrawHUD ( FEditorViewportClient* ViewportClient
                             , FViewport* Viewport
                             , const FSceneView* View
                             , FCanvas* Canvas );

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
        /** Callback for when the tool is loaded */
        virtual void Load();
        /** Callback for when the tool is unloaded */
        virtual void Unload();

        virtual bool SupportsColorType( EOdysseyPainterEditorColorType ColorType );

    protected:
        void PopupContextMenu();
        TSharedPtr<SWidget> CreateContextMenu();
        virtual void ExtendContextMenu( FMenuBuilder& menu );
        FSceneView* GetSceneView( FEditorViewportClient* iViewportClient );
/* Gary

        virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual bool OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);


        virtual bool OnKeyDown(const FKey& iKey);
        virtual bool OnKeyUp(const FKey& iKey);
        // For global key press events
        virtual bool OnKeyUpGlobal(const FKeyEvent& InKeyEvent);
        virtual bool OnKeyDownGlobal(const FKeyEvent& InKeyEvent);

        // Tick
        virtual void Tick(float iDeltaTime);

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

    public:
        virtual void Init( FArianeEditor* iEditor );
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

    public:
        UPROPERTY(EditDefaultsOnly, Category = "Tool")
        FSlateBrush Icon;
};
