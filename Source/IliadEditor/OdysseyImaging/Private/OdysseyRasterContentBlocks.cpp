// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterContentBlocks.h"

#include "Engine/TextureRenderTarget2D.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyRasterBlockEdit.h"
#include "OdysseyRasterContent.h"
#include "OdysseyRasterEditSession.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"
#include "ULISLoaderModule.h"

namespace Odyssey::RasterContentBlocks
{
TSharedPtr<::ULIS::FBlock> ReadBlock(FOdysseyRasterContent& Content)
{
    if (!Content.EnsureImage())
    {
        return nullptr;
    }
    const FOdysseyRasterContent::FDescription Description = Content.Describe();
    return Odyssey::TiledImage::CopyToBlock(*Content.GetImage(), FIntRect(FIntPoint::ZeroValue, Description.Size), Description.Format, Description.bSRGB);
}

bool WriteBlock(FOdysseyRasterContent& Content, const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(Content.CreateRenderTarget());
    if (!Target)
    {
        return false;
    }

    // As the former raster block, convert the block to the expected format first: its 8-bit colors are then sRGB encoded when bSRGB is set.
    const FOdysseyRasterContent::FDescription Description = Content.Describe();
    const ::ULIS::FBlock* Pixels = &Block;
    TSharedPtr<::ULIS::FBlock> Converted;
    if (Block.Format() != Description.Format)
    {
        Converted = MakeShared<::ULIS::FBlock>(Block.Width(), Block.Height(), Description.Format);
        ::ULIS::FContext& Context = IULISLoaderModule::StaticFindOrAddContext(Description.Format);
        Context.ConvertFormat(Block, *Converted, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::MultiScanlines);
        Context.Finish();
        Pixels = Converted.Get();
    }

    const FIntRect BlockBounds(0, 0, Pixels->Width(), Pixels->Height());
    TArray<FIntRect> InBlock;
    InBlock.Reserve(Rects.Num());
    for (FIntRect Rect : Rects)
    {
        Rect.Clip(BlockBounds);
        if (Rect.Width() > 0 && Rect.Height() > 0)
        {
            InBlock.Add(Rect);
        }
    }
    if (InBlock.IsEmpty())
    {
        return false;
    }

    // The render target is in the tile format: the write copies its pixels as they are.
    if (!Odyssey::TiledImage::CopyBlockToRenderTarget(*Target, Content.GetImage()->GetFormat(), *Pixels, InBlock, Description.bSRGB))
    {
        return false;
    }
    return Content.Write(*Target, InBlock);
}

bool EditBlock(FOdysseyRasterContent& Content, TFunctionRef<void(::ULIS::FBlock& Block)> Edit)
{
    TSharedPtr<::ULIS::FBlock> Block = ReadBlock(Content);
    if (!Block)
    {
        return false;
    }
    Edit(*Block);
    return WriteBlock(Content, *Block, { FIntRect(0, 0, Block->Width(), Block->Height()) });
}

TSharedPtr<FOdysseyRasterBlockEdit> BeginBlockEdit(FOdysseyRasterContent& Content)
{
    // Read first: beginning the edit shows its target, which starts with the pixels of the tiles.
    TSharedPtr<::ULIS::FBlock> Pixels = ReadBlock(Content);
    if (!Pixels)
    {
        return nullptr;
    }
    TSharedPtr<FOdysseyRasterEditSession> Session = Content.BeginEdit();
    if (!Session)
    {
        return nullptr;
    }
    return MakeShared<FOdysseyRasterBlockEdit>(Session.ToSharedRef(), Pixels.ToSharedRef(), Content.GetImage()->GetFormat(), Content.Describe().bSRGB);
}
}
