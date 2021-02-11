// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorViewportTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorViewportTab();
    FOdysseyPainterEditorViewportTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts() override;

public:
    // Public Getters
    TSharedPtr<SOdysseySurfaceViewport> GetViewport();

protected:
    // Widget Getters
    virtual IOdysseySurface* Surface() const;

protected:
    // Event Listeners
    virtual void OnViewportParameterChanged();
    virtual void HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture);

protected:
    //Shortcuts
    virtual void OnResetViewportPosition();
    virtual void OnResetViewportRotation();
    virtual void OnRotateViewportLeft();
    virtual void OnRotateViewportRight();
    virtual void OnSetZoom(double iZoomValue);
    virtual void OnSetZoomFitScreen();
    virtual void OnZoomIn();
    virtual void OnZoomOut();

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SOdysseySurfaceViewport> mViewport;
};

