// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "PaperFlipbook.h"

class FOdysseyFlipbookEditorData;
class FOdysseyFlipbookEditorGUI;
class FOdysseyFlipbookEditorController;

/**
 * Implements an Editor for flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditor
    : public TOdysseyPainterEditor<UPaperFlipbook>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditor();
    FOdysseyFlipbookEditor();

public:
    virtual void Init();

public:
    virtual TArray<UObject*> GetAllEditedObjects() override;
    virtual void OnToolkitInitialized() override;
    virtual bool OnCloseRequested();
    virtual const TSharedRef<FTabManager::FLayout>& CreateLayout() const override;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const override;
    
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

private:
    void SetTimelineNavigationShortcuts(TSharedPtr<SWidget> iWidget);
    void OnSpriteCreated(UPaperSprite* iSprite);
    void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);
    void OnTextureCreated(UTexture2D* iTexture);
    void OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe);

private:
	TSharedPtr<FOdysseyFlipbookEditorData> mData;
	TSharedPtr<FOdysseyFlipbookEditorGUI> mGUI;
	TSharedPtr<FOdysseyFlipbookEditorController> mController;

    FDelegateHandle mOnSpriteTextureChangedHandle;
};

