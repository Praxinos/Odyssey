// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyHUD.h"
#include "SOdysseyViewport.h"
#include "FOdysseySceneViewport.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportClient.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "Toolkits/BaseToolkit.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "PaperFlipbook.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorViewportTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_Viewport"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

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
    : FOdysseyEditorTab(LOCTEXT( "viewport-tab.name", "Viewport" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ))
    , mEditor(iEditor)
    , mViewport(nullptr)
    , mViewportClient(nullptr)
{
    SetDefaultTexture();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorViewportTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorViewportTab::CreateWidget()
{
    SAssignNew(mViewport, SOdysseyViewport)
        .Texture(this, &FOdysseyPainterEditorViewportTab::Texture);

    //TODO: not cool to have to go through the whole GUI for an info, move that in the painterEditor Data
    mViewportClient = MakeShareable(new FOdysseyPainterEditorViewportClient(mEditor, mViewport, mEditor->GetMeshSelector().Get()));

    //TODO: manage colorpicking here, viewportClient itself should not know the action to pick a color
    mViewportClient->OnPickColor().BindRaw(this, &FOdysseyPainterEditorViewportTab::HandleViewportColorPicked);
    mViewportClient->OnMouseDown().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDown);
    mViewportClient->OnMouseClick().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseClick);
    mViewportClient->OnMouseDoubleClick().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDoubleClick);
    mViewportClient->OnMouseUp().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseUp);
    mViewportClient->OnMouseHover().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseHover);
    mViewportClient->OnMouseDrag().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportMouseDrag);
    mViewportClient->OnKeyDown().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportKeyDown);
    mViewportClient->OnKeyUp().BindRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportKeyUp);

    mViewport->SetViewportClient(mViewportClient);
    //mViewport->GetViewport()->ViewportResizedEvent.AddRaw(this, &FOdysseyPainterEditorViewportTab::OnViewportSizeChanged);

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
    MAP_ACTION(painterEditorCommands.FlipViewportHorizontally, OnFlipViewportHorizontally)
    MAP_ACTION(painterEditorCommands.FlipViewportVertically, OnFlipViewportVertically)
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

TSharedPtr<SOdysseyViewport>
FOdysseyPainterEditorViewportTab::GetViewport()
{
    return mViewport;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Setters

void
FOdysseyPainterEditorViewportTab::SetDefaultTexture()
{
    mTexture = TAttribute<UTexture*>::CreateLambda(
        [this]() -> UTexture*
        {
            if (!mEditor->GetSource())
                return nullptr;

            return mEditor->GetSource()->DisplayTexture();
        }
    );
}

void
FOdysseyPainterEditorViewportTab::SetTexture(const TAttribute<UTexture*>& iTexture )
{
    mTexture = iTexture;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UTexture*
FOdysseyPainterEditorViewportTab::Texture() const
{
    UObject* editedObject = mEditor->GetEditedObject();
    if (!editedObject || !editedObject->IsA<UPaperFlipbook>())
    {
        TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
        if (!source)
            return nullptr;

        return source->DisplayTexture();
    }

    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(editedObject);

    TSharedPtr<FOdysseyPainterEditorFlipbookTimelineTab> timelineTab = mEditor->FindTab<FOdysseyPainterEditorFlipbookTimelineTab>();
    if (timelineTab->Timeline() && timelineTab->Timeline()->IsScrubbing())
    {
        int32 index = timelineTab->Timeline()->GetCurrentKeyframeIndex();
        UTexture2D* texture = OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(flipbook, index);
        return texture;
    }

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source)
        return nullptr;

    return source->DisplayTexture();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorViewportTab::HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture)
{
    if (iEventState == eOdysseyEventState::kSet)
        mEditor->GetColorPickerTool()->PickColorUp(FOdysseyPoint(iPositionInTexture.X, iPositionInTexture.Y));
    else
        mEditor->GetColorPickerTool()->PickColorMove(FOdysseyPoint(iPositionInTexture.X, iPositionInTexture.Y));
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->OnMouseDown(iPointInTexture, iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->OnMouseClick(iPointInTexture, iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->OnMouseDoubleClick(iPointInTexture, iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    return tool->OnMouseUp(iPointInTexture, iKey);
}

void
FOdysseyPainterEditorViewportTab::OnViewportMouseHover(const FOdysseyPoint& iPointInTexture)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return;

    tool->OnMouseHover(iPointInTexture);
}

void
FOdysseyPainterEditorViewportTab::OnViewportMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return;

    tool->OnMouseDrag(iPointInTexture);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportKeyDown(const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if ( !tool )
        return false;

    return tool->OnKeyDown(iKey);
}

bool
FOdysseyPainterEditorViewportTab::OnViewportKeyUp(const FKey& iKey)
{
    //mEditor->GetCurrentTool()->SetTransform(mViewport->GetTransformToSourceTexture());
    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if ( !tool )
        return false;

    return tool->OnKeyUp(iKey);
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

void FOdysseyPainterEditorViewportTab::OnFlipViewportHorizontally()
{
    mViewport->FlipHorizontal();
}

void FOdysseyPainterEditorViewportTab::OnFlipViewportVertically()
{
    mViewport->FlipVertical();
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

/* void FOdysseyPainterEditorViewportTab::OnViewportSizeChanged(FViewport* iViewport, uint32 iUnused)
{
    if ( iViewport == mViewport->GetViewport().Get() )
    {
        mEditor->HUDSystem()->RebuildHUDSurface( FVector2D( iViewport->GetSizeXY() ));
        mEditor->PersistentHUDSystem()->RebuildHUDSurface(FVector2D(iViewport->GetSizeXY()));
    }
} */

#undef LOCTEXT_NAMESPACE
