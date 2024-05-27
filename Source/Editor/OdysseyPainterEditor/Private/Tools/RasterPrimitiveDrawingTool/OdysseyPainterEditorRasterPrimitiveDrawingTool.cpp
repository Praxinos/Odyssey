// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterPrimitiveDrawingTool/OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "FreehandShape/OdysseyFreehandShape.h"
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
    SelectedShape(EOdysseyShape::kFreehand)
{
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Shapes64");

    UOdysseyFreehandShape* freehandShape = CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterDrawingTool::FreehandShape");
    freehandShape->DisplayHUD(true);

    AvailableShapes.Add(EOdysseyShape::kFreehand, freehandShape);
    AvailableShapes.Add(EOdysseyShape::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::LineShape"));
    AvailableShapes.Add(EOdysseyShape::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::RectangleShape"));
    AvailableShapes.Add(EOdysseyShape::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::PolygonShape"));
    AvailableShapes.Add(EOdysseyShape::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::EllipseShape"));
    AvailableShapes.Add(EOdysseyShape::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::BezierShape"));

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

    shape->OnPathBeginDelegate().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathBegin);
    shape->OnPathToDelegate().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathTo);
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

void
UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathBegin(const FOdysseyPoint& iPoint)
{
    mPath.Empty();
    mPath.Add(iPoint);
}

void
UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathTo(const TArray<FOdysseyPoint>& iPoints)
{
    mPath.Append(iPoints);
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathEnd(const FOdysseyPoint& iPoint)
{
    mPath.Add(iPoint);

    GEditor->BeginTransaction(LOCTEXT("raster-primitive-drawing-tool.transaction.draw-shape", "Draw Primitive Shape"));

    /* FOdysseyShapeDrawOptions options;

    options.mColor = GetEditor()->PaintColor().GetValue();
    options.mFilled = Filled;
    options.mPrecision = Precision; */

    //SelectedShapeInstance->Draw(mPaintEngine.PaintBlock().Get(), options);


    //START Blend2D version

    int w = mPaintEngine.PaintBlock()->Width();
    int h = mPaintEngine.PaintBlock()->Height();
    ::ULIS::eFormat format = mPaintEngine.PaintBlock()->Format();

    TSharedPtr<::ULIS::FBlock> maskBlock = MakeShared<ULIS::FBlock>(w, h, ::ULIS::Format_BGRA8);

    ::ULIS::FContext& ulisCtx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    ulisCtx.Clear(*maskBlock);
    ulisCtx.Finish();

    BLImage img;
    int stride = maskBlock->BytesPerScanLine();
    img.createFromData(w, h, BL_FORMAT_PRGB32, (void*)maskBlock->Bits(), stride);
    BLContext blend2DCtx;
    BLContextCreateInfo createInfo{};
    createInfo.threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    blend2DCtx.begin(img, createInfo);

    bool isLine = SelectedShapeInstance == AvailableShapes[EOdysseyShape::kLine];
    bool isBezier = SelectedShapeInstance == AvailableShapes[EOdysseyShape::kBezier];

    if (mPath.Num() < 2)
        return;

    if ( isLine )
    {
        mPath = {
            mPath[0],
            mPath.Last()
        };
    }

    BLPath path;

    if ( SubPixel )
    {
        path.moveTo(mPath[0].x, mPath[0].y);
        for ( int i = 1; i < mPath.Num(); i++ )
        {
            path.lineTo(mPath[i].x, mPath[i].y);
        }
    }
    else
    {
        path.moveTo(FMath::Floor(mPath[0].x) + 0.5f, FMath::Floor(mPath[0].y) + 0.5f);
        for ( int i = 1; i < mPath.Num(); i++ )
        {
            path.lineTo(FMath::Floor(mPath[i].x) + 0.5f, FMath::Floor(mPath[i].y) + 0.5f);
        }
    }

    if ( !isLine && !isBezier || !isLine && Filled)
    {
        path.close();
    }

    const ::ULIS::FColor& ulisColor = GetEditor()->PaintColor().GetValue().ToFormat(::ULIS::Format_BGRA8);
    BLRgba32 blend2DColor(ulisColor.Red8(), ulisColor.Green8(), ulisColor.Blue8(), 255);
    int strokeWidth = 0;

    if ( Filled && !isLine )
    {
        blend2DCtx.setFillStyle(blend2DColor);
        blend2DCtx.fillPath(path);
    }
    else
    {
        strokeWidth = StrokeWidth;
        blend2DCtx.setStrokeStyle(BLRgba32(0, 0, 0, 255));
        blend2DCtx.setStrokeWidth(strokeWidth);
        blend2DCtx.strokePath(path);
    }
    blend2DCtx.end();

    BLBox bbox;
    path.getBoundingBox(&bbox);

    ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromMinMax(
        bbox.x0 - 1 - strokeWidth / 2,
        bbox.y0 - 1 - strokeWidth / 2,
        bbox.x1 + 1 + strokeWidth / 2,
        bbox.y1 + 1 + strokeWidth / 2
    ) & maskBlock->Rect();

    ulisCtx.Unpremultiply(*maskBlock, invalidRect );
    ulisCtx.Finish();

    if (!Antialiasing)
    {
        ulisCtx.FilterInPlace(
            []( ::ULIS::FPixel& iPixel, uint64 iNumPixels )
            {
                for (int i = 0; i < iNumPixels; i++, iPixel.Next())
                {
                    if ( iPixel.Alpha8() >= 127 )
                    {
                        iPixel.SetAlpha8(255);
                    }
                    else
                    {
                        iPixel.SetAlpha8(0);
                    }
                }
            }
            , *maskBlock
            , invalidRect
        );
        ulisCtx.Finish();
    }

    TSharedPtr<::ULIS::FBlock> dstBlock = MakeShared<ULIS::FBlock>(invalidRect.w, invalidRect.h, format);
    ulisCtx.ConvertFormat(*maskBlock, *dstBlock, invalidRect);
    ulisCtx.Finish();

    ulisCtx.Blend(*dstBlock, *mPaintEngine.PaintBlock(), dstBlock->Rect(), invalidRect.Position());
    ulisCtx.Finish();

    //END Blend2D version












    /* ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mPaintEngine.PaintBlock()->Format());

    if (SelectedShapeInstance == AvailableShapes[EOdysseyShape::kLine] && !Filled)
    {
        if (Precision == EOdysseyDrawingPrecision::kRaw)
            ctx.DrawLine(*mPaintEngine.PaintBlock(), ::ULIS::FVec2I(mPath[0].x, mPath[0].y), ::ULIS::FVec2I(mPath.Last().x, mPath.Last().y), GetEditor()->PaintColor().GetValue());
        else if (Precision == EOdysseyDrawingPrecision::kAA)
            ctx.DrawLineAA(*mPaintEngine.PaintBlock(), ::ULIS::FVec2I(mPath[0].x, mPath[0].y), ::ULIS::FVec2I(mPath.Last().x, mPath.Last().y), GetEditor()->PaintColor().GetValue());
        else if (Precision == EOdysseyDrawingPrecision::kSP)
            ctx.DrawLineSP(*mPaintEngine.PaintBlock(), ::ULIS::FVec2I(mPath[0].x, mPath[0].y), ::ULIS::FVec2I(mPath.Last().x, mPath.Last().y), GetEditor()->PaintColor().GetValue());
    }
    else if (SelectedShapeInstance == AvailableShapes[EOdysseyShape::kBezier] && !Filled)
    {
        UOdysseyBezierShape* bezierShape = Cast<UOdysseyBezierShape>(SelectedShapeInstance);
        if (Precision == EOdysseyDrawingPrecision::kRaw)
            ctx.DrawQuadraticBezier(*mPaintEngine.PaintBlock(), ::ULIS::FVec2I(bezierShape->GetStartPoint().X, bezierShape->GetStartPoint().Y), ::ULIS::FVec2I(bezierShape->GetControlPoint().X, bezierShape->GetControlPoint().Y), ::ULIS::FVec2I(bezierShape->GetEndPoint().X, bezierShape->GetEndPoint().Y), 1.f, GetEditor()->PaintColor().GetValue());
        else if (Precision == EOdysseyDrawingPrecision::kAA)
            ctx.DrawQuadraticBezierAA(*mPaintEngine.PaintBlock(), ::ULIS::FVec2I(bezierShape->GetStartPoint().X, bezierShape->GetStartPoint().Y), ::ULIS::FVec2I(bezierShape->GetControlPoint().X, bezierShape->GetControlPoint().Y), ::ULIS::FVec2I(bezierShape->GetEndPoint().X, bezierShape->GetEndPoint().Y), 1.f, GetEditor()->PaintColor().GetValue());
        else if (Precision == EOdysseyDrawingPrecision::kSP)
            ctx.DrawQuadraticBezierSP(*mPaintEngine.PaintBlock(), ::ULIS::FVec2I(bezierShape->GetStartPoint().X, bezierShape->GetStartPoint().Y), ::ULIS::FVec2I(bezierShape->GetControlPoint().X, bezierShape->GetControlPoint().Y), ::ULIS::FVec2I(bezierShape->GetEndPoint().X, bezierShape->GetEndPoint().Y), 1.f, GetEditor()->PaintColor().GetValue());
    }
    else
    {

        if (Precision == EOdysseyDrawingPrecision::kRaw)
        {
            std::vector< ::ULIS::FVec2I > points;
            for (int i = 0; i < mPath.Num(); i++)
            {
                points.push_back(::ULIS::FVec2I(mPath[i].x, mPath[i].y));
            }
            
            ctx.DrawPolygon(*mPaintEngine.PaintBlock(), points, GetEditor()->PaintColor().GetValue(), Filled );
        }
        else if (Precision == EOdysseyDrawingPrecision::kAA)
        {
            std::vector< ::ULIS::FVec2I > points;
            for (int i = 0; i < mPath.Num(); i++)
            {
                points.push_back(::ULIS::FVec2I(mPath[i].x, mPath[i].y));
            }

            ctx.DrawPolygonAA(*mPaintEngine.PaintBlock(), points, GetEditor()->PaintColor().GetValue(), Filled);
        }
        else if (Precision == EOdysseyDrawingPrecision::kSP)
        {
            std::vector< ::ULIS::FVec2F > points;
            for (int i = 0; i < mPath.Num(); i++)
            {
                points.push_back(::ULIS::FVec2F(mPath[i].x, mPath[i].y));
            }

            ctx.DrawPolygonSP(*mPaintEngine.PaintBlock(), points, GetEditor()->PaintColor().GetValue(), Filled);
        }
    }

    ctx.Finish(); */

    mPaintEngine.PaintBlock()->Dirty();
    mPaintEngine.Update(BlendParameters);

    Flush();
    Commit();
    
    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();

    GEditor->EndTransaction();
    mHUD->EmptyElements();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathAbort()
{
    mPaintEngine.Abort();

    mHUD->EmptyElements();
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapePathReset()
{
    mPaintEngine.Abort();

    mHUD->EmptyElements();
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
