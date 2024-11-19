// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

class FOdysseyViewportDrawingEditorExtension;

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyViewportDrawingEditorGUI
{
public:
    static void ExtendLevelEditorLayout(FLayoutExtender& Extender);

public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorGUI();
    FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditorExtension* iExtension);

public:
    void Initialize();
    void Finalize();

private:
    void CustomizeAnimationEditorTimeline();

private:
    FOdysseyViewportDrawingEditorExtension* mExtension;
};
