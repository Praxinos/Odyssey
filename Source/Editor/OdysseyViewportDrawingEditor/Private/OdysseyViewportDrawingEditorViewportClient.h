// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LevelEditorViewport.h" 
#include "SLevelViewport.h"

class FOdysseyViewportDrawingEditorViewportClient
    : public FLevelEditorViewportClient
{
public:
    FOdysseyViewportDrawingEditorViewportClient();

public:
    virtual EMouseCaptureMode GetMouseCaptureMode() const override;
    virtual bool              IgnoreInput() override;

    void SetViewport(FViewport* iViewport);
    void SetViewportClient( FViewportClient* iViewportClient );

private:
    FViewport*                mTrueViewport;
    FViewportClient*          mViewportClient;
};