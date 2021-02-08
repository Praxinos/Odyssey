// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture2D.h"

class FOdysseyTextureEditorData;
class FOdysseyTextureEditorGUI;
class FOdysseyTextureEditorController;

/**
 * Implements an Editor for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditor();
    FOdysseyTextureEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit); //Non Initialized constructor
    FOdysseyTextureEditor(UTexture2D* iTexture, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit);

public:
    virtual void Init() override;

public:
    //TEMPORARY
    TSharedPtr<FOdysseyTextureEditorData> GetData();

public:
    virtual bool OnCloseRequested();
    virtual const TSharedRef<FTabManager::FLayout>& CreateLayout() const override;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const override;
    
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

private:
	TSharedPtr<FOdysseyTextureEditorData> mData;
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
	TSharedPtr<FOdysseyTextureEditorController> mController;
};

