// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
// Ariane
#include "ArianeEditorTool.generated.h"

class FArianeEditor;
/* Gary
class FArianeEditorToolInputProcessor;
*/

UCLASS()
class ARIANEEDITOR_API UArianeEditorTool : public UObject
{
    GENERATED_BODY()

    DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SWidget>, FContextMenuFunc, FMenuBuilder&)

    public:
        // Destructor
        virtual ~UArianeEditorTool();

        //Constructor
        UArianeEditorTool();
        UArianeEditorTool( FContextMenuFunc iContextMenufunc );

    public:
        //Activates the tool
        UFUNCTION(BlueprintCallable, Category="Tools")
        virtual void Activate();

        //Inactivates the tool
        UFUNCTION(BlueprintPure, Category="Tools")
        virtual bool IsActivable() const;

        //Inactivates the tool
        UFUNCTION(BlueprintPure, Category="Tools")
        virtual bool IsActivated() const;

        //Inactivates the tool
        UFUNCTION(BlueprintCallable, Category="Tools")
        virtual void Inactivate();

        virtual void Load();
        virtual void Unload();

    public:
        //Mouse events
        virtual bool OnMouseDown( const FVector2D& iViewportCoords, const FKey& iKey, bool iRepeat = false );
        virtual void OnMouseHover( const FVector2D& iViewportCoords );
        virtual void OnMouseDrag( const FVector2D& iViewportCoords );
        virtual bool OnMouseUp( const FVector2D& iViewportCoords, const FKey& iKey );
        virtual void Tick( float iDeltaTime );
        virtual FText GetTooltip() const;

    protected:
        void PopupContextMenu();
        TSharedPtr<SWidget> CreateContextMenu();
        void ExtendContextMenu( FMenuBuilder& menu );

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
        void SetEditor(class FArianeEditor* iEditor);
        FArianeEditor* GetEditor() const;
/* Gary
        //template<class T> T* GetEditorAs() const { return static_cast<T*>(mEditor); };
*/

    protected:
/*
        TSharedPtr<FArianeEditorToolInputProcessor> mInputProcessor;
        TSharedPtr<FOdysseyHUDElement>      mHUD;

*/
        FArianeEditor* mEditor;
        TSharedPtr<FUICommandList> mCommandList;
        FContextMenuFunc mContextMenuFunc;

    public:
        UPROPERTY(EditDefaultsOnly, Category = "Tool")
        FSlateBrush Icon;
};
