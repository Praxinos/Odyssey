// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "OdysseyMediaRaster.h"
#include "Tools/RasterEraserTool/Widgets/SOdysseyPainterEditorRasterEraserToolTopTab.h"

#include "FreehandShape/OdysseyFreehandShape.h"
#include "LineShape/OdysseyLineShape.h"
#include "RectangleShape/OdysseyRectangleShape.h"
#include "PolygonShape/OdysseyPolygonShape.h"
#include "EllipseShape/OdysseyEllipseShape.h"
#include "BezierShape/OdysseyBezierShape.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDSystem.h"

#include "UObject/OdysseyObjectEditorUtils.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterEraserTool::~UOdysseyPainterEditorRasterEraserTool()
{
}

UOdysseyPainterEditorRasterEraserTool::UOdysseyPainterEditorRasterEraserTool()
    : Super()
    //Properties
    , SelectedShape(EOdysseyShape::kFreehand)
    , SelectedShapeInstance(nullptr)
    //Internal
    , mPaintEngine()
    , mStampBlock(nullptr)
    , mStampBlockMask(nullptr)
    , mBlendParameters(true, EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode::kNormal, Opacity)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Eraser64");

    AvailableShapes.Add(EOdysseyShape::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterEraserTool::FreehandShape"));
    AvailableShapes.Add(EOdysseyShape::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterEraserTool::LineShape"));
    AvailableShapes.Add(EOdysseyShape::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterEraserTool::RectangleShape"));
    AvailableShapes.Add(EOdysseyShape::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterEraserTool::PolygonShape"));
    AvailableShapes.Add(EOdysseyShape::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterEraserTool::EllipseShape"));
    AvailableShapes.Add(EOdysseyShape::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterEraserTool::BezierShape"));

    SelectedShapeInstance = AvailableShapes[SelectedShape];
    mStampBlockMask = CreateStampBlockMask();
}

template<class T>
T*
UOdysseyPainterEditorRasterEraserTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnPathBeginDelegate().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapePathBegin);
    shape->OnPathToDelegate().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapePathTo);
    shape->OnPathEndDelegate().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapePathEnd);
    shape->OnPathAbortDelegate().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapePathAbort);
    shape->OnPathResetDelegate().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapePathReset);

    shape->AdaptStepDelegate().BindUObject(this, &UOdysseyPainterEditorRasterEraserTool::AdaptShapeStep);

    shape->SetHUD( mHUD );

    return shape;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyPainterEditorRasterEraserTool::CreateStampBlockMask()
{
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(Size, Size, ::ULIS::Format_GAF);

    float center = Size/2.f;
    for (int y = 0; y < Size; y++)
    {
        for (int x = 0; x < Size; x++)
        {
            float dist = FVector2D::Distance(FVector2D(center, center), FVector2D(x, y)) / (Size / 2.f);
            ::ULIS::FColor color = ::ULIS::FColor::FromGreyAF(0, 0);
            if (dist <= 1.f)
                color = ::ULIS::FColor::FromGreyAF(0, 1.f - dist);
            block->SetPixel(x, y, color);
        }
    }

    return block;
}

float
UOdysseyPainterEditorRasterEraserTool::AdaptShapeStep(float iStep)
{
    return (iStep / 100.f) * Size;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorRasterEraserTool::Activate()
{   
    Super::Activate();
}

void
UOdysseyPainterEditorRasterEraserTool::Load()
{
	/* TODO: Done in OnMouseDown(), but check if we need to do something here too or not
    mPaintEngine.RasterBlock(mToolContext->GetRasterBlock());

	if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock()); */
}

void
UOdysseyPainterEditorRasterEraserTool::Unload()
{
	mPaintEngine.RasterBlock(nullptr);
}

bool
UOdysseyPainterEditorRasterEraserTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool
UOdysseyPainterEditorRasterEraserTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;
    
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    mPaintEngine.RasterBlock(rasterBlock);
    return SelectedShapeInstance->OnMouseDown(iPointInTexture, iKey);
}

bool
UOdysseyPainterEditorRasterEraserTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;

    return SelectedShapeInstance->OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyPainterEditorRasterEraserTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return;

    SelectedShapeInstance->OnMouseHover(iPointInTexture);
}

void
UOdysseyPainterEditorRasterEraserTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return;

    SelectedShapeInstance->OnMouseDrag(iPointInTexture);
}

bool
UOdysseyPainterEditorRasterEraserTool::OnKeyDown(const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;

    return SelectedShapeInstance->OnKeyDown(iKey);
}

bool
UOdysseyPainterEditorRasterEraserTool::OnKeyUp(const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;
        
    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;

    return SelectedShapeInstance->OnKeyUp(iKey);
}

void
UOdysseyPainterEditorRasterEraserTool::Commit()
{
    FScopedTransaction transaction(LOCTEXT("raster-drawing-tool.transaction.paint-stroke", "Paint Stroke"));
    mPaintEngine.Commit(mBlendParameters);
    
    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();
}

TSharedRef<SWidget>
UOdysseyPainterEditorRasterEraserTool::CreateTopTabWidget()
{
    return SNew(SOdysseyPainterEditorRasterEraserToolTopTab, this);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Shape Callbacks

void
UOdysseyPainterEditorRasterEraserTool::PrepareStampBlock()
{
    TSharedPtr<::ULIS::FBlock> paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(paintBlock->Format());
    if (!mStampBlock || mStampBlock->Width() != mStampBlockMask->Width() || mStampBlock->Height() != mStampBlockMask->Height() || mStampBlock->Format() != paintBlock->Format())
    {
        mStampBlock = MakeShared<::ULIS::FBlock>(mStampBlockMask->Width(), mStampBlockMask->Height(), paintBlock->Format());
        ctx.ConvertFormat(*mStampBlockMask, *mStampBlock);
        ctx.Finish();
    }
}

void
UOdysseyPainterEditorRasterEraserTool::Stamp(const FOdysseyPoint& iPoint)
{
    PrepareStampBlock();

    TSharedPtr<::ULIS::FBlock> paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(paintBlock->Format());

    ctx.BlendAA(
          *mStampBlock
        , *paintBlock
        , ::ULIS::FRectI::Auto
        , ::ULIS::FVec2F(iPoint.x - mStampBlock->Width() / 2.f, iPoint.y - mStampBlock->Height() / 2.f)
        , ::ULIS::Blend_Normal
        , ::ULIS::Alpha_Normal
        , iPoint.pressure * Flow / 100.f
    );
    //ctx.Copy(*mStampBlock, *paintBlock);
    ctx.Finish();

    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(iPoint.x - mStampBlock->Width() / 2.f, iPoint.y - mStampBlock->Height() / 2.f, mStampBlock->Width(), mStampBlock->Height());
    paintBlock->Dirty(rect);
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapePathBegin( const FOdysseyPoint& iPoint )
{
    Stamp(iPoint);
    mPaintEngine.Update(mBlendParameters);
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapePathTo( const TArray<FOdysseyPoint>& iPoints )
{
    for (int i = 0; i < iPoints.Num(); i++)
    {
        Stamp(iPoints[i]);
    }
    mPaintEngine.Update(mBlendParameters);
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapePathEnd( const FOdysseyPoint& iPoint )
{
    Flush();
    Commit();

    mHUD->EmptyHUDElements();
    mEditor->HUDSystem()->ClearHUDSurface();
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapePathAbort()
{
    mPaintEngine.Abort();

    //Update immediately the changes
    mPaintEngine.Update(mBlendParameters);
    
    mHUD->EmptyHUDElements();
    mEditor->HUDSystem()->ClearHUDSurface();
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapePathReset()
{
    mPaintEngine.Abort();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

EOdysseyShape
UOdysseyPainterEditorRasterEraserTool::GetSelectedShape() const
{
    return SelectedShape;
}

UOdysseyShape*
UOdysseyPainterEditorRasterEraserTool::GetSelectedShapeInstance() const
{
    return SelectedShapeInstance;
}

FSimpleMulticastDelegate&
UOdysseyPainterEditorRasterEraserTool::OnShapeChanged()
{
    return mOnShapeChanged;
}

FSimpleMulticastDelegate&
UOdysseyPainterEditorRasterEraserTool::OnSizeChanged()
{
    return mOnSizeChanged;
}

FSimpleMulticastDelegate&
UOdysseyPainterEditorRasterEraserTool::OnOpacityChanged()
{
    return mOnOpacityChanged;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- UObject Overrides

void
UOdysseyPainterEditorRasterEraserTool::SelectedShapeChanged()
{
    SelectedShapeInstance->AbortShape();
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedShapeInstance", AvailableShapes[SelectedShape]);
    mOnShapeChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterEraserTool::SizeChanged()
{
    mStampBlockMask = CreateStampBlockMask();
    mOnSizeChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterEraserTool::OpacityChanged()
{
    mBlendParameters.Opacity = Opacity;
    mOnOpacityChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterEraserTool::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == "SelectedShape")
        SelectedShapeChanged();

    if (iPropertyName == "Size")
        SizeChanged();

    if (iPropertyName == "Opacity")
        OpacityChanged();
}

EMouseCursor::Type
UOdysseyPainterEditorRasterEraserTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

    return UOdysseyPainterEditorTool::GetMouseCursor();
}

FText
UOdysseyPainterEditorRasterEraserTool::GetTooltip() const
{
    return LOCTEXT("raster-eraser-tool.tooltip", "Eraser Tool");
}

#undef LOCTEXT_NAMESPACE
