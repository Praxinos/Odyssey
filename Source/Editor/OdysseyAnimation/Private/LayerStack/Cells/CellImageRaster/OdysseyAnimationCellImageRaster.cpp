// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRasterEditingAbility.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderingAbility.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCellImageRaster"

TSharedPtr<FOdysseyAnimationCellImageRaster>
FOdysseyAnimationCellImageRaster::Create(UObject* iOwner, int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = MakeShared<FOdysseyAnimationCellImageRaster>(iOwner);
    cell->Init(iWidth, iHeight, iFormat);
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

FOdysseyAnimationCellImageRaster::FOdysseyAnimationCellImageRaster(UObject* iOwner)
    : mOwner(iOwner)
    , mRasterBlock(nullptr)

{
}

void
FOdysseyAnimationCellImageRaster::Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    mRasterBlock = MakeShared<FOdysseyRasterBlock>(mOwner);
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iWidth, iHeight, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.Clear(*block.Get());
    ctx.Finish();

    mRasterBlock->SetBlock(block);

    //InitDelegates();
    InitAbilities();
}

void
FOdysseyAnimationCellImageRaster::InitAbilities()
{
    //Set Abilities
    SetAbility(MakeShared<FOdysseyAnimationCellImageRasterImageRenderingAbility>(SharedThis(this)));
    SetAbility(MakeShared<FOdysseyAnimationCellImageRasterImageRasterEditingAbility>(SharedThis(this)));
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
    InitAbilities();
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
        mRasterBlock = MakeShared<FOdysseyRasterBlock>(mOwner);
    }
    Ar << *mRasterBlock;
}

#undef LOCTEXT_NAMESPACE