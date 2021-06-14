// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EdMode.h"
#include "EditorModeManager.h"
#include "PhysicsEngine/PhysicsSettings.h"

#include "OdysseyViewportDrawingEditor.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "OdysseyViewportDrawingEditorPainter.h"

#define LOCTEXT_NAMESPACE "FOdysseyViewportDrawingEditorEdMode"

const FEditorModeID FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId = TEXT("EM_OdysseyViewportDrawingEditorEdMode");

void FOdysseyViewportDrawingEditorEdMode::Initialize()
{
    mEditor = MakeShareable(new FOdysseyViewportDrawingEditor());
	mToolkit = MakeShareable(new FOdysseyViewportDrawingEditorToolkit(mEditor, this));
	mEditor->Initialize(nullptr);
	mToolkit->Initialize();

    mViewportDrawingEditorPainter = new FOdysseyViewportDrawingEditorPainter(mEditor);
	MeshPainter = mViewportDrawingEditorPainter;
}

TSharedPtr<class FModeToolkit> FOdysseyViewportDrawingEditorEdMode::GetToolkit()
{
    return mToolkit;
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
    if( !UPhysicsSettings::Get()->bSupportUVFromHitResults )
    {
        FText Title = LOCTEXT("TitleCollisionUVNoSupport","CollisionUVNoSupport");
        FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("Enable FindCollisionUV","'Support UV From Hit Results' doesn't seem to be enabled. Enable it from project settings in order to use this paint editor properly."),&Title);
    }

    mViewportDrawingEditorPainter->Initialize();
    //mViewportDrawingEditorPainter->GetController()->EdModeEnter();
	IMeshPaintEdMode::Enter();
}

void FOdysseyViewportDrawingEditorEdMode::Exit()
{
    // mViewportDrawingEditorPainter->GetController()->EdModeExit();
    // virer les tabs restantes.
    // UnregisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager)
    mViewportDrawingEditorPainter->Finalize();
	IMeshPaintEdMode::Exit();
}

#undef LOCTEXT_NAMESPACE
