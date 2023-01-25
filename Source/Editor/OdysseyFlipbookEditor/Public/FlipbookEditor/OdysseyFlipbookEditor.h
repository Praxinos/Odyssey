// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "TextureEditor/OdysseyTextureEditor.h"

#include "OdysseyFlipbookEditorGUI.h"
#include "OdysseyFlipbookWrapper.h"

class FOdysseyFlipbookEditorGUI;
class FOdysseySurfaceTexture2D;

/**
 * Implements an Editor for flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditor
    : public FOdysseyTextureEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditor();
    FOdysseyFlipbookEditor(); //Non-Initialized Contructor
    FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook);

public:
    //Initialization
    virtual void InitData(UObject* iEditedObject) override;

public:
    // Getters
	TSharedPtr<FOdysseyFlipbookWrapper>&    FlipbookWrapper();
    FOdysseySurfaceTexture2D*			    PreviewSurface();

public:
    //Overrides
    virtual FOdysseyFlipbookEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

protected:
    virtual void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);
    virtual void SetTextureAtKeyframeIndex(int32 iKeyframeIndex);

private:
    TSharedPtr<FOdysseyFlipbookWrapper> mFlipbookWrapper; //TODO: No need for a SharedPtr here, and no need for a pointer here
	FOdysseySurfaceTexture2D*                mPreviewSurface; //TODO: No need for a pointer here I guess

	TSharedPtr<FOdysseyFlipbookEditorGUI> mGUI;
};

