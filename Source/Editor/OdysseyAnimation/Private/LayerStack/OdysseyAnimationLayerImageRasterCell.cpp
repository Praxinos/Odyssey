// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerImageRasterCell.h"

#include "Abilities/OdysseyAnimationImageProviderAbility.h"
#include "Abilities/OdysseyAnimationImageRasterEditingAbility.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationLayerImageRasterCell"

TSharedPtr<FOdysseyAnimationLayerImageRasterCell>
FOdysseyAnimationLayerImageRasterCell::Create(UOdysseyLayer* iLayer, int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    TSharedPtr<FOdysseyAnimationLayerImageRasterCell> cell = MakeShared<FOdysseyAnimationLayerImageRasterCell>(iLayer);
    cell->Init(iWidth, iHeight, iFormat);
    return cell;
}

const FName&
FOdysseyAnimationLayerImageRasterCell::StaticType()
{
    static FName type = TEXT("FOdysseyAnimationLayerImageRasterCell");
    return type;
}

FOdysseyAnimationLayerImageRasterCell::~FOdysseyAnimationLayerImageRasterCell()
{
    mRasterBlock->OnBlockChanged().RemoveAll(this);
    mRasterBlock->OnBlockCommited().RemoveAll(this);
    mRasterBlock->OnBlockPtrChanged().RemoveAll(this);
}

FOdysseyAnimationLayerImageRasterCell::FOdysseyAnimationLayerImageRasterCell(UOdysseyLayer* iLayer)
    : FOdysseyAnimationLayerCell(iLayer)
    , mRasterBlock(nullptr)
{
}

void
FOdysseyAnimationLayerImageRasterCell::Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    mRasterBlock = MakeShared<FOdysseyRasterBlock>(GetLayer());
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iWidth, iHeight, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.Clear(*block.Get());
    ctx.Finish();

    mRasterBlock->SetBlock(block);

    InitDelegates();
    InitAbilities();
}

void
FOdysseyAnimationLayerImageRasterCell::InitAbilities()
{
    //Set Abilities
    SetAbility(MakeShared<FImageProviderAbility>(SharedThis(this)));
    SetAbility(MakeShared<FImageRasterEditingAbility>(SharedThis(this)));
}

void
FOdysseyAnimationLayerImageRasterCell::InitDelegates()
{
    //Set Abilities
    mRasterBlock->OnBlockChanged().AddRaw(this, &::FOdysseyAnimationLayerImageRasterCell::OnBlockChanged);
    mRasterBlock->OnBlockCommited().AddRaw(this, &::FOdysseyAnimationLayerImageRasterCell::OnBlockCommited);
    mRasterBlock->OnBlockPtrChanged().AddRaw(this, &::FOdysseyAnimationLayerImageRasterCell::OnBlockPtrChanged);
}

const FName&
FOdysseyAnimationLayerImageRasterCell::GetType() const
{
    return StaticType();
}

void
FOdysseyAnimationLayerImageRasterCell::PostLoad()
{
    FOdysseyAnimationLayerCell::PostLoad();
    InitDelegates();
    InitAbilities();
}

void
FOdysseyAnimationLayerImageRasterCell::PostDuplicate()
{
    FOdysseyAnimationLayerCell::PostDuplicate();
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerImageRasterCell::Preload(int iFrameIndex)
{
    return mRasterBlock->Preload();
}

void
FOdysseyAnimationLayerImageRasterCell::Serialize(FArchive& Ar)
{
    FOdysseyAnimationLayerCell::Serialize(Ar);

    if ( Ar.IsLoading() )
    {
        mRasterBlock = MakeShared<FOdysseyRasterBlock>(GetLayer());
    }
    Ar << *mRasterBlock;
}

void
FOdysseyAnimationLayerImageRasterCell::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    OnRenderImageRaster
}

void
FOdysseyAnimationLayerImageRasterCell::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{

}

void
FOdysseyAnimationLayerImageRasterCell::OnBlockPtrChanged()
{

}

//======== Abilities
  
class FOdysseyAnimationLayerImageRasterCell::FImageProviderAbility
    : public FOdysseyAnimationImageProviderAbility
{
public:
    FImageProviderAbility(TSharedPtr<FOdysseyAnimationLayerImageRasterCell> iCell)
        : mCell(iCell)
    {
    }

public:
    virtual TSharedPtr<::ULIS::FBlock> GetBlock(uint32 iFrameIndex) const override
    {
        TSharedPtr<FOdysseyAnimationLayerImageRasterCell> cell = mCell.Pin();
        if (!cell)
            return nullptr;

        if (!cell->mRasterBlock)
            return nullptr;

        return cell->mRasterBlock->IsBeingEdited() ? cell->mRasterBlock->GetUndoableBlock() : cell->mRasterBlock->GetBlock();
    }

    virtual FString GetFrameId(uint32 iFrameIndex) const override
    {
        TSharedPtr<FOdysseyAnimationLayerImageRasterCell> cell = mCell.Pin();
        if (!cell)
            return "";

        if (!cell->mRasterBlock)
            return "";

        return cell->mRasterBlock->GetId().ToString();
    }

public:
    TWeakPtr<FOdysseyAnimationLayerImageRasterCell> mCell;
};
  
class FOdysseyAnimationLayerImageRasterCell::FImageRasterEditingAbility
    : public FOdysseyAnimationImageRasterEditingAbility
{
public:
    FImageRasterEditingAbility(TSharedPtr<FOdysseyAnimationLayerImageRasterCell> iCell)
        : mCell(iCell)
    {
    }

public:
    virtual TSharedPtr<FOdysseyRasterBlock> GetRasterBlock(uint32 iFrameIndex) const override
    {
        TSharedPtr<FOdysseyAnimationLayerImageRasterCell> cell = mCell.Pin();
        if (!cell)
            return nullptr;

        return cell->mRasterBlock;
    }

public:
    TWeakPtr<FOdysseyAnimationLayerImageRasterCell> mCell;
};

#undef LOCTEXT_NAMESPACE