// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "TickableEditorObject.h"

#include "OdysseyEditorTab.h"


class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorTiledViewportTab
    : public FOdysseyEditorTab
    , public FTickableEditorObject
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorTiledViewportTab();
    FOdysseyPainterEditorTiledViewportTab(class FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual void Init() override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

public:
    // FTickableEditorObject
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyPainterEditorTiledViewportTab, STATGROUP_Tickables); }

private:
    void UpdateViewportTextureRenderer();
    void OnEditorSourceChanged();

private:
    class FOdysseyPainterEditor* mEditor;

    TSharedPtr<class SViewport> mViewportWidget;
    TSharedPtr<class FOdysseyPainterEditorTiledViewportClient> mViewportClient;
    TSharedPtr<class FSceneViewport> mSceneViewport;
};
