// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "Abilities/OdysseyAnimationImageProviderAbility.h"
#include "LayerStack/OdysseyAnimationLayerImageRasterCell.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationLayerImageRaster"

UOdysseyAnimationLayerImageRaster::FOnIsAlphaLockedChanged&
UOdysseyAnimationLayerImageRaster::OnIsAlphaLockedChanged()
{
    static FOnIsAlphaLockedChanged onIsAlphaLockedChanged;
    return onIsAlphaLockedChanged;
}

UOdysseyAnimationLayerImageRaster::FOnBlendModeChanged&
UOdysseyAnimationLayerImageRaster::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyAnimationLayerImageRaster::FOnOpacityChanged&
UOdysseyAnimationLayerImageRaster::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

UOdysseyAnimationLayerImageRaster::FOnCellsChanged&
UOdysseyAnimationLayerImageRaster::OnCellsChanged()
{
    static FOnCellsChanged onCellsChanged;
    return onCellsChanged;
}

UOdysseyAnimationLayerImageRaster::~UOdysseyAnimationLayerImageRaster()
{
}

UOdysseyAnimationLayerImageRaster::UOdysseyAnimationLayerImageRaster()
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Raster Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
}

void
UOdysseyAnimationLayerImageRaster::OnCreated_Implementation()
{
    AddImageCell();
}

FInt32Range
UOdysseyAnimationLayerImageRaster::GetFrameRange() const
{
    uint32 length = 0;
    for (TSharedPtr<FOdysseyAnimationLayerCell> cell : mCells)
    {
        length += cell->GetLength();
    }
    return FInt32Range::Inclusive(Offset, Offset + length - 1);
}

FString
UOdysseyAnimationLayerImageRaster::GetFrameId(int iFrameIndex) const
{
    int cellIndex = INDEX_NONE;
    int cellFrameIndex = INDEX_NONE;
    if (!GetCellIndexAtFrame(iFrameIndex, cellIndex, cellFrameIndex))
        return "";

    TSharedPtr<FOdysseyAnimationLayerCell> cell = mCells[cellIndex];
    TSharedPtr<FOdysseyAnimationImageProviderAbility> imageProviderAbility = cell->GetAbility<FOdysseyAnimationImageProviderAbility>();
    if (!imageProviderAbility )
        return "";
	return imageProviderAbility->GetFrameId(cellFrameIndex);
}

TSharedRef<IOdysseyAnimationRenderImageState>
UOdysseyAnimationLayerImageRaster::CreateRenderImageState() const
{
    return MakeShared<FOdysseyAnimationLayerImageRasterRenderImageState>(this);
}

int
UOdysseyAnimationLayerImageRaster::GetCellsCount() const
{
    return mCells.Num();
}

bool
UOdysseyAnimationLayerImageRaster::GetCellIndexAtFrame(int iFrameIndex, int& oCellIndex, int& oCellFrameIndex) const
{
    if( iFrameIndex < Offset)
        return false;

    int frameIndex = Offset;
    for (int i = 0; i < mCells.Num(); i++)
    {
        TSharedPtr<FOdysseyAnimationLayerCell> cell = mCells[i];
        frameIndex += cell->GetLength();

        if (frameIndex >= iFrameIndex)
        {
            oCellIndex = i;
            oCellFrameIndex = frameIndex - iFrameIndex;
            return true;
        }
    }

    return false;
}

bool
UOdysseyAnimationLayerImageRaster::GetCellFrameRange(int iIndex, FInt32Range& oFrameRange) const
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return false;

    uint32 startFrame = Offset;
    for (int i = 0; i < iIndex; i++ )
    {
        startFrame += mCells[i]->GetLength();
    }
    oFrameRange = FInt32Range::Inclusive(startFrame, startFrame + mCells[iIndex]->GetLength() - 1);
    return true;
}

bool
UOdysseyAnimationLayerImageRaster::GetCellLength(int iIndex, int& oLength) const
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return false;

    oLength = mCells[iIndex]->GetLength();
    return true;
}

bool
UOdysseyAnimationLayerImageRaster::GetCellType(int iIndex, FName& oType) const
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return false;

    oType = mCells[iIndex]->GetType();
    return true;
}

void
UOdysseyAnimationLayerImageRaster::AddImageCell()
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    //TODO: Use iIndex to Insert
    mCells.Add(FOdysseyAnimationLayerImageRasterCell::Create(this, animation->Width(), animation->Height(), animation->Format()));
    RenderImageIdChanged();
}

void
UOdysseyAnimationLayerImageRaster::RemoveCell(int iIndex)
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return;

    mCells.RemoveAt(iIndex);
    RenderImageIdChanged();
}

void
UOdysseyAnimationLayerImageRaster::SetCellLength(int iIndex, int iLength)
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return;

    mCells[iIndex]->SetLength(iLength);
    RenderImageIdChanged();
}

TArray<TSharedPtr<FOdysseyAnimationLayerCell>>&
UOdysseyAnimationLayerImageRaster::GetCells()
{
    return mCells;
}

TSharedPtr<FOdysseyAnimationLayerCell>
UOdysseyAnimationLayerImageRaster::GetCell(int iIndex) const
{
    if ( iIndex < 0 || iIndex >= mCells.Num() )
        return nullptr;

    return mCells[iIndex];
}

TSharedPtr<FOdysseyAnimationLayerCell> 
UOdysseyAnimationLayerImageRaster::GetCellAtFrame(int iFrameIndex, int& iCelFrameIndex) const
{
    int celIndex = INDEX_NONE;
    int celFrameIndex = INDEX_NONE;
    if ( !GetCellIndexAtFrame(iFrameIndex, celIndex, celFrameIndex) )
        return nullptr;

    return mCells[celIndex];
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerImageRaster::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    int cellIndex = INDEX_NONE;
    int cellFrameIndex = INDEX_NONE;
    if (!GetCellIndexAtFrame(iFrame, cellIndex, cellFrameIndex))
        return iWaitList;

    TSharedPtr<FOdysseyAnimationLayerCell> cell = mCells[cellIndex];
    TSharedPtr<FOdysseyAnimationImageProviderAbility> imageAbility = cell->GetAbility<FOdysseyAnimationImageProviderAbility>();
    if (!imageAbility)
        return iWaitList;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = imageAbility->GetBlock(cellFrameIndex);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, ULISRasterBlock->Format(), iRect, iPos, iWaitList,
        [this, &ULISRasterBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(ULISRasterBlock).Build();
            ctx.Blend(
                *ULISRasterBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::eBlendMode(BlendMode),
                ::ULIS::Alpha_Normal,
                Opacity,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventBlend
            );
            return { eventBlend };
        }
    );

    ctx.Flush();

    return eventConvertAndExecute;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerImageRaster::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    int cellIndex = INDEX_NONE;
    int cellFrameIndex = INDEX_NONE;
    if (!GetCellIndexAtFrame(iFrame, cellIndex, cellFrameIndex))
        return iWaitList;

    TSharedPtr<FOdysseyAnimationLayerCell> cell = mCells[cellIndex];
    TSharedPtr<FOdysseyAnimationImageProviderAbility> imageAbility = cell->GetAbility<FOdysseyAnimationImageProviderAbility>();
    if (!imageAbility)
        return iWaitList;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = imageAbility->GetBlock(cellFrameIndex);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, ULISRasterBlock->Format(), iRect, iPos, iWaitList,
        [this, &ULISRasterBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(ULISRasterBlock).Build();
            ctx.Copy(
                *ULISRasterBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventCopy
            );
            return { eventCopy };
        }
    );

    ctx.Flush();

    return eventConvertAndExecute;
}

void
UOdysseyAnimationLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    //TODO: Merge Cells and Raster Blocks

    /*
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Merge Layers"));
#endif

    RasterBlock->Modify();
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();

    //Make tmpblock to merge the layers into
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> lastEvent = {};
    for (UOdysseyLayer* layer : iLayers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        lastEvent = animationLayer->RenderImage(ULISRasterBlock, ULISRasterBlock->Rect(), ::ULIS::FVec2I(0), lastEvent);
    }
    ctx.Finish();

    RasterBlock->Invalidate({ ULISRasterBlock->Rect() }, false);
    */
}

void
UOdysseyAnimationLayerImageRaster::IsAlphaLockedChanged()
{
    OnIsAlphaLockedChanged().Broadcast(this);
}

void
UOdysseyAnimationLayerImageRaster::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);
    RenderImageIdChanged();
}

void
UOdysseyAnimationLayerImageRaster::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    RenderImageIdChanged();
}

void
UOdysseyAnimationLayerImageRaster::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
    if (iPropertyName == "IsAlphaLocked")
        IsAlphaLockedChanged();
}

void
UOdysseyAnimationLayerImageRaster::PostLoad()
{
    Super::PostLoad();

    for (TSharedPtr<FOdysseyAnimationLayerCell> cell : mCells)
        cell->PostLoad();

    /* for ( TSharedPtr<FOdysseyRasterBlock> rasterBlock : mRasterBlocks )
    {
        rasterBlock->OnBlockChanged().RemoveAll(this);
        rasterBlock->OnBlockPtrChanged().RemoveAll(this);
        rasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockChanged, rasterBlock);
        rasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged, rasterBlock);
    } */
}

void
UOdysseyAnimationLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    for (TSharedPtr<FOdysseyAnimationLayerCell> cell : mCells)
        cell->PostDuplicate();

    Super::PostDuplicate(bDuplicateForPIE);
}

TSharedPtr<IOdysseyHandle>
UOdysseyAnimationLayerImageRaster::Preload(int iFrame)
{
    int cellIndex = INDEX_NONE;
    int cellFrameIndex = INDEX_NONE;
    if (!GetCellIndexAtFrame(iFrame, cellIndex, cellFrameIndex))
        return nullptr;

    TSharedPtr<FOdysseyAnimationLayerCell> cell = mCells[cellIndex];
    return cell->Preload(cellFrameIndex);
}

void
UOdysseyAnimationLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if ( Ar.IsLoading() )
    {
        mCells.Empty();
    }

    int32 numCells = mCells.Num();
    Ar << numCells;
    for ( int i = 0; i < numCells; i++ )
    {
        if ( Ar.IsLoading() )
        {
            FName cellType;
            Ar << cellType;
            TSharedPtr<FOdysseyAnimationLayerCell> cell;
            if ( cellType == FOdysseyAnimationLayerImageRasterCell::StaticType() )
            {
                cell = MakeShared<FOdysseyAnimationLayerImageRasterCell>(this);
            }
            else
            {
                checkf(false, TEXT(""))
            }
            cell->Serialize(Ar);
            mCells.Add(cell);
        }
        else
        {
            FName cellType = mCells[i]->GetType();
            Ar << cellType;
            mCells[i]->Serialize(Ar);
        }
    }
}


#undef LOCTEXT_NAMESPACE