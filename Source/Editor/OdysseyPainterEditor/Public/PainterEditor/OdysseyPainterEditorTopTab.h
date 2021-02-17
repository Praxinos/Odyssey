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

protected:
    // Widget Getters
    virtual float Size() const;
    virtual float Opacity() const;
    virtual float Flow() const;
    virtual ::ul3::eBlendingMode BlendingMode() const;
    virtual ::ul3::eAlphaMode AlphaMode() const;

public:
    // Public Getters
    TSharedPtr<SOdysseyPaintModifiers> PaintModifiers();

protected:
    // Event Listeners
    virtual void OnSizeChanged( int32 iValue );
    virtual void OnOpacityChanged( int32 iValue );
    virtual void OnFlowChanged( int32 iValue );
    virtual void OnBlendingModeChanged( int32 iValue );
    virtual void OnAlphaModeChanged( int32 iValue );

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SOdysseyPaintModifiers> mPaintModifiers;
};

