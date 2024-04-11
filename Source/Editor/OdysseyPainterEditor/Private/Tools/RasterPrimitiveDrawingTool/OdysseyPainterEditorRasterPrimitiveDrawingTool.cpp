// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterPrimitiveDrawingTool/OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "LineShape/OdysseyLineShape.h"
#include "RectangleShape/OdysseyRectangleShape.h"
#include "PolygonShape/OdysseyPolygonShape.h"
#include "EllipseShape/OdysseyEllipseShape.h"
#include "BezierShape/OdysseyBezierShape.h"

#include "UObject/OdysseyObjectEditorUtils.h"

#include "OdysseyMediaRaster.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyHUDHandle.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterPrimitiveDrawingTool::~UOdysseyPainterEditorRasterPrimitiveDrawingTool()
{
}

UOdysseyPainterEditorRasterPrimitiveDrawingTool::UOdysseyPainterEditorRasterPrimitiveDrawingTool() :
    mPaintEngine(),
    SelectedShape(EOdysseyPrimitiveShape::kLine)
{
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Shapes64");

    AvailableShapes.Add(EOdysseyPrimitiveShape::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::LineShape"));
    AvailableShapes.Add(EOdysseyPrimitiveShape::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::RectangleShape"));
    AvailableShapes.Add(EOdysseyPrimitiveShape::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::PolygonShape"));
    AvailableShapes.Add(EOdysseyPrimitiveShape::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::EllipseShape"));
    AvailableShapes.Add(EOdysseyPrimitiveShape::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::BezierShape"));

    for (const auto& Shape : AvailableShapes)
    {
        Shape.Value->IsPrimitive = true;
    }

    SelectedShapeInstance = AvailableShapes[SelectedShape];
}

template<class T>
T*
UOdysseyPainterEditorRasterPrimitiveDrawingTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnPathEndDelegate().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathEnd);
    shape->OnPathAbortDelegate().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathAbort);
    shape->OnPathResetDelegate().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathReset);

    shape->SetHUD(mHUD);

    return shape;
}

bool
UOdysseyPainterEditorRasterPrimitiveDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    mPaintEngine.RasterBlock(rasterBlock);

    return SelectedShapeInstance->OnMouseDown(iPointInTexture, iKey);
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;

    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    SelectedShapeInstance->OnMouseHover( iPointInTexture );
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;

    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    SelectedShapeInstance->OnMouseDrag(iPointInTexture);
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;

    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return false;

    return SelectedShapeInstance->OnMouseUp(iPointInTexture, iKey);
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnKeyDown(const FKey& iKey)
{
    return SelectedShapeInstance->OnKeyDown(iKey);
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnKeyUp(const FKey& iKey)
{
    return SelectedShapeInstance->OnKeyUp(iKey);
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::Load()
{
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::Unload()
{
    UOdysseyPainterEditorTool::Unload();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::Flush()
{

}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::Commit()
{
    mPaintEngine.Commit(BlendParameters);
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::Tick(float iDeltaTime)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;

    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    //Tick the shape
    SelectedShapeInstance->Tick(iDeltaTime);

    //Update the paintEngine
    mPaintEngine.Update(BlendParameters);
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::SelectedShapeChanged()
{
    SelectedShapeInstance->AbortShape();
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedShapeInstance", AvailableShapes[SelectedShape]);
    mOnShapeChanged.Broadcast();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathEnd(const FOdysseyPoint& iPoint)
{
    GEditor->BeginTransaction(LOCTEXT("raster-primitive-drawing-tool.transaction.draw-shape", "Draw Primitive Shape"));

    FOdysseyShapeDrawOptions options;

    options.mColor = GetEditor()->PaintColor().GetValue();
    options.mFilled = Filled;
    options.mPrecision = Precision;

    SelectedShapeInstance->Draw(mPaintEngine.PaintBlock().Get(), options);
    mPaintEngine.PaintBlock()->Dirty();
    mPaintEngine.Update(BlendParameters);

    Flush();
    Commit();
    
    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();

    GEditor->EndTransaction();
    mHUD->EmptyHUDElements();
    mEditor->HUDSystem()->ClearHUDSurface();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathAbort()
{
    mPaintEngine.Abort();

    mHUD->EmptyHUDElements();
    mEditor->HUDSystem()->ClearHUDSurface();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathReset()
{
    mPaintEngine.Abort();

    mHUD->EmptyHUDElements();
    mEditor->HUDSystem()->ClearHUDSurface();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == "SelectedShape")
        SelectedShapeChanged();
}

FSimpleMulticastDelegate& UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeChanged()
{
    return mOnShapeChanged;
}

EMouseCursor::Type
UOdysseyPainterEditorRasterPrimitiveDrawingTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

    return UOdysseyPainterEditorTool::GetMouseCursor();
}

FText
UOdysseyPainterEditorRasterPrimitiveDrawingTool::GetTooltip() const
{
    return LOCTEXT("raster-primitive-drawing-tool.tooltip", "Primitive Drawing Tool");
}

#undef LOCTEXT_NAMESPACE
