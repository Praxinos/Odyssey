// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorTopTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorTopTab();
    FOdysseyPainterEditorTopTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual TSharedRef< SDockTab > SpawnTab( const FSpawnTabArgs& iArgs ) override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

public:
    // Setters
    void IsVertical(bool iIsVertical);

protected:
    // Widget Getters
    virtual float Size() const;
    virtual float Opacity() const;
    virtual float Flow() const;
    virtual ::ul3::eBlendingMode BlendingMode() const;
    virtual ::ul3::eAlphaMode AlphaMode() const;

protected:
    // Event Listeners
    virtual void OnSizeChanged( int32 iValue );
    virtual void OnOpacityChanged( int32 iValue );
    virtual void OnFlowChanged( int32 iValue );
    virtual void OnBlendingModeChanged( int32 iValue );
    virtual void OnAlphaModeChanged( int32 iValue );

protected:
    // Methods
    virtual void SetAlphaMode(::ul3::eAlphaMode iAlphaMode);
    virtual void AddSize(int32 iValue);

private:
    FOdysseyPainterEditor* mEditor;
    bool mIsVertical;
};

