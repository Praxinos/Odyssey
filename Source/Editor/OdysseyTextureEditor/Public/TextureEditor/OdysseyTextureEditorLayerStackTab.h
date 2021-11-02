// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyTextureEditor;
class FOdysseyLayerStack;
class SOdysseyLayerStackView;

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
    virtual void FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender) override;

protected:
    // Widget Getters
    virtual FOdysseyLayerStack* LayerStack() const;

protected:
    // Event Listeners

private:
    //Methods
    virtual void ExtendMenuFile(FMenuBuilder& ioMenuBuilder);
    virtual void ImportTexturesAsLayers();
    virtual void ExportTextureToOperatingSystem();
    virtual void ExportLayersAsTextures();
    virtual void ExportCurrentLayerAsTexture();
    virtual void CreateNewLayer();
    virtual void DuplicateCurrentLayer();
    virtual void DeleteCurrentLayer();
    virtual void ChangeLayerOpacity(float iOpacity);

private:
    FOdysseyTextureEditor* mEditor;
    TSharedPtr<SOdysseyLayerStackView> mLayerStackView;
};

