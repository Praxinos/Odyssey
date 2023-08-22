// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderingAbility.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterMediaAbility.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCellImageRaster"

TSharedPtr<FOdysseyAnimationCellImageRaster>
FOdysseyAnimationCellImageRaster::Create(UOdysseyAnimationLayerImageRaster* iLayer, int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = MakeShared<FOdysseyAnimationCellImageRaster>(iLayer);
    cell->Init(iWidth, iHeight, iFormat);
    return cell;
}

TSharedPtr<FOdysseyAnimationCellImageRaster>
FOdysseyAnimationCellImageRaster::Create(UOdysseyAnimationLayerImageRaster* iLayer, TSharedPtr<::ULIS::FBlock> iBlock)
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = MakeShared<FOdysseyAnimationCellImageRaster>(iLayer);
    cell->Init(iBlock);
    return cell;
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyAnimationCellImageRaster::GetRasterBlock() const
{
    return mRasterBlock;
}

const FName&
FOdysseyAnimationCellImageRaster::StaticType()
{
    static FName type = TEXT("FOdysseyAnimationCellImageRaster");
    return type;
}

FOdysseyAnimationCellImageRaster::~FOdysseyAnimationCellImageRaster()
{
}

FOdysseyAnimationCellImageRaster::FOdysseyAnimationCellImageRaster(UOdysseyAnimationLayerImageRaster* iLayer)
    : mLayer(iLayer)
    , mRasterBlock(nullptr)

{
}

void
FOdysseyAnimationCellImageRaster::Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    mRasterBlock = MakeShared<FOdysseyRasterBlock>(mLayer);
    mRasterBlock->PostProcess().BindRaw(this, &FOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iWidth, iHeight, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.Clear(*block.Get());
    ctx.Finish();

    mRasterBlock->SetBlock(block);

    //InitDelegates();
    InitAbilities();
}

void
FOdysseyAnimationCellImageRaster::Init(TSharedPtr<::ULIS::FBlock> iBlock)
{
    mRasterBlock = MakeShared<FOdysseyRasterBlock>(mLayer);
    mRasterBlock->PostProcess().BindRaw(this, &FOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
    mRasterBlock->SetBlock(iBlock);

    //InitDelegates();
    InitAbilities();
}

void
FOdysseyAnimationCellImageRaster::InitAbilities()
{
    //Set Abilities
    SetAbility(MakeShared<FOdysseyAnimationCellImageRasterImageRenderingAbility>(SharedThis(this)));
    SetAbility(MakeShared<FOdysseyAnimationCellImageRasterMediaAbility>(SharedThis(this)));
}

const FName&
FOdysseyAnimationCellImageRaster::GetType() const
{
    return StaticType();
}

void
FOdysseyAnimationCellImageRaster::PostLoad()
{
    FOdysseyAnimationCell::PostLoad();
    mRasterBlock->PostProcess().Unbind();
    mRasterBlock->PostProcess().BindRaw(this, &FOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
}

void
FOdysseyAnimationCellImageRaster::PostDuplicate()
{
    FOdysseyAnimationCell::PostDuplicate();
}

void
FOdysseyAnimationCellImageRaster::Serialize(FArchive& Ar)
{
    FOdysseyAnimationCell::Serialize(Ar);

    if ( Ar.IsLoading() )
    {
        mRasterBlock = MakeShared<FOdysseyRasterBlock>(mLayer);
    }
    
    Ar << *mRasterBlock;

    if ( Ar.IsLoading() )
    {
        InitAbilities();
    }
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayer->IsAlphaLocked)
        return iWaitList;

    //Apply AlphaLock
    TArray<::ULIS::FEvent> events;
    TSharedPtr<::ULIS::FBlock> block = mRasterBlock->GetBlock();
    TArray<FIntPoint> invalidTiles = iInvalidMap.InvalidTiles();    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mRasterBlock->GetFormat());
    for (const FIntPoint& invalidTile : invalidTiles)
    {
        TSharedPtr<::ULIS::FBlock> originalBlock = iOriginalBlocks[invalidTile];
        ::ULIS::FRectI rect = iInvalidMap.GetTileRect(invalidTile);
        ::ULIS::FEvent eventBlend;
        ctx.Blend(
            *originalBlock
            , *block
            , ::ULIS::FRectI::Auto
            , rect.Position()
            , ::ULIS::Blend_Back
            , ::ULIS::Alpha_Top
            , 1.f
            , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            , iWaitList.Num()
            , iWaitList.GetData()
            , &eventBlend
        );
        events.Add(eventBlend);
    }

    return events;
}

#undef LOCTEXT_NAMESPACE