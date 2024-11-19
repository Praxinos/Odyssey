// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyTextureEditorExtension;
class UOdysseyLayerStack;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorLayerStackTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorLayerStackTab();
    FOdysseyTextureEditorLayerStackTab(FOdysseyTextureEditorExtension* iExtension);

protected:
    // FOdysseyTextureEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender ) override;

protected:
    // Widget Getters
    virtual UOdysseyLayerStack* LayerStack() const;

protected:
    // Event Listeners

private:
    //Methods
    void MapActions( TSharedPtr<FUICommandList> iCommandList );
    virtual void ExtendMenuFile( TSharedRef<FExtender> iExtender );
    virtual void ImportTexturesAsLayers();
    virtual void ExportTextureToOperatingSystem();
    virtual void ExportLayersAsTextures();
    virtual void ExportCurrentLayerAsTexture();
    virtual void CreateNewLayer();
    virtual void ChangeLayerOpacity(float iOpacity);

private:
    FOdysseyTextureEditorExtension* mExtension;
};
