// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorViewportTab.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyBrushOptions.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportTab::~FOdysseyPainterEditorViewportTab()
{
    if (mViewport)
        mViewport->GetViewport()->ViewportResizedEvent.RemoveAll(this);
}

FOdysseyPainterEditorViewportTab::FOdysseyPainterEditorViewportTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_Viewport"),
                            LOCTEXT( "OdysseyPainterEditorViewportTab", "Viewport" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ))
    , mEditor(iEditor)
    , mViewport(nullptr)
    , mViewportClient(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorViewportTab::CreateWidget()
{
    SAssignNew(mViewport, SOdysseySurfaceViewport)
        .Surface_Raw(this, &FOdysseyPainterEditorViewportTab::Surface);

    //TODO: not cool to have to go through the whole GUI for an info, move that in the painterEditor Data
	mViewportClient = MakeShareable(new FOdysseyPainterEditorViewportClient(mEditor, mViewport, mEditor->GetGUI()->GetMeshSelectorTab()->MeshSelector()->GetMeshSelectorPtr()));
	
    //TODO: manage colorpicking here, viewportClient itself should not know the action to pick a color
    mViewportClient->OnPickColor().BindRaw(this, &FOdysseyPainterEditorViewportTab::HandleViewportColorPicked);
    mViewportClient->OnMouseDown().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDown);
    mViewportClient->OnMouseUp().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseUp);
    mViewportClient->OnMouseHover().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseHover);
    mViewportClient->OnMouseDrag().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDrag);
    mViewportClient->OnKeyDown().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportKeyDown);
    mViewportClient->OnKeyUp().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportKeyUp);

	mViewport->SetViewportClient(mViewportClient);
    mViewport->GetViewport()->ViewportResizedEvent.AddRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportSizeChanged);

    return mViewport;
}

void
FOdysseyPainterEditorViewportTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorViewportTab::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorCommands.ResetViewportPosition, OnResetViewportPosition )
	MAP_ACTION(painterEditorCommands.ResetViewportRotation, OnResetViewportRotation )
	MAP_ACTION(painterEditorCommands.RotateViewportLeft, OnRotateViewportLeft )
	MAP_ACTION(painterEditorCommands.RotateViewportRight, OnRotateViewportRight )
	MAP_ACTION(painterEditorCommands.SetZoom10Percent, OnSetZoom, 0.1 )
	MAP_ACTION(painterEditorCommands.SetZoom20Percent, OnSetZoom, 0.2 )
	MAP_ACTION(painterEditorCommands.SetZoom30Percent, OnSetZoom, 0.3 )
	MAP_ACTION(painterEditorCommands.SetZoom40Percent, OnSetZoom, 0.4 )
	MAP_ACTION(painterEditorCommands.SetZoom50Percent, OnSetZoom, 0.5 )
	MAP_ACTION(painterEditorCommands.SetZoom60Percent, OnSetZoom, 0.6 )
	MAP_ACTION(painterEditorCommands.SetZoom70Percent, OnSetZoom, 0.7 )
	MAP_ACTION(painterEditorCommands.SetZoom80Percent, OnSetZoom, 0.8 )
	MAP_ACTION(painterEditorCommands.SetZoom90Percent, OnSetZoom, 0.9 )
	MAP_ACTION(painterEditorCommands.SetZoom100Percent, OnSetZoom, 1.0 )
	MAP_ACTION(painterEditorCommands.SetZoomFitScreen, OnSetZoomFitScreen )
	MAP_ACTION(painterEditorCommands.ZoomInExponential, OnZoomInExponential )
	MAP_ACTION(painterEditorCommands.ZoomOutExponential, OnZoomOutExponential )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseySurfaceViewport>
FOdysseyPainterEditorViewportTab::GetViewport()
{
    return mViewport;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

IOdysseySurface*
FOdysseyPainterEditorViewportTab::Surface() const
{
    return mEditor->DisplaySurface();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorViewportTab::HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture)
{
	if (!mEditor->DisplaySurface())
		return;

    ::ULIS::FBlock* block = mEditor->DisplaySurface()->Block();
    if (iPositionInTexture.X >= 0 && iPositionInTexture.X < block->Width() &&
        iPositionInTexture.Y >= 0 && iPositionInTexture.Y < block->Height())
    {
        const ::ULIS::FColor& color = block->Color(iPositionInTexture.X, iPositionInTexture.Y);
        mEditor->PaintColor( color );
        //TriggerStateChanged
    }
        
    if (iEventState == eOdysseyEventState::kSet)
    {
        //TriggerStateChanged
        //PATCH: should be automatic in the new drawing Tool, fix it asap

        UOdysseyDrawingTool* drawingTool = Cast<UOdysseyDrawingTool>(mEditor->GetSelectedTool());
        if (!drawingTool)
            return;

        FObjectEditorUtils::SetPropertyValue(drawingTool->GetBrushOptions(), "Color", FOdysseyBrushColor(mEditor->PaintColor()));
    }
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mEditor->GetSelectedTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    return mEditor->GetSelectedTool()->OnMouseDown(iPointInTexture, iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mEditor->GetSelectedTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    return mEditor->GetSelectedTool()->OnMouseUp(iPointInTexture, iKey);
}

void
FOdysseyPainterEditorViewportTab::OnViewportMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mEditor->GetSelectedTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    mEditor->GetSelectedTool()->OnMouseHover(iPointInTexture);
}

void
FOdysseyPainterEditorViewportTab::OnViewportMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mEditor->GetSelectedTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    mEditor->GetSelectedTool()->OnMouseDrag(iPointInTexture);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportKeyDown(const FKey& iKey)
{
    mEditor->GetSelectedTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    return mEditor->GetSelectedTool()->OnKeyDown(iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportKeyUp(const FKey& iKey)
{
    mEditor->GetSelectedTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    return mEditor->GetSelectedTool()->OnKeyUp(iKey);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Shortcuts

void
FOdysseyPainterEditorViewportTab::OnResetViewportPosition()
{
    mViewport->SetRotation( 0 );
    mViewport->ResetPan();
}

void
FOdysseyPainterEditorViewportTab::OnResetViewportRotation()
{
    mViewport->SetRotation( 0 );
}

void
FOdysseyPainterEditorViewportTab::OnRotateViewportLeft()
{
    mViewport->RotateLeft();
}

void
FOdysseyPainterEditorViewportTab::OnRotateViewportRight()
{
    mViewport->RotateRight();
}

void
FOdysseyPainterEditorViewportTab::OnSetZoom(double iZoomValue)
{
    mViewport->SetZoom(iZoomValue, mViewport->GetViewportCenter());
}

void
FOdysseyPainterEditorViewportTab::OnSetZoomFitScreen()
{
    mViewport->ToggleFitToViewport();
}

void
FOdysseyPainterEditorViewportTab::OnZoomInExponential()
{
    mViewport->ZoomExponential(mViewport->GetZoom(), 0.1);
}

void
FOdysseyPainterEditorViewportTab::OnZoomOutExponential()
{
    mViewport->ZoomExponential(mViewport->GetZoom(), -0.1);
}

void FOdysseyPainterEditorViewportTab::OnViewportSizeChanged(FViewport* iViewport, uint32 iUnused)
{
    mEditor->HUDSystem()->RefreshHUDSurface( FVector2D( iViewport->GetSizeXY() ));
}

#undef LOCTEXT_NAMESPACE
