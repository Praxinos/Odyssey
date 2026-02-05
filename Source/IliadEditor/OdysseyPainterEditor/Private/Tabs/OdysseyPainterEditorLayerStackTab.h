// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditorTextureExportShortcuts.h"

class FOdysseyPainterEditor;
class UOdysseyLayerStack;

class FOdysseyPainterEditorLayerStackTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorLayerStackTab();
    FOdysseyPainterEditorLayerStackTab(FOdysseyPainterEditor* iEditor);

protected:
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender ) override;
    virtual bool CanOpen() const override;

protected:
    // Widget Getters
    UOdysseyLayerStack* LayerStack() const;

protected:
    // Event Listeners

private:
    //Methods
    void ExtendMenuFile( TSharedRef<FExtender> iExtender );
    void BuildImportMenu(FMenuBuilder& iMenuBuilder);
    void BuildExportMenu(FMenuBuilder& iMenuBuilder);


    void ExportLayersAsTextures();
    void ExportLayersAsImages();
    void ImportImages();
    void ImportTextures();

private:
    FOdysseyPainterEditor* mEditor;
    FOdysseyPainterEditorTextureExportShortcuts mTextureExportShortcuts;
};
