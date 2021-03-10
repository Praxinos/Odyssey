// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyTextureEditor.h"
#include "OdysseyViewportDrawingEditorGUI.h"

struct FPaintableTexture;

/**
 * Implements an Editor for textures.
 */
class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditor
    : public FOdysseyTextureEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditor();
    FOdysseyViewportDrawingEditor();

public:
    // Initialization
    virtual void InitData() override;

public:
    // Getters
    TArray<FPaintableTexture>& PaintableTextures(); //Selectable Textures

public:
    // Overrides
    virtual FOdysseyViewportDrawingEditorGUI* GetGUI() override;
    virtual void OnPreTextureChange(UTexture2D* iNewTexture) override;
    virtual void OnPostTextureChange(UTexture2D* iOldTexture) override;
    
private:
	TSharedPtr<FOdysseyViewportDrawingEditorGUI> mGUI;
    TArray<FPaintableTexture> mPaintableTextures;
};

