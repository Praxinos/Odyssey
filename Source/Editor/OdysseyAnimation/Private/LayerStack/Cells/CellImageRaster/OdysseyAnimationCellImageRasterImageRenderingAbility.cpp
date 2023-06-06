// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderingAbility.h"

#include "Misc/OdysseyHandle.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"



FOdysseyAnimationCellImageRasterImageRenderingAbility::~FOdysseyAnimationCellImageRasterImageRenderingAbility()
{
    mRasterBlock->OnBlockChanged().RemoveAll(this);
    mRasterBlock->OnBlockCommited().RemoveAll(this);
    mRasterBlock->OnBlockPtrChanged().RemoveAll(this);

    mRasterBlock = nullptr;
}

FOdysseyAnimationCellImageRasterImageRenderingAbility::FOdysseyAnimationCellImageRasterImageRenderingAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster)
    : mCellImageRaster(iCellImageRaster)
    , mRasterBlock(iCellImageRaster->GetRasterBlock())
{
    mRasterBlock->OnBlockChanged().AddRaw(this, &FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockChanged);
    mRasterBlock->OnBlockCommited().AddRaw(this, &FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockCommited);
    mRasterBlock->OnBlockPtrChanged().AddRaw(this, &FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockPtrChanged);
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationCellImageRasterImageRenderingAbility::BuildRenderer(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TSharedPtr<IOdysseyImageRenderer> renderer = MakeShared<FOdysseyAnimationCellImageRasterImageRenderer>(mCellImageRaster.Pin(), iFrame, iRenderType, GetRects());
    renderer->AddHandle(Preload(iFrame, iRenderType));
    return renderer;
}

TArray<FGuid>
FOdysseyAnimationCellImageRasterImageRenderingAbility::GetComposition(int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return { GetId() };
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationCellImageRasterImageRenderingAbility::Preload(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cellImageRaster = mCellImageRaster.Pin();
    if (!cellImageRaster)
        return nullptr;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = cellImageRaster->GetRasterBlock();
    if (!rasterBlock)
        return nullptr;

    TArray<TSharedPtr<IOdysseyHandle>> handles = { rasterBlock->Preload() };

    return MakeShared<FOdysseyHandleContainer>(handles);
}

TArray<::ULIS::FRectI>
FOdysseyAnimationCellImageRasterImageRenderingAbility::GetRects() const
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cellImageRaster = mCellImageRaster.Pin();
    if (!cellImageRaster)
        return {};

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = cellImageRaster->GetRasterBlock();
    if (!rasterBlock)
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) };
}

void
FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    Changed(iRects);
}

void
FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    Commited(iRects);
}

void
FOdysseyAnimationCellImageRasterImageRenderingAbility::OnBlockPtrChanged()
{
    Changed();
    Commited();
}