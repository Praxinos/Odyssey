// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EdMode.h"
#include "OdysseyViewportDrawingEditorEdModeToolkit.h"
#include "EditorModeManager.h"

#include "OdysseyViewportDrawingEditorPainter.h"

const FEditorModeID FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId = TEXT("EM_OdysseyViewportDrawingEditorEdMode");

void FOdysseyViewportDrawingEditorEdMode::Initialize()
{
	MeshPainter = FOdysseyViewportDrawingEditorPainter::Get();
}

TSharedPtr<class FModeToolkit> FOdysseyViewportDrawingEditorEdMode::GetToolkit()
{
	return MakeShareable(new FOdysseyViewportDrawingEditorEdModeToolkit(this));
}

bool FOdysseyViewportDrawingEditorEdMode::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
	if (!IsEditingEnabled())
	{
		return false;
	}

	return IMeshPaintEdMode::InputKey( iViewportClient, iViewport, iKey, iEvent);
}

void FOdysseyViewportDrawingEditorEdMode::Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI)
{
}

bool FOdysseyViewportDrawingEditorEdMode::ProcessCapturedMouseMoves(FEditorViewportClient* InViewportClient,FViewport* InViewport,const TArrayView<FIntPoint>& CapturedMouseMoves)
{
    //If we draw by using the stylus, we ignore the mouse events here
    FOdysseyViewportDrawingEditorPainter* painter = (FOdysseyViewportDrawingEditorPainter*)MeshPainter;
    if(painter->IsCapturedByStylus())
        return false;

    UE_LOG(LogTemp, Display, TEXT("CaptureMouseMove"))

    // We only care about perspective viewpo1rts
    bool bPaintApplied = false;
    if(InViewportClient->IsPerspective())
    {
        if(MeshPainter->IsPainting() && CapturedMouseMoves.Num() > 0)
        {
            // Compute a world space ray from the screen space mouse coordinates
            FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
                InViewportClient->Viewport,
                InViewportClient->GetScene(),
                InViewportClient->EngineShowFlags)
                .SetRealtimeUpdate(InViewportClient->IsRealtime()));
            FSceneView* View = InViewportClient->CalcSceneView(&ViewFamily);

            TArray<TPair<FVector,FVector>> Rays;
            Rays.Reserve(CapturedMouseMoves.Num());

            FEditorViewportClient* Client = (FEditorViewportClient*)InViewport->GetClient();
            for(int32 i = 0; i < CapturedMouseMoves.Num(); ++i)
            {
                FViewportCursorLocation MouseViewportRay(View,Client,CapturedMouseMoves[i].X,CapturedMouseMoves[i].Y);
                Rays.Emplace(TPair<FVector,FVector>(MouseViewportRay.GetOrigin(),MouseViewportRay.GetDirection()));
            }

            bPaintApplied = MeshPainter->Paint(InViewport,View->ViewMatrices.GetViewOrigin(),Rays);
        }
    }

    return bPaintApplied;
}



bool FOdysseyViewportDrawingEditorEdMode::IsEditingEnabled() const
{
	return GetWorld() ? GetWorld()->FeatureLevel >= ERHIFeatureLevel::SM5 : false;
}

void FOdysseyViewportDrawingEditorEdMode::Enter()
{
	FOdysseyViewportDrawingEditorPainter::Get()->GetController()->EdModeEnter();
	IMeshPaintEdMode::Enter();
}

void FOdysseyViewportDrawingEditorEdMode::Exit()
{
	FOdysseyViewportDrawingEditorPainter::Get()->GetController()->EdModeExit();
	IMeshPaintEdMode::Exit();
}
