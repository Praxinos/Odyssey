// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorGUI.h"

class FOdysseyTextureEditorExtension;
class UOdysseyLayerStack;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorGUI
    : public TSharedFromThis<FOdysseyTextureEditorGUI>
{
public:
    static void ExtendLevelEditorLayout(FLayoutExtender& Extender);
    
public:
    // Construction / Destruction
    ~FOdysseyTextureEditorGUI();
    FOdysseyTextureEditorGUI(FOdysseyTextureEditorExtension* iExtension);

public:
    void Init();
    void BindShortcuts( FBaseToolkit* iToolkit );
    void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );
	void BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder);

private:
    //Init
	void CreateTabs();
    void ExtendMenuAbout( FToolMenuOwner iOwner, FName iMenuName );

protected:
	TSharedRef<FTabManager::FSplitter>	CreateRightSection();

public:
    void ResetView();
    void GroupPaint();
    void Group();
    void Ungroup();
    void BringForward();
    void SendBackward();
    void DeleteSelection();
    void FlipHorizontal();
    void FlipVertical();
    void StitchVertices();
 
public:
    void BindVectorScene( FOdysseyVectorScene* iScene );
protected:
    void OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags );
    void OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack );

private:
	FOdysseyTextureEditorExtension* mExtension;
};
