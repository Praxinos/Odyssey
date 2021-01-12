// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

bool FOdysseyViewportDrawingEditorEdMode::MouseMove(FEditorViewportClient* iViewportClient,FViewport* iViewport,int32 iX,int32 iY)
{
    //If we draw by using the stylus, we ignore the mouse events here
    FOdysseyViewportDrawingEditorPainter* painter = (FOdysseyViewportDrawingEditorPainter*)MeshPainter;
    if(painter->IsCapturedByStylus())
        return true;

    // We only care about perspective viewpo1rts
    bool bPaintApplied = false;

    if(iViewportClient->IsPerspective())
    {
            if(MeshPainter->IsPainting() )
            {
                // Compute a world space ray from the screen space mouse coordinates
                FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
                    iViewportClient->Viewport,
                    iViewportClient->GetScene(),
                    iViewportClient->EngineShowFlags)
                    .SetRealtimeUpdate(iViewportClient->IsRealtime()));
                FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);


                FEditorViewportClient* client = (FEditorViewportClient*)iViewport->GetClient();
                FViewportCursorLocation MouseViewportRay(view,client,iX,iY);

                bPaintApplied = MeshPainter->Paint(iViewport, view->ViewMatrices.GetViewOrigin(), MouseViewportRay.GetOrigin(), MouseViewportRay.GetDirection());
            }
    }

    return bPaintApplied;
}

bool FOdysseyViewportDrawingEditorEdMode::ProcessCapturedMouseMoves(FEditorViewportClient* InViewportClient,FViewport* InViewport,const TArrayView<FIntPoint>& CapturedMouseMoves)
{
    return true;
}



bool FOdysseyViewportDrawingEditorEdMode::IsEditingEnabled() const
{
	return GetWorld() ? GetWorld()->FeatureLevel >= ERHIFeatureLevel::SM5 : false;
}

void FOdysseyViewportDrawingEditorEdMode::Enter()
{
    FOdysseyViewportDrawingEditorPainter::Get()->Initialize();
	FOdysseyViewportDrawingEditorPainter::Get()->GetController()->EdModeEnter();
	IMeshPaintEdMode::Enter();
}

void FOdysseyViewportDrawingEditorEdMode::Exit()
{
    FOdysseyViewportDrawingEditorPainter::Get()->GetController()->EdModeExit();
    FOdysseyViewportDrawingEditorPainter::Get()->Finalize();
	IMeshPaintEdMode::Exit();
}
