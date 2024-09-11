// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorGUI.h"

class FOdysseyTextureEditorExtension;
class UOdysseyLayerStack;
class FOdysseyVectorGroupPaint;

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

protected:
	TSharedRef<FTabManager::FSplitter>	CreateRightSection();

public:
    void BindVectorScene( FOdysseyVectorGroupPaint* iScene );

protected:
    void OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack );
    void OnSourceChanged();

private:
	FOdysseyTextureEditorExtension* mExtension;
};
