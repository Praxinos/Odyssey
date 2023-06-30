// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorGUI.h"

#include "OdysseyTextureEditorLayerStackTab.h"
#include "OdysseyTextureEditorTextureDetailsTab.h"
#include "OdysseyTextureEditorVectorContextMenu.h"

class FOdysseyTextureEditor;

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
    virtual void CreateContextMenus() override;
    virtual void BindShortcuts( FBaseToolkit* iToolkit ) override;
    virtual void ExtendMenuAbout( FToolMenuOwner iOwner, FName iMenuName ) override;

public:
    // GettersFName
	virtual FName GetLayoutName() override;
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>& GetLayerStackTab();
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>& GetTextureDetailsTab();
	TSharedPtr<FOdysseyTextureEditorVectorContextMenu>& GetVectorContextMenu();

protected:
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection() override;


public:
    virtual void ResetView();
    virtual void GroupPaint();
    virtual void Group();
    virtual void Ungroup();
    virtual void BringForward();
    virtual void SendBackward();
    virtual void DeleteSelection();
    virtual void FlipHorizontal();
    virtual void FlipVertical();

public:
    void BindVectorScene( FOdysseyVectorScene* iScene );
protected:
    void OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags );
    void OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack );

private:
	FOdysseyTextureEditor* mEditor;

private:
    //Tabs
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>          mLayerStackTab;
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>      mTextureDetailsTab;
	TSharedPtr<FOdysseyTextureEditorVectorContextMenu>      mVectorContextMenu;
};
