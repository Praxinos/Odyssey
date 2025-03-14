// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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

    //BIG BIG PATCH: let's make that bad guy destroy itself MOUAHAHAHAHA
    delete this;
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
