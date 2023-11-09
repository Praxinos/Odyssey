// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterTransformTool/Selection/OdysseyPainterEditorRasterSelection.h"
#include "GeomTools.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterSelection"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterSelection::~UOdysseyPainterEditorRasterSelection()
{
    //We give back what we don't own
    mEditor = nullptr;
    mHUD = nullptr;
    //--

    if (mSelectionBlock)
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }
}

UOdysseyPainterEditorRasterSelection::UOdysseyPainterEditorRasterSelection():
    mIsSelectionAreaSet(false),
    mSelectionArea(nullptr),
    mPaintEngine(),
    mSelectionBlock(nullptr)
{
}

void UOdysseyPainterEditorRasterSelection::Init(FOdysseyHUDElement* iHUD, FOdysseyPainterEditor* iEditor, bool iUniform)
{
    mHUD = iHUD;
    mEditor = iEditor;
    Uniform = iUniform;
}

bool UOdysseyPainterEditorRasterSelection::IsActivable() const
{
    return false;
}

bool UOdysseyPainterEditorRasterSelection::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

void UOdysseyPainterEditorRasterSelection::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void UOdysseyPainterEditorRasterSelection::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

bool UOdysseyPainterEditorRasterSelection::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool UOdysseyPainterEditorRasterSelection::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    return false;
}

bool UOdysseyPainterEditorRasterSelection::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    else if (iKey == EKeys::Enter || iKey == EKeys::SpaceBar || iKey == EKeys::Escape)
    {
        ClearSelection();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterSelection::Load()
{
}

void UOdysseyPainterEditorRasterSelection::Unload()
{
    ClearSelection();
}

bool UOdysseyPainterEditorRasterSelection::IsSelectionAreaSet()
{
    return mIsSelectionAreaSet;
}

bool UOdysseyPainterEditorRasterSelection::IsInSelectionArea(FVector2D iPoint)
{
    if( !mSelectionArea || mSelectionArea->GetPoints().Num() == 0 )
        return false;

    return FGeomTools2D::IsPointInPolygon(iPoint, mSelectionArea->GetPoints()); 
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> UOdysseyPainterEditorRasterSelection::GetSelectionBlock()
{
    return mSelectionBlock;
}

::ULIS::FRectI UOdysseyPainterEditorRasterSelection::GetSelectionAreaBoundingRect()
{
    if (mSelectionArea && mSelectionArea->GetPoints().Num() != 0)
    {
        int minX = mSelectionArea->GetPoints()[0].X;
        int maxX = mSelectionArea->GetPoints()[0].X;
        int minY = mSelectionArea->GetPoints()[0].Y;
        int maxY = mSelectionArea->GetPoints()[0].Y;
        for (int i = 1; i < mSelectionArea->GetPoints().Num(); i++)
        {
            minX = FMath::Min(minX, mSelectionArea->GetPoints()[i].X);
            maxX = FMath::Max(maxX, mSelectionArea->GetPoints()[i].X);
            minY = FMath::Min(minY, mSelectionArea->GetPoints()[i].Y);
            maxY = FMath::Max(maxY, mSelectionArea->GetPoints()[i].Y);
        }
        return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
    }
    return ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);
}

bool UOdysseyPainterEditorRasterSelection::IsSelectionValid(::ULIS::FRectI iSelectionArea)
{
    if (iSelectionArea.w > 8192 || iSelectionArea.h > 8192) //Unreal limitations + very slow in ULIS at these sizes
        return false;

    return true;
}

void UOdysseyPainterEditorRasterSelection::ClearSelection()
{
    mHUD->EmptyHUDElements();
    mIsSelectionAreaSet = false;
    if( mSelectionBlock )
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }
    mSelectionArea = nullptr;
    mEditor->HUDSystem()->ClearHUDSurface();
}

void UOdysseyPainterEditorRasterSelection::ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    if (!iBlock)
        return;

    ::ULIS::eFormat format = iBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ::ULIS::FEvent clearEvent;

    ctx.Clear(
        *iBlock,
        iBlock->Rect(),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
        0,
        nullptr,
        &clearEvent);

    ctx.Finish();
}

#undef LOCTEXT_NAMESPACE
