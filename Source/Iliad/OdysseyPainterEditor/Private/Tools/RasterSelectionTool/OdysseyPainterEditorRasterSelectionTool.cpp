// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "OdysseyMediaRaster.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDElement.h"
#include "Input/OdysseyPoint.h"

#include "FreehandShape/OdysseyFreehandShape.h"
#include "RectangleShape/OdysseyRectangleShape.h"
#include "PolygonShape/OdysseyPolygonShape.h"
#include "EllipseShape/OdysseyEllipseShape.h"
#include "BezierShape/OdysseyBezierShape.h"
#include "PainterEditor/OdysseyPainterEditorViewportTab.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorRasterSelectionTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterSelectionTool::~UOdysseyPainterEditorRasterSelectionTool()
{
}

UOdysseyPainterEditorRasterSelectionTool::UOdysseyPainterEditorRasterSelectionTool()
    : SelectedShape( EOdysseyFillShape::kFreehand )
    , mSelectionState( EOdysseySelectionState::Normal )
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
{
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Lasso32");

    UOdysseyFreehandShape* freehandShape = CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterSelectionTool::FreehandShape");
    freehandShape->DisplayHUD(true);

    AvailableShapes.Add(EOdysseyFillShape::kFreehand, freehandShape);
    AvailableShapes.Add(EOdysseyFillShape::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterSelectionTool::RectangleShape"));
    AvailableShapes.Add(EOdysseyFillShape::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterSelectionTool::PolygonShape"));
    AvailableShapes.Add(EOdysseyFillShape::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterSelectionTool::EllipseShape"));
    AvailableShapes.Add(EOdysseyFillShape::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterSelectionTool::BezierShape"));

    SelectedShapeInstance = AvailableShapes[SelectedShape];
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
    return SelectedShapeInstance->OnMouseDown(point, iKey);
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyPoint point = iPointInTexture;
    SelectedShapeInstance->OnMouseHover( point );
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyPoint point = iPointInTexture;
    SelectedShapeInstance->OnMouseDrag(point);
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyPoint point = iPointInTexture;
    return SelectedShapeInstance->OnMouseUp(point, iKey);
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
    return SelectedShapeInstance->OnKeyDown(iKey);
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
    return SelectedShapeInstance->OnKeyUp(iKey);
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
    SelectedShapeInstance->Tick(iDeltaTime);
}

void UOdysseyPainterEditorRasterSelectionTool::SelectedShapeChanged()
{
    SelectedShapeInstance->Abort();
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterSelectionTool, SelectedShapeInstance), AvailableShapes[SelectedShape]);
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

void UOdysseyPainterEditorRasterSelectionTool::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterSelectionTool, SelectedShape))
        SelectedShapeChanged();
}

#undef LOCTEXT_NAMESPACE
