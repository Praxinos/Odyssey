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
	FOdysseyModeToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor, class FEdMode* iEditorMode);
	
	virtual void Initialize() override;

	/** IToolkit interface */
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);

public:
    //IAssetEditorInstance
	virtual FName GetEditorName() const;
	virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr);
	virtual bool CloseWindow();
	virtual bool IsPrimaryEditor() const;
	virtual void InvokeTab(const struct FTabId& TabId);
	virtual FName GetToolbarTabId() const;
	virtual TSharedPtr<class FTabManager> GetAssociatedTabManager();
	virtual double GetLastActivationTime();
	virtual void RemoveEditingAsset(UObject* Asset);

private:
	/** Owning editor mode */
	class FEdMode* mEditorMode;
};
