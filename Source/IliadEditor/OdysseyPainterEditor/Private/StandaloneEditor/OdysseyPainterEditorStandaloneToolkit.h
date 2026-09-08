// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Toolkits/AssetEditorToolkit.h"

class FOdysseyPainterEditor;
class FTabManager;

class FOdysseyPainterEditorStandaloneToolkit
    : public FAssetEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorStandaloneToolkit();
    FOdysseyPainterEditorStandaloneToolkit(UObject* iEditedObject);

public:
    void Open();

protected:
    // FAssetEditorToolkit interface
    virtual void SaveAssetAs_Execute() override;
    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& iTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& iTabManager) override;
    virtual FText GetToolkitName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FText GetToolkitToolTipText() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual void InitToolMenuContext(FToolMenuContext& MenuContext) override;

    /** Called to check to see if there's an asset capable of being reimported */
        virtual bool CanReimport() const;
    virtual bool CanReimport(UObject* EditingObject) const;

private:
    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);

private:
    UObject* mEditedObject;
    FName mAppIdentifier;
    FText mTitle;
    FName mLayoutName;
    FString mWorldCentricTabPrefix;
    TSharedPtr<FOdysseyPainterEditor> mEditor;

private:
    // Telemetry
    FDateTime SessionStartTime;
};
