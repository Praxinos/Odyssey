// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyTextureEditor;
class UOdysseyLayerStack;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorLayerStackTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorLayerStackTab();
    FOdysseyTextureEditorLayerStackTab(FOdysseyTextureEditor* iEditor);

protected:
    // FOdysseyTextureEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;

protected:
    // Widget Getters
    virtual UOdysseyLayerStack* LayerStack() const;

protected:
    // Event Listeners

private:
    //Methods
    virtual void ExtendMenuFile( FToolMenuOwner iOwner, FName iMenuName );
    virtual void ImportTexturesAsLayers();
    virtual void ExportTextureToOperatingSystem();
    virtual void ExportLayersAsTextures();
    virtual void ExportCurrentLayerAsTexture();
    virtual void CreateNewLayer();
    //virtual void DuplicateCurrentLayer();
    //virtual void DeleteCurrentLayer();
    virtual void ChangeLayerOpacity(float iOpacity);

private:
    FOdysseyTextureEditor* mEditor;
};

