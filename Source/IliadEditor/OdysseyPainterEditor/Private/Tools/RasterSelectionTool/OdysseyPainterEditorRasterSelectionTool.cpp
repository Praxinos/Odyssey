// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDElement.h"
#include "Input/OdysseyPoint.h"

#include "FreehandShape/OdysseyFreehandShape.h"
#include "RectangleShape/OdysseyRectangleShape.h"
#include "PolygonShape/OdysseyPolygonShape.h"
#include "EllipseShape/OdysseyEllipseShape.h"
#include "BezierShape/OdysseyBezierShape.h"
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
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Lasso32");

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

    shape->OnCommit().AddUObject(this, &UOdysseyPainterEditorRasterSelectionTool::OnShapeCommit);

    shape->SetHUD(mShapeHUD);

    return shape;
}

bool UOdysseyPainterEditorRasterSelectionTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
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
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    mHUD->RemoveElement(rasterSelection->GetHUD());
    mHUD->RemoveElement(mShapeHUD);

    UOdysseyPainterEditorTool::Unload();
}

void UOdysseyPainterEditorRasterSelectionTool::Tick(float iDeltaTime)
{
    //Tick the shape
    Shapes.GetActiveShape()->Tick(iDeltaTime);
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

#undef LOCTEXT_NAMESPACE
