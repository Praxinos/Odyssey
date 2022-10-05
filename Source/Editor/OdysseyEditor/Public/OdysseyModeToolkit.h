// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/BaseToolkit.h"

#include "OdysseyToolkit.h"

class ODYSSEYEDITOR_API FOdysseyModeToolkit
	: public FModeToolkit,
      public IAssetEditorInstance
{
public:
    // Construction / Destruction
    virtual ~FOdysseyModeToolkit();
    FOdysseyModeToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor, class FEdMode* iEditorMode);

public:
    //virtual void Initialize();
    virtual void Init(const TSharedPtr<IToolkitHost>& iInitToolkitHost, TWeakObjectPtr<UEdMode> iOwningMode);


	/** IToolkit interface */
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);

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
	virtual void ExtendMenu();


	TSharedPtr<FOdysseyEditor> mEditor;
	
private:
	/** Owning editor mode */
	class FEdMode* mEditorMode;
};
