// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

class FOdysseyViewportDrawingEditorExtension;

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyViewportDrawingEditorGUI
{
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
