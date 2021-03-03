// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyStrokeOptions.h"
#include "SOdysseyTextureDetails.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "OdysseyPainterEditorGUI.h"

class SDockableTab;
class STextBlock;
class SOdysseySurfaceViewport;
class FOdysseyTextureEditor;
class FOdysseyTextureEditorController;

class FOdysseyTextureEditorLayerStackTab;
class FOdysseyTextureEditorTextureDetailsTab;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorGUI :
	public FOdysseyPainterEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorGUI();
    FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor);

protected:
    //Init
	virtual void CreateTabs() override;

public:
    // GettersFName
	virtual FName GetLayoutName() override;
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>& GetLayerStackTab();
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>& GetTextureDetailsTab();

protected:
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();

private:
	FOdysseyTextureEditor* mEditor;

private:
    //Tabs
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>          mLayerStackTab;
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>      mTextureDetailsTab;
};

