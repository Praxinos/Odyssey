// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/BaseToolkit.h"

#include "OdysseyToolkit.h"

class ODYSSEYEDITOR_API FOdysseyModeToolkit
	: public TOdysseyToolkit<FModeToolkit>,
      public IAssetEditorInstance
{
public:
    // Construction / Destruction
    virtual ~FOdysseyModeToolkit();
    FOdysseyModeToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor, class FEdMode* iEditorMode);

public:
    virtual void Initialize() override;

	/** IToolkit interface */
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);

    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& iTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& iTabManager) override;

public:
    //from IAssetEditorInstance
	virtual FName GetEditorName() const override;
	virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr) override;
	virtual bool CloseWindow() override;
	virtual bool IsPrimaryEditor() const override;
	virtual void InvokeTab(const struct FTabId& TabId) override;
	virtual FName GetToolbarTabId() const override;
	virtual TSharedPtr<class FTabManager> GetAssociatedTabManager() override;
	virtual double GetLastActivationTime() override;
	virtual void RemoveEditingAsset(UObject* Asset) override;
	virtual void ExtendMenu() override;

private:
	/** Owning editor mode */
	class FEdMode* mEditorMode;
};
