// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "PaperFlipbook.h"

#include "OdysseyFlipbookWrapper.h"

class FOdysseyFlipbookEditorData;
class FOdysseyFlipbookEditorGUI;
class FOdysseyFlipbookEditorController;

/**
 * Implements an Editor for flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditor();
    FOdysseyFlipbookEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit); //Non-Initialized Contructor
    FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit);

public:
    virtual void Init() override;

public:
    //TEMPORARY
    TSharedPtr<FOdysseyFlipbookEditorData> GetData();

public:
    virtual void OnToolkitInitialized() override;
    virtual bool OnCloseRequested();
    virtual const TSharedRef<FTabManager::FLayout>& CreateLayout() const override;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const override;
    
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

private:
    TSharedPtr<FOdysseyFlipbookWrapper> mFlipbookWrapper;

	TSharedPtr<FOdysseyFlipbookEditorData> mData;
	TSharedPtr<FOdysseyFlipbookEditorGUI> mGUI;
	TSharedPtr<FOdysseyFlipbookEditorController> mController;

    FDelegateHandle mOnSpriteTextureChangedHandle;
};

