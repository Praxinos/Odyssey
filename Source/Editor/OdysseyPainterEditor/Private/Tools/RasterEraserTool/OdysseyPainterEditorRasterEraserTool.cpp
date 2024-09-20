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
#include "FreehandShape/Interpolation/OdysseyInterpolationLine.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"

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
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
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

    shape->OnInteractive().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapeInteractive);
    shape->OnCommit().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapeCommit);
    shape->OnAbort().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapeAbort);

    shape->SetHUD( mShapeHUD );

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
    UOdysseyPainterEditorTool::Load();

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnRasterSelectionChanged);
    if (!rasterSelection->IsEmpty())
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
    
    mHUD->AddElement(rasterSelection->GetHUD());
    mHUD->AddElement(mShapeHUD);
	/* TODO: Done in OnMouseDown(), but check if we need to do something here too or not
    mPaintEngine.RasterBlock(mToolContext->GetRasterBlock());

	if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock()); */
}

void
UOdysseyPainterEditorRasterEraserTool::Unload()
{
	mPaintEngine.RasterBlock(nullptr);
    mPaintEngine.SetMaskBlock(nullptr);
    
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().RemoveAll(this);
    mHUD->RemoveElement(rasterSelection->GetHUD());
    mHUD->RemoveElement(mShapeHUD);

    UOdysseyPainterEditorTool::Unload();
}

bool
UOdysseyPainterEditorRasterEraserTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool
UOdysseyPainterEditorRasterEraserTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
    SelectedShapeInstance->Abort();
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, SelectedShapeInstance), AvailableShapes[SelectedShape]);
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
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, SelectedShape))
        SelectedShapeChanged();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Size))
        SizeChanged();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Opacity))
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

void
UOdysseyPainterEditorRasterEraserTool::OnRasterSelectionChanged()
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

void
UOdysseyPainterEditorRasterEraserTool::OnShapeInteractive(const TArray<FOdysseyPoint>& iPoints)
{
    if ( !SelectedShapeInstance->IsProgressive() )
        return;

    for ( const FOdysseyPoint& point : iPoints )
    {
        TArray<FOdysseyPoint> interpolatedPoints = InterpolateTo(point);
        for ( const FOdysseyPoint& interpolatedPoint : interpolatedPoints )
        {
            Stamp(interpolatedPoint);
        }
    }

    Flush();

    mPaintEngine.Update(mBlendParameters);
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset)
{
    if ( iReset )
    {
        ResetInterpolation();
        mPaintEngine.Abort();

        for ( const FOdysseyPoint& point : iPoints )
        {
            TArray<FOdysseyPoint> interpolatedPoints = InterpolateTo(point);
            for ( const FOdysseyPoint& interpolatedPoint : interpolatedPoints )
            {
                Stamp(interpolatedPoint);
            }
        }
    }

    mPaintEngine.Update(mBlendParameters);

    Flush();
    Commit();

    ResetInterpolation();
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapeAbort()
{
    ResetInterpolation();
    mPaintEngine.Abort();
    mPaintEngine.Update(mBlendParameters);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - Interpolation

TArray<FOdysseyPoint>
UOdysseyPainterEditorRasterEraserTool::InterpolateTo(const FOdysseyPoint& iPoint)
{
    if (!mInterpolator)
    {
        if (SelectedShape == EOdysseyShape::kFreehand )
        {
            switch(InterpolationType)
            {
                case EOdysseyInterpolationType::kCatmullRom: mInterpolator = MakeShared<FOdysseyInterpolationCatmullRom>(); break;
                case EOdysseyInterpolationType::kBezier: mInterpolator = MakeShared<FOdysseyInterpolationBezier>(); break;
                case EOdysseyInterpolationType::kLine: mInterpolator = MakeShared<FOdysseyInterpolationLine>(); break;

                default: break;
            }
        }
        else
        {
            mInterpolator = MakeShared<FOdysseyInterpolationLine>();
        }

        if (AdaptativeStep)
        {
            mInterpolator->SetStep( FMath::Max( 1.f, AdaptShapeStep(Step) ) );
        }
        else
        {
            mInterpolator->SetStep(Step);
        }
        
        mInterpolator->AddPoint( iPoint );
        mLastPoint = iPoint;
        return { iPoint };
    }

    //If the Interpolator is ready to produce points do it, otherwise.... don't (Thanks Captain Obvious)
    while( !mInterpolator->IsReady() )
    {
        //Add Point to Interpolator
        mInterpolator->AddPoint( iPoint );
    }

    TArray<FOdysseyPoint> newPoints = mInterpolator->ComputePoints();

    for( int i = 0; i < newPoints.Num(); ++i )
    {
        newPoints[i].ComputeRelativeParameters(mLastPoint, true);
        mLastPoint = newPoints[i];
    }

    return newPoints;
}

void
UOdysseyPainterEditorRasterEraserTool::ResetInterpolation()
{
    mInterpolator = nullptr;
    mLastPoint = FOdysseyPoint();
}

#undef LOCTEXT_NAMESPACE
