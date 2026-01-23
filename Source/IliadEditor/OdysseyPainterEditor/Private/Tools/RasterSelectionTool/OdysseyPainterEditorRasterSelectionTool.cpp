// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDElement.h"
#include "Input/OdysseyPoint.h"

#include "OdysseyFreehandShape.h"
#include "OdysseyRectangleShape.h"
#include "OdysseyPolygonShape.h"
#include "OdysseyEllipseShape.h"
#include "OdysseyBezierShape.h"
#include "OdysseyPainterEditorViewportTab.h"

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
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Lasso64");

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

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
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
    FOdysseyPoint point = iPointInTexture;
    Shapes.GetActiveShape()->OnMouseDrag(point);
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
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

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    mHUD->AddElement(rasterSelection->GetHUD());
    mHUD->AddElement(mShapeHUD);
}

void UOdysseyPainterEditorRasterSelectionTool::Unload()
{
    Shapes.GetActiveShape()->Abort();

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    mHUD->RemoveElement(rasterSelection->GetHUD());
    mHUD->RemoveElement(mShapeHUD);

    UOdysseyPainterEditorTool::Unload();
}

void
UOdysseyPainterEditorRasterSelectionTool::Deselect()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->Clear();
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
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (mSelectionState == EOdysseySelectionState::Add) //Add selection to existing one
    {
        rasterSelection->Add(points);
    }
    else if (mSelectionState == EOdysseySelectionState::Substract) //Remove selection to existing one
    {
        rasterSelection->Substract(points);
    }
    else //Normal, we replace the selection
    {
        rasterSelection->Clear();
        rasterSelection->Add(points);
    }
}

void
UOdysseyPainterEditorRasterSelectionTool::OnShapeAbort()
{
}

#undef LOCTEXT_NAMESPACE
