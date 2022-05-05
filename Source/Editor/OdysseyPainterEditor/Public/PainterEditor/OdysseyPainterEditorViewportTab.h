// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"

class FOdysseyPainterEditor;
class SOdysseySurfaceViewport;
class IOdysseySurface;
class FOdysseyPainterEditorViewportClient;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorViewportTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorViewportTab();
    FOdysseyPainterEditorViewportTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

public:
    // Public Getters
    TSharedPtr<SOdysseySurfaceViewport> GetViewport();

protected:
    // Widget Getters
    virtual IOdysseySurface* Surface() const;

protected:
    // Event Listeners
    virtual void HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture);

    virtual bool OnViewportMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnViewportMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnViewportMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnViewportMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnViewportKeyDown(const FKey& iKey);
    virtual bool OnViewportKeyUp(const FKey& iKey);

protected:
    //Shortcuts
    virtual void OnResetViewportPosition();
    virtual void OnResetViewportRotation();
    virtual void OnRotateViewportLeft();
    virtual void OnRotateViewportRight();
    virtual void OnSetZoom(double iZoomValue);
    virtual void OnSetZoomFitScreen();
    virtual void OnZoomInExponential();
    virtual void OnZoomOutExponential();

    //HUD
    virtual void OnViewportSizeChanged(FViewport* iViewport, uint32 iUnused);

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SOdysseySurfaceViewport> mViewport;
    TSharedPtr<FOdysseyPainterEditorViewportClient> mViewportClient;
};

