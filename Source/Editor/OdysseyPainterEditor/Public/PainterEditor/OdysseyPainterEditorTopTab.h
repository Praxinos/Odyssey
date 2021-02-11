// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorTopTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorTopTab();
    FOdysseyPainterEditorTopTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual TSharedRef< SDockTab > SpawnTab( const FSpawnTabArgs& iArgs ) override;

public:
    // Public Getters
    TSharedPtr<SOdysseyPaintModifiers> PaintModifiers();

private:
    // Event Listeners
    void OnSizeModifierChanged( int32 iValue );
    void OnOpacityModifierChanged( int32 iValue );
    void OnFlowModifierChanged( int32 iValue );
    void OnBlendingModeModifierChanged( int32 iValue );
    void OnAlphaModeModifierChanged( int32 iValue );

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SOdysseyPaintModifiers> mPaintModifiers;
};

