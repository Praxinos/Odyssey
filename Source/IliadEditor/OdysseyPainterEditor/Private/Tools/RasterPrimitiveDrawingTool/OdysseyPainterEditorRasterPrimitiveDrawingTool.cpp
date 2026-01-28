// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterPrimitiveDrawingTool/OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "OdysseyFreehandShape.h"
#include "OdysseyLineShape.h"
#include "OdysseyRectangleShape.h"
#include "OdysseyPolygonShape.h"
#include "OdysseyEllipseShape.h"
#include "OdysseyBezierShape.h"

#include "OdysseyMediaRaster.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorRasterSelection.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterPrimitiveDrawingTool::~UOdysseyPainterEditorRasterPrimitiveDrawingTool()
{
}

UOdysseyPainterEditorRasterPrimitiveDrawingTool::UOdysseyPainterEditorRasterPrimitiveDrawingTool()
    : mPaintEngine()
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Shapes64"));

    UOdysseyFreehandShape* freehandShape = CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::FreehandShape");
    freehandShape->DisplayHUD(true);

    Shapes.AddShapeType(EOdysseyShapeType::kFreehand, freehandShape);
    Shapes.AddShapeType(EOdysseyShapeType::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::LineShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::RectangleShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::PolygonShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::EllipseShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterPrimitiveDrawingTool::BezierShape"));

    Shapes.SetActiveShapeType(EOdysseyShapeType::kFreehand);
}

template<class T>
T*
UOdysseyPainterEditorRasterPrimitiveDrawingTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnBegin().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeBegin);
    shape->OnCommit().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeCommit);
    shape->OnAbort().AddUObject(this, &UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeAbort);

    shape->SetHUD(mShapeHUD);

    return shape;
}

bool
UOdysseyPainterEditorRasterPrimitiveDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyPainterEditorRasterPrimitiveDrawingTool::GetRasterBlockFromEditor(bool iCreate) const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return nullptr;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters;
    if (iCreate)
        mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    else
        mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();

    if( mediaRasters.Num() <= 0 )
        return nullptr;

    return mediaRasters[0]->GetRasterBlock();
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyPainterEditorTool::OnMouseDown(iPointInTexture, iKey);

    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(true);
    if (!rasterBlock)
        return false;

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

    return Shapes.GetActiveShape()->OnMouseDown(point, iKey);
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
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

    Shapes.GetActiveShape()->OnMouseHover( point );
}

void UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        Shapes.GetActiveShape()->Abort();
        return;
    }

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

    Shapes.GetActiveShape()->OnMouseDrag(point);
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        Shapes.GetActiveShape()->Abort();
        return false;
    }

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

    return Shapes.GetActiveShape()->OnMouseUp(point, iKey);
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnKeyDown(const FKey& iKey)
{
    return Shapes.GetActiveShape()->OnKeyDown(iKey);
}

bool UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnKeyUp(const FKey& iKey)
{
    return Shapes.GetActiveShape()->OnKeyUp(iKey);
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
    Shapes.GetActiveShape()->Abort();

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
    Shapes.GetActiveShape()->Tick(iDeltaTime);

    //Update the paintEngine
    mPaintEngine.Update(BlendParameters);
}

void
UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeBegin()
{
    mTransaction = MakeShared<FScopedTransaction>(LOCTEXT("raster-primitive-drawing-tool.transaction.draw-shape", "Draw Primitive Shape"));
}

void
UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset)
{
    mPath = iPoints;

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
    img.create_from_data(w, h, BL_FORMAT_PRGB32, (void*)maskBlock->Bits(), stride);
    BLContext blend2DCtx;
    BLContextCreateInfo createInfo{};
    createInfo.thread_count = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    blend2DCtx.begin(img, createInfo);

    bool isLine = Shapes.GetActiveShapeType() == EOdysseyShapeType::kLine;
    bool isBezier = Shapes.GetActiveShapeType() == EOdysseyShapeType::kBezier;

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
    path.move_to(mPath[0].x, mPath[0].y);
    for ( int i = 1; i < mPath.Num(); i++ )
    {
        path.line_to(mPath[i].x, mPath[i].y);
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
        blend2DCtx.set_fill_style(blend2DColor);
        blend2DCtx.fill_path(path);
    }
    else
    {
        strokeWidth = StrokeWidth;
        blend2DCtx.set_stroke_style(blend2DColor);
        blend2DCtx.set_stroke_width(strokeWidth);
        blend2DCtx.stroke_path(path);
    }
    blend2DCtx.end();

    BLBox bbox;
    path.get_bounding_box(&bbox);

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

    mShapeHUD->EmptyElements();

    mTransaction = nullptr; //Finish the undo transaction
}

void
UOdysseyPainterEditorRasterPrimitiveDrawingTool::OnShapeAbort()
{
    if(mTransaction)
        mTransaction->Cancel();
    mTransaction = nullptr; //Finish the undo transaction
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
