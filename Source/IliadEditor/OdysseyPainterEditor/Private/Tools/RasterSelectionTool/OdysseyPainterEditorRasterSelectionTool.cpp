// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"

#include "Input/OdysseyPoint.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDElement.h"

#include "OdysseyFreehandShape.h"
#include "OdysseyRectangleShape.h"
#include "OdysseyPolygonShape.h"
#include "OdysseyEllipseShape.h"
#include "OdysseyBezierShape.h"
#include "OdysseyPainterEditorViewportTab.h"

#include "ScopedTransaction.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorRasterSelectionTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterSelectionTool::~UOdysseyPainterEditorRasterSelectionTool()
{
}

UOdysseyPainterEditorRasterSelectionTool::UOdysseyPainterEditorRasterSelectionTool()
    : mSelectionState( EOdysseySelectionState::Normal )
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Lasso64"));

    mHUD->AddElement(mShapeHUD);

    UOdysseyFreehandShape* freehandShape = CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterSelectionTool::FreehandShape");
    freehandShape->DisplayHUD(true);

    Shapes.AddShapeType(EOdysseyShapeType::kFreehand, freehandShape);
    Shapes.AddShapeType(EOdysseyShapeType::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterDrawingTool::RectangleShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterDrawingTool::PolygonShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterDrawingTool::EllipseShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterDrawingTool::BezierShape"));

    Shapes.SetActiveShapeType(EOdysseyShapeType::kFreehand);
}

template<class T>
T*
UOdysseyPainterEditorRasterSelectionTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnBegin().AddUObject(this, &UOdysseyPainterEditorRasterSelectionTool::OnShapeBegin);
    shape->OnCommit().AddUObject(this, &UOdysseyPainterEditorRasterSelectionTool::OnShapeCommit);
    shape->OnAbort().AddUObject(this, &UOdysseyPainterEditorRasterSelectionTool::OnShapeAbort);

    shape->SetHUD(mShapeHUD);

    return shape;
}

bool UOdysseyPainterEditorRasterSelectionTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

FText
UOdysseyPainterEditorRasterSelectionTool::GetTooltip() const
{
    return LOCTEXT( "raster-selection-tool.tooltip", "Selection Tool" );
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    UOdysseyPainterEditorTool::OnMouseDown(iPointInTexture, iKey);

    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyPoint point = iPointInTexture;
    return Shapes.GetActiveShape()->OnMouseDown(point, iKey);
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyPoint point = iPointInTexture;
    Shapes.GetActiveShape()->OnMouseHover( point );
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    FOdysseyPoint point = iPointInTexture;
    Shapes.GetActiveShape()->OnMouseDrag(point);
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyPoint point = iPointInTexture;
    return Shapes.GetActiveShape()->OnMouseUp(point, iKey);
}

bool UOdysseyPainterEditorRasterSelectionTool::OnKeyDown(const FKey& iKey)
{
    if ( iKey == EKeys::LeftControl || iKey == EKeys::RightControl )
    {
        mSelectionState = EOdysseySelectionState::Add;
        return true;
    }
    else if ( iKey == EKeys::LeftAlt || iKey == EKeys::RightAlt )
    {
        mSelectionState = EOdysseySelectionState::Substract;
        return true;
    }
    return Shapes.GetActiveShape()->OnKeyDown(iKey);
}

bool UOdysseyPainterEditorRasterSelectionTool::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftControl || iKey == EKeys::RightControl)
    {
        mSelectionState = EOdysseySelectionState::Normal;
        return true;
    }
    else if (iKey == EKeys::LeftAlt || iKey == EKeys::RightAlt)
    {
        mSelectionState = EOdysseySelectionState::Normal;
        return true;
    }
    return Shapes.GetActiveShape()->OnKeyUp(iKey);
}

void UOdysseyPainterEditorRasterSelectionTool::Load()
{
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterSelectionTool::Unload()
{
    Shapes.GetActiveShape()->Abort();
    UOdysseyPainterEditorTool::Unload();
}

void
UOdysseyPainterEditorRasterSelectionTool::Deselect()
{
    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->Clear();
}

TOptional<FMouseCursor>
UOdysseyPainterEditorRasterSelectionTool::GetMouseCursorOverride() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return FMouseCursor(EMouseCursor::SlashedCircle);

    return Super::GetMouseCursorOverride();
}

void UOdysseyPainterEditorRasterSelectionTool::Tick(float iDeltaTime)
{
    //Tick the shape
    Shapes.GetActiveShape()->Tick(iDeltaTime);
}

void
UOdysseyPainterEditorRasterSelectionTool::OnShapeBegin()
{
}

void
UOdysseyPainterEditorRasterSelectionTool::OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset)
{
    TArray<FVector2D> points(iPoints);
    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (mSelectionState == EOdysseySelectionState::Add) //Add selection to existing one
    {
        FScopedTransaction scopedTransaction(LOCTEXT("actions.raster.selection.add", "Add Raster Selection"));
        rasterSelection->Add(points);
    }
    else if (mSelectionState == EOdysseySelectionState::Substract) //Remove selection to existing one
    {
        FScopedTransaction scopedTransaction(LOCTEXT("actions.raster.selection.substract", "Substract Raster Selection"));
        rasterSelection->Substract(points);
    }
    else //Normal, we replace the selection
    {
        FScopedTransaction scopedTransaction(LOCTEXT("actions.raster.selection", "Raster Selection"));
        rasterSelection->Clear();
        rasterSelection->Add(points);
    }
}

void
UOdysseyPainterEditorRasterSelectionTool::OnShapeAbort()
{
}

#undef LOCTEXT_NAMESPACE
