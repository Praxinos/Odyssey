// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorViewportClient.h"
#include "Slate/SceneViewport.h"

FOdysseyViewportDrawingEditorViewportClient::FOdysseyViewportDrawingEditorViewportClient() :
    FLevelEditorViewportClient( nullptr ),
    mTrueViewport(nullptr),
    mViewportClient( nullptr )
{
}

EMouseCaptureMode FOdysseyViewportDrawingEditorViewportClient::GetMouseCaptureMode() const
{
    return EMouseCaptureMode::NoCapture;
}

bool FOdysseyViewportDrawingEditorViewportClient::IgnoreInput()
{
    if( mViewportClient )
    {
        mTrueViewport->SetViewportClient(mViewportClient);
        mViewportClient = nullptr;
    }
    return false;
}

void FOdysseyViewportDrawingEditorViewportClient::SetViewport(FViewport* iViewport)
{
    mTrueViewport = iViewport;
}


void FOdysseyViewportDrawingEditorViewportClient::SetViewportClient(FViewportClient* iViewportClient)
{
    mViewportClient = iViewportClient;
}

