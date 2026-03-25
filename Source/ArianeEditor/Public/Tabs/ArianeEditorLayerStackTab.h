// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "ArianeEditorTab.h"

class FArianeEditor;
class UArianeLayerStack;

class FArianeEditorLayerStackTab :
    public FArianeEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FArianeEditorLayerStackTab();
    FArianeEditorLayerStackTab(FArianeEditor* iEditor);

protected:
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender ) override;
    virtual bool CanOpen() const override;

protected:
    // Event Listeners

private:
    //Methods
    void MapActions( TSharedPtr<FUICommandList> iCommandList );
    virtual void ExtendMenuFile( TSharedRef<FExtender> iExtender );

private:
    FArianeEditor* Editor;
};
