// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

