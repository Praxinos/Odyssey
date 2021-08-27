// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorViewportTab.h"

#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportTab::~FOdysseyPainterEditorViewportTab()
{
}

FOdysseyPainterEditorViewportTab::FOdysseyPainterEditorViewportTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_Viewport"),
                            LOCTEXT( "OdysseyPainterEditorViewportTab", "Viewport" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ))
    , mEditor(iEditor)
    , mViewport(nullptr)
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
	TSharedPtr<FOdysseyPainterEditorViewportClient> viewportClient = MakeShareable(new FOdysseyPainterEditorViewportClient(mEditor, mViewport, mEditor->GetGUI()->GetMeshSelectorTab()->MeshSelector()->GetMeshSelectorPtr()));
	viewportClient->OnPickColor().AddRaw(this, &FOdysseyPainterEditorViewportTab::HandleViewportColorPicked);
	mViewport->SetViewportClient(viewportClient);

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

    ::ul3::FBlock* block = mEditor->DisplaySurface()->Block()->GetBlock();
    if (iPositionInTexture.X >= 0 && iPositionInTexture.X < block->Width() &&
        iPositionInTexture.Y >= 0 && iPositionInTexture.Y < block->Height())
    {
        const ::ul3::FPixelValue& color = block->PixelValue(iPositionInTexture.X, iPositionInTexture.Y);
        mEditor->PaintColor(color);
    }
        
    if (iEventState == eOdysseyEventState::kSet)
    {
        mEditor->PaintEngine()->SetColor(mEditor->PaintColor());
    }
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

#undef LOCTEXT_NAMESPACE
