// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorToolkit.h"

#include <ULIS3>

class FOdysseyFlipbookEditorData;
class FOdysseyFlipbookEditorGUI;
class FOdysseyFlipbookEditorController;

class UPaperFlipbook;

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class FOdysseyFlipbookEditorToolkit
    : public FOdysseyPainterEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorToolkit();
    FOdysseyFlipbookEditorToolkit();
	void Init(const EToolkitMode::Type iMode, const TSharedPtr< class IToolkitHost >& iInitToolkitHost, const FName& iAppIdentifier, UPaperFlipbook* iFlipbook);

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

	/** Called to check to see if there's an asset capable of being reimported */
	virtual bool CanReimport() const;
	virtual bool CanReimport(UObject* EditingObject) const;

protected:
    // FOdysseyPainterEditorToolkit overrides
    virtual const TSharedRef<FTabManager::FLayout>& GetLayout() const override;
    virtual const TArray<TSharedPtr<FExtender>>& GetMenuExtenders() const override;

protected:
	void OnSpriteCreated(UPaperSprite* iSprite);
    void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);
	void OnTextureCreated(UTexture2D* iTexture);
	void OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe);

private:
    void SetTimelineNavigationShortcuts(TSharedPtr<SWidget> iWidget);

private:
	TSharedPtr<FOdysseyFlipbookEditorData> mData;
	TSharedPtr<FOdysseyFlipbookEditorGUI> mGUI;
	TSharedPtr<FOdysseyFlipbookEditorController> mController;

	FDelegateHandle mOnSpriteTextureChangedHandle;
};

