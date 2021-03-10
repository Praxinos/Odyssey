// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS3>

class FOdysseyViewportDrawingEditor;

class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorTextureSelectorTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorTextureSelectorTab();
    FOdysseyViewportDrawingEditorTextureSelectorTab(FOdysseyViewportDrawingEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    FString PaintTexturePath() const;
    bool ShouldFilterTextureAsset(const FAssetData& iAssetData) const;

protected:
    // Event Listeners
    void OnObjectChanged(const FAssetData& iAssetData);

private:
    FOdysseyViewportDrawingEditor* mEditor;
};

