// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyEditorShortcuts.h"

class FOdysseyPainterEditor;
class FUICommandList;
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGlobalTextureShortcuts
    : public FOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyPainterEditorGlobalTextureShortcuts() {};
    FOdysseyPainterEditorGlobalTextureShortcuts(FOdysseyPainterEditor* iEditor);

public:
    //Shortcuts
    void Action_ExportLayersAsTextures();
    void Action_ExportLayersAsImages();
    void Action_ImportImages();
    void Action_ImportTextures();

    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    FOdysseyPainterEditor* mEditor;
};
