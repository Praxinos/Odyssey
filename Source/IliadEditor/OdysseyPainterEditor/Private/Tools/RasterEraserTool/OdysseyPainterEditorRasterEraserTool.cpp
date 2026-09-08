// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyHUDElement.h"

#include "OdysseyFreehandShape.h"
#include "OdysseyLineShape.h"
#include "OdysseyRectangleShape.h"
#include "OdysseyPolygonShape.h"
#include "OdysseyEllipseShape.h"
#include "OdysseyBezierShape.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaProvider.h"
#include "Interpolation/OdysseyInterpolationBezier.h"
#include "Interpolation/OdysseyInterpolationCatmullRom.h"
#include "Interpolation/OdysseyInterpolationLine.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "SOdysseySinglePropertyView.h"

#include "ToolMenu.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Layout/SBox.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterEraserTool::~UOdysseyPainterEditorRasterEraserTool()
{
}

UOdysseyPainterEditorRasterEraserTool::UOdysseyPainterEditorRasterEraserTool()
    : Super()
    //Internal
    , mPaintEngine()
    , mStampBlock(nullptr)
    , mStampBlockMask(nullptr)
    , mBlendParameters(true, EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode::kNormal, Opacity)
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Eraser64"));

    mHUD->AddElement(mShapeHUD);

    Shapes.AddShapeType(EOdysseyShapeType::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterEraserTool::FreehandShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterEraserTool::LineShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterEraserTool::RectangleShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterEraserTool::PolygonShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterEraserTool::EllipseShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterEraserTool::BezierShape"));

    Shapes.SetActiveShapeType(EOdysseyShapeType::kFreehand);
}

template<class T>
T*
UOdysseyPainterEditorRasterEraserTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnBegin().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapeBegin);
    shape->OnInteractive().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapeInteractive);
    shape->OnCommit().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapeCommit);
    shape->OnAbort().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnShapeAbort);

    shape->SetHUD( mShapeHUD );

    return shape;
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

    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterEraserTool::OnRasterSelectionChanged);
    if (!rasterSelection->IsEmpty())
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
}

void
UOdysseyPainterEditorRasterEraserTool::Unload()
{
    Shapes.GetActiveShape()->Abort();

    mPaintEngine.RasterBlock(nullptr);
    mPaintEngine.SetMaskBlock(nullptr);

    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().RemoveAll(this);

    UOdysseyPainterEditorTool::Unload();
}

void UOdysseyPainterEditorRasterEraserTool::RefreshToolFromProperties()
{
    SizeChanged();
    OpacityChanged();
}

bool
UOdysseyPainterEditorRasterEraserTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool
UOdysseyPainterEditorRasterEraserTool::HasRadius() const
{
    return true;
}

void
UOdysseyPainterEditorRasterEraserTool::SetRadius(float Radius)
{
    Size = Radius * 2.f;
    if (!IsInInteractiveMode())
    {
        SizeChanged();
        mOnSizeChanged.Broadcast();
    }
}

float
UOdysseyPainterEditorRasterEraserTool::GetRadius() const
{
    return Size / 2.f;
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyPainterEditorRasterEraserTool::GetRasterBlockFromEditor(bool iCreate) const
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

void
UOdysseyPainterEditorRasterEraserTool::Tick(float iDeltaTime)
{
    Shapes.GetActiveShape()->Tick(iDeltaTime);
    mWorker.ExecuteFor(1000 / 60); //60fps
    mPaintEngine.Update(mBlendParameters);
}

bool
UOdysseyPainterEditorRasterEraserTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyPainterEditorTool::OnMouseDown(iPointInTexture, iKey);

    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(true);
    if (!rasterBlock)
        return false;

    mPaintEngine.RasterBlock(rasterBlock);

    mSubPixelPoint = iPointInTexture;
    if (!SubPixel)
    {
        mSubPixelPoint.x = FMath::Floor(mSubPixelPoint.x) + 0.5f;
        mSubPixelPoint.y = FMath::Floor(mSubPixelPoint.y) + 0.5f;
    }

    return Shapes.GetActiveShape()->OnMouseDown(mSubPixelPoint, iKey);
}

bool
UOdysseyPainterEditorRasterEraserTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        Shapes.GetActiveShape()->Abort();
        return false;
    }

    mSubPixelPoint = iPointInTexture;
    if (!SubPixel)
    {
        mSubPixelPoint.x = FMath::Floor(mSubPixelPoint.x) + 0.5f;
        mSubPixelPoint.y = FMath::Floor(mSubPixelPoint.y) + 0.5f;
    }

    return Shapes.GetActiveShape()->OnMouseUp(mSubPixelPoint, iKey);
}

void
UOdysseyPainterEditorRasterEraserTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return;

    mSubPixelPoint = iPointInTexture;
    if (!SubPixel)
    {
        mSubPixelPoint.x = FMath::Floor(mSubPixelPoint.x) + 0.5f;
        mSubPixelPoint.y = FMath::Floor(mSubPixelPoint.y) + 0.5f;
    }

    Shapes.GetActiveShape()->OnMouseHover(mSubPixelPoint);
}

void
UOdysseyPainterEditorRasterEraserTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
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
        point.x = FMath::Floor(point.x) + 0.5f;
        point.y = FMath::Floor(point.y) + 0.5f;

        if (mSubPixelPoint == point)
            return;
    }

    mSubPixelPoint = point;

    Shapes.GetActiveShape()->OnMouseDrag(point);
}

bool
UOdysseyPainterEditorRasterEraserTool::OnKeyDown(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Shapes.GetActiveShape()->OnKeyDown(iKey);
}

bool
UOdysseyPainterEditorRasterEraserTool::OnKeyUp(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Shapes.GetActiveShape()->OnKeyUp(iKey);
}

void
UOdysseyPainterEditorRasterEraserTool::Commit()
{
    mPaintEngine.Commit(mBlendParameters);

    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();
}

void
UOdysseyPainterEditorRasterEraserTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Size), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Opacity), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Flow), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Shape Callbacks



void
UOdysseyPainterEditorRasterEraserTool::UpdateStampBlockMask()
{
    int32 StampSize = 1 + FMath::CeilToInt32(Size) * 2; //we add 1 for antialisaing purposes
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(StampSize, StampSize, ::ULIS::Format_GAF);

    FVector2D center(StampSize/2.f, StampSize/2.f);
    float radius = Size / 2.f;
    float normalizedHardness = Hardness / 100.f;
    float AAThreshold = Antialiasing ? 0.5f / StampSize : 0.0f;
    float AALowDist = 1.0f - AAThreshold;
    float AAHighDist = 1.0f + AAThreshold;
    float AARatio = AAHighDist != AALowDist ? 1.0f / (AAHighDist - AALowDist) : 1.f;

    for (int y = 0; y < StampSize; y++)
    {
        for (int x = 0; x < StampSize; x++)
        {
            float dist = FVector2D::Distance(center, FVector2D(x + 0.5f, y + 0.5f)) / radius;
            ::ULIS::FColor color = ::ULIS::FColor::FromGreyAF(0, 0);

            if (dist >= AAHighDist)
            {
                //Write empty pixel
                block->SetPixel(x, y, color);
                continue;
            }

            float alpha = 1.0f;
            if (normalizedHardness >= 1.0f)
            {
                alpha = 1.0f;
            }
            else if (dist > normalizedHardness)
            {
                float newDist = (dist - normalizedHardness) / (1.0f - normalizedHardness);
                alpha = FMath::Clamp(1.0f - newDist, 0.0f, 1.0f);
            }

            if (dist <= AALowDist)
            {
                //Write non AA pixel
                color = ::ULIS::FColor::FromGreyAF(0, alpha);
                block->SetPixel(x, y, color);
                continue;
            }

            // Write AA pixel
            float AAValue = (dist - AALowDist) * AARatio;
            color = ::ULIS::FColor::FromGreyAF(0, alpha * AAValue);
            block->SetPixel(x, y, color);
        }
    }

    mStampBlockMask = block;
}

void
UOdysseyPainterEditorRasterEraserTool::UpdateStampBlock()
{
    TSharedPtr<::ULIS::FBlock> paintBlock = mPaintEngine.PaintBlock();

    if (!mStampBlock || bNeedsStampBlockUpdate)
    {
        UpdateStampBlockMask();

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(paintBlock->Format());
        mStampBlock = MakeShared<::ULIS::FBlock>(mStampBlockMask->Width(), mStampBlockMask->Height(), paintBlock->Format());
        ctx.ConvertFormat(*mStampBlockMask, *mStampBlock);
        ctx.Finish();

        bNeedsStampBlockUpdate = false;
    }
}

void
UOdysseyPainterEditorRasterEraserTool::Stamp(const FOdysseyPoint& iPoint)
{
    UpdateStampBlock();

    TSharedPtr<::ULIS::FBlock> paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(paintBlock->Format());

    if (Antialiasing)
    {
        ctx.BlendAA(
            *mStampBlock
            , *paintBlock
            , ::ULIS::FRectI::Auto
            , ::ULIS::FVec2F(iPoint.x - mStampBlock->Width() / 2.f, iPoint.y - mStampBlock->Height() / 2.f)
            , ::ULIS::Blend_Normal
            , ::ULIS::Alpha_Normal
            , iPoint.pressure * Flow / 100.f
        );
    }
    else
    {
        ctx.Blend(
            *mStampBlock
            , *paintBlock
            , ::ULIS::FRectI::Auto
            , ::ULIS::FVec2F(iPoint.x - mStampBlock->Width() / 2.f, iPoint.y - mStampBlock->Height() / 2.f)
            , ::ULIS::Blend_Normal
            , ::ULIS::Alpha_Normal
            , iPoint.pressure * Flow / 100.f
        );
    }
    //ctx.Copy(*mStampBlock, *paintBlock);
    ctx.Finish();

    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(iPoint.x - mStampBlock->Width() / 2.f, iPoint.y - mStampBlock->Height() / 2.f, mStampBlock->Width(), mStampBlock->Height());
    paintBlock->Dirty(rect);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

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
UOdysseyPainterEditorRasterEraserTool::SizeChanged()
{
    bNeedsStampBlockUpdate = true;
}

void
UOdysseyPainterEditorRasterEraserTool::OpacityChanged()
{
    mBlendParameters.Opacity = Opacity;
}

void
UOdysseyPainterEditorRasterEraserTool::HardnessChanged()
{
    bNeedsStampBlockUpdate = true;
}

void
UOdysseyPainterEditorRasterEraserTool::AntialiasingChanged()
{
    bNeedsStampBlockUpdate = true;
}

void UOdysseyPainterEditorRasterEraserTool::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName, iMemberPropertyName, iIsInteractive);

    if (iIsInteractive)
        return;

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Size))
        SizeChanged();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Opacity))
        OpacityChanged();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Hardness))
        HardnessChanged();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Antialiasing))
        AntialiasingChanged();
}

void
UOdysseyPainterEditorRasterEraserTool::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
    Super::PostPropertyChanged(iPropertyName, iIsInteractive);

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Size))
        mOnSizeChanged.Broadcast();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterEraserTool, Opacity))
        mOnOpacityChanged.Broadcast();
}

TOptional<FMouseCursor> UOdysseyPainterEditorRasterEraserTool::GetMouseCursorOverride() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if( mediaProvider.IsLocked() )
        return FMouseCursor( EMouseCursor::SlashedCircle );

    return Super::GetMouseCursorOverride();
}

FText
UOdysseyPainterEditorRasterEraserTool::GetTooltip() const
{
    return LOCTEXT("raster-eraser-tool.tooltip", "Eraser Tool");
}

void
UOdysseyPainterEditorRasterEraserTool::OnRasterSelectionChanged()
{
    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
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
UOdysseyPainterEditorRasterEraserTool::OnShapeBegin()
{
    mTransaction = MakeShared<FScopedTransaction>(LOCTEXT("raster-eraser-tool.transaction.paint-stroke", "Eraser Stroke"));
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapeInteractive(const TArray<FOdysseyPoint>& iPoints)
{
    if ( !Shapes.GetActiveShape()->IsProgressive() )
        return;

    for ( const FOdysseyPoint& point : iPoints )
    {
        TArray<FOdysseyPoint> interpolatedPoints = InterpolateTo(point);
        for ( const FOdysseyPoint& interpolatedPoint : interpolatedPoints )
        {
            mWorker.Push([this, interpolatedPoint]()
                {
                    Stamp(interpolatedPoint);
                });
        }
    }

    Flush();
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
    mWorker.Finish();
    Commit();
    mTransaction = nullptr; //Finish the undo transaction

    ResetInterpolation();
}

void
UOdysseyPainterEditorRasterEraserTool::OnShapeAbort()
{
    ResetInterpolation();
    mPaintEngine.Abort();
    mPaintEngine.Update(mBlendParameters);
    if(mTransaction)
        mTransaction->Cancel();
    mTransaction = nullptr; //Finish the undo transaction
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - Interpolation

TArray<FOdysseyPoint>
UOdysseyPainterEditorRasterEraserTool::InterpolateTo(const FOdysseyPoint& iPoint)
{
    if (!mInterpolator)
    {
        if (Shapes.GetActiveShapeType() == EOdysseyShapeType::kFreehand )
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
