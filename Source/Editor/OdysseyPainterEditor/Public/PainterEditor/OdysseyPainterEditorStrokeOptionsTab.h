// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"

class FOdysseyPainterEditor;
class SOdysseyStrokeOptions;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorStrokeOptionsTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorStrokeOptionsTab();
    FOdysseyPainterEditorStrokeOptionsTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters

public:
    // Public Getters
    TSharedPtr<SOdysseyStrokeOptions> StrokeOptions();

private:
    // Event Listeners
    virtual void OnStrokeStepChanged( int32 iValue );
    virtual void OnStrokeAdaptativeChanged( bool iValue );
    virtual void OnStrokePaintOnTickChanged( bool iValue );
    virtual void OnInterpolationTypeChanged( int32 iValue );
    virtual void OnSmoothingMethodChanged( int32 iValue );
    virtual void OnSmoothingStrengthChanged( int32 iValue );
    virtual void OnSmoothingEnabledChanged( bool iValue );
    virtual void OnSmoothingRealTimeChanged( bool iValue );
    virtual void OnSmoothingCatchUpChanged( bool iValue );

private:
    FOdysseyPainterEditor* mEditor;
    TSharedPtr<SOdysseyStrokeOptions> mStrokeOptions;
};

