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

#include "OdysseyMediaRaster.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyHUDHandle.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterPrimitiveDrawingTool::~UOdysseyPainterEditorRasterPrimitiveDrawingTool()
{
}

UOdysseyPainterEditorRasterPrimitiveDrawingTool::UOdysseyPainterEditorRasterPrimitiveDrawingTool()
    : mPaintEngine()
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
    , SelectedShape(EOdysseyShape::kFreehand)
{
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Shapes64");

    UOdysseyFreehandShape* freehandShape = CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::FreehandShape");
    freehandShape->DisplayHUD(true);

    AvailableShapes.Add(EOdysseyShape::kFreehand, freehandShape);
    AvailableShapes.Add(EOdysseyShape::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::LineShape"));
    AvailableShapes.Add(EOdysseyShape::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::RectangleShape"));
    AvailableShapes.Add(EOdysseyShape::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::PolygonShape"));
    AvailableShapes.Add(EOdysseyShape::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::EllipseShape"));
    AvailableShapes.Add(EOdysseyShape::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::BezierShape"));

    /* for (const auto& Shape : AvailableShapes)
    {
        Shape.Value->IsPrimitive = true;
    } */

    SelectedShapeInstance = AvailableShapes[SelectedShape];
}

template<class T>
T*
UOdysseyPainterEditorRasterPrimitiveDrawingTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnCommit().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeCommit);

    shape->SetHUD(mShapeHUD);

    return shape;
}

bool
UOdysseyPainterEditorRasterPrimitiveDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (UOdysseyPainterEditorRasterBaseTool::OnMouseDown(iPointInTexture, iKey))
        return true;

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

    FOdysseyPoint point = iPointInTexture;
    if (!SubPixel)
    {
        point.x = FMath::Floor(point.x);
        point.y = FMath::Floor(point.y);
        if (!Filled)
        {
            point.x += 0.5f;
            point.y += 0.5f;
        }
    }

    return SelectedShapeInstance->OnMouseDown(point, iKey);
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

    FOdysseyPoint point = iPointInTexture;
    if (!SubPixel)
    {
        point.x = FMath::Floor(point.x);
        point.y = FMath::Floor(point.y);
        if (!Filled)
        {
            point.x += 0.5f;
            point.y += 0.5f;
        }
    }

    SelectedShapeInstance->OnMouseHover( point );
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

    FOdysseyPoint point = iPointInTexture;
    if (!SubPixel)
    {
        point.x = FMath::Floor(point.x);
        point.y = FMath::Floor(point.y);
        if (!Filled)
        {
            point.x += 0.5f;
            point.y += 0.5f;
        }
    }

    SelectedShapeInstance->OnMouseDrag(point);
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

    FOdysseyPoint point = iPointInTexture;
    if (!SubPixel)
    {
        point.x = FMath::Floor(point.x);
        point.y = FMath::Floor(point.y);
        if (!Filled)
        {
            point.x += 0.5f;
            point.y += 0.5f;
        }
    }

    return SelectedShapeInstance->OnMouseUp(point, iKey);
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
    
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnRasterSelectionChanged);
    if (!rasterSelection->IsEmpty())
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());

    mHUD->AddElement(rasterSelection->GetHUD());
    mHUD->AddElement(mShapeHUD);
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::Unload()
{
    UOdysseyPainterEditorTool::Unload();
    mPaintEngine.SetMaskBlock(nullptr);
    
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().RemoveAll(this);

    mHUD->AddElement(rasterSelection->GetHUD());
    mHUD->RemoveElement(mShapeHUD);
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
    SelectedShapeInstance->Abort();
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedShapeInstance", AvailableShapes[SelectedShape]);
    mOnShapeChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset)
{
    mPath = iPoints;

    GEditor->BeginTransaction(LOCTEXT("raster-primitive-drawing-tool.transaction.draw-shape", "Draw Primitive Shape"));

    int w = mPaintEngine.PaintBlock()->Width();
    int h = mPaintEngine.PaintBlock()->Height();
    ::ULIS::eFormat format = mPaintEngine.PaintBlock()->Format();

    TSharedPtr<::ULIS::FBlock> maskBlock = MakeShared<ULIS::FBlock>(w, h, ::ULIS::Format_BGRA8);

    ::ULIS::FContext& ulisCtx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    ::ULIS::FContext& formatCtx = IULISLoaderModule::StaticFindOrAddContext(format);
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

    //if ( SubPixel )
    //{
        path.moveTo(mPath[0].x, mPath[0].y);
        for ( int i = 1; i < mPath.Num(); i++ )
        {
            path.lineTo(mPath[i].x, mPath[i].y);
        }
    /* }
    else
    {
        path.moveTo(FMath::Floor(mPath[0].x) + 0.5f, FMath::Floor(mPath[0].y) + 0.5f);
        for ( int i = 1; i < mPath.Num(); i++ )
        {
            path.lineTo(FMath::Floor(mPath[i].x) + 0.5f, FMath::Floor(mPath[i].y) + 0.5f);
        }
    } */

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
        blend2DCtx.setStrokeStyle(blend2DColor);
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

    formatCtx.Blend(*dstBlock, *mPaintEngine.PaintBlock(), dstBlock->Rect(), invalidRect.Position());
    formatCtx.Finish();

    mPaintEngine.PaintBlock()->Dirty();
    mPaintEngine.Update(BlendParameters);

    Flush();
    Commit();
    
    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();

    GEditor->EndTransaction();
    mShapeHUD->EmptyElements();
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

void
UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnRasterSelectionChanged()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (rasterSelection->IsEmpty())
    {
        mPaintEngine.SetMaskBlock(nullptr);
    }
    else
    {
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
    }
}

#undef LOCTEXT_NAMESPACE
