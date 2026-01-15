// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyHUDElement.h"

#include "FreehandShape/OdysseyFreehandShape.h"
#include "LineShape/OdysseyLineShape.h"
#include "RectangleShape/OdysseyRectangleShape.h"
#include "PolygonShape/OdysseyPolygonShape.h"
#include "EllipseShape/OdysseyEllipseShape.h"
#include "BezierShape/OdysseyBezierShape.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaProvider.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationBezier.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationCatmullRom.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationLine.h"
#include "OdysseyPainterEditorRasterSelection.h"

#include "UObject/OdysseyObjectEditorUtils.h"
#include "SOdysseySinglePropertyView.h"


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
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Eraser64");


    Shapes.AddShapeType(EOdysseyShapeType::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterEraserTool::FreehandShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterEraserTool::LineShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterEraserTool::RectangleShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterEraserTool::PolygonShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterEraserTool::EllipseShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterEraserTool::BezierShape"));

    Shapes.SetActiveShapeType(EOdysseyShapeType::kFreehand);

    mStampBlockMask = CreateStampBlockMask();
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
}

void
UOdysseyPainterEditorRasterEraserTool::Unload()
{
    Shapes.GetActiveShape()->Abort();

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
    return Shapes.GetActiveShape()->OnMouseDown(iPointInTexture, iKey);
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

    return Shapes.GetActiveShape()->OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyPainterEditorRasterEraserTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return;

    Shapes.GetActiveShape()->OnMouseHover(iPointInTexture);
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

    Shapes.GetActiveShape()->OnMouseDrag(iPointInTexture);
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
    mStampBlockMask = CreateStampBlockMask();
}

void
UOdysseyPainterEditorRasterEraserTool::OpacityChanged()
{
    mBlendParameters.Opacity = Opacity;
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
