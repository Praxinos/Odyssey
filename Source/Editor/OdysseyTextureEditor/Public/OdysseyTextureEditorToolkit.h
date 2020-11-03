// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorToolkit.h"

#include <ULIS3>

class FOdysseyTextureEditorData;
class FOdysseyTextureEditorGUI;
class FOdysseyTextureEditorController;

class UTexture;

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyTextureEditorToolkit
    : public FOdysseyPainterEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorToolkit();
    FOdysseyTextureEditorToolkit();
	void Init(const EToolkitMode::Type iMode, const TSharedPtr< class IToolkitHost >& iInitToolkitHost, const FName& iAppIdentifier, UTexture2D* iTexture);
protected:
    //FAssetEditorToolkit override
    virtual void SaveAsset_Execute() override;
    virtual void SaveAssetAs_Execute() override;
    virtual bool OnRequestClose() override;

	void OpenAsset(UObject* iObject);

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
    virtual FName GetToolkitFName() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    
protected:
    // FAssetEditorToolkit interface
    virtual void RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

protected:
    // FOdysseyPainterEditorToolkit overrides
    virtual const TSharedRef<FTabManager::FLayout>& GetLayout() const override;
    virtual const TArray<TSharedPtr<FExtender>>& GetMenuExtenders() const override;

private:
	TSharedPtr<FOdysseyTextureEditorData> mData;
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
	TSharedPtr<FOdysseyTextureEditorController> mController;
};

