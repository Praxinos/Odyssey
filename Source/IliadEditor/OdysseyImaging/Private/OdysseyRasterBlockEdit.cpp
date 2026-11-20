// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterBlockEdit.h"

#include "Engine/TextureRenderTarget2D.h"

#include "OdysseyRasterEditSession.h"
#include "OdysseyTiledImageBlock.h"

namespace
{
TSharedRef<::ULIS::FBlock> CopyBlock(const ::ULIS::FBlock& Source)
{
    TSharedRef<::ULIS::FBlock> Copy = MakeShared<::ULIS::FBlock>(Source.Width(), Source.Height(), Source.Format());
    FMemory::Memcpy(Copy->Bits(), Source.Bits(), Source.BytesTotal());
    return Copy;
}
}

FOdysseyRasterBlockEdit::~FOdysseyRasterBlockEdit()
{
    End();
}

FOdysseyRasterBlockEdit::FOdysseyRasterBlockEdit(TSharedRef<FOdysseyRasterEditSession> InSession, TSharedRef<::ULIS::FBlock> InOriginal,
    EOdysseyTiledImageFormat InTileFormat, bool bInSRGB)
    : Session(MoveTemp(InSession))
    , Original(InOriginal)
    , Block(CopyBlock(*InOriginal))
    , TileFormat(InTileFormat)
    , bSRGB(bInSRGB)
    , EditedTiles(TileSize, InOriginal->Width(), InOriginal->Height())
{
}

const TSharedRef<::ULIS::FBlock>&
FOdysseyRasterBlockEdit::GetOriginalBlock() const
{
    return Original;
}

const TSharedRef<::ULIS::FBlock>&
FOdysseyRasterBlockEdit::GetBlock() const
{
    return Block;
}

const FOdysseyRasterEditSession&
FOdysseyRasterBlockEdit::GetSession() const
{
    return *Session;
}

bool
FOdysseyRasterBlockEdit::IsActive() const
{
    return Session->IsActive();
}

TArray<FIntRect>
FOdysseyRasterBlockEdit::GetEditedRects() const
{
    return EditedTiles.InvalidRects();
}

void
FOdysseyRasterBlockEdit::Update(const TArray<FIntRect>& Rects)
{
    if (!IsActive())
    {
        return;
    }

    const FIntRect Bounds(0, 0, Block->Width(), Block->Height());
    TArray<FIntRect> Clipped;
    Clipped.Reserve(Rects.Num());
    for (FIntRect Rect : Rects)
    {
        Rect.Clip(Bounds);
        if (Rect.Width() > 0 && Rect.Height() > 0)
        {
            Clipped.Add(Rect);
        }
    }
    if (Clipped.IsEmpty())
    {
        return;
    }

    Odyssey::TiledImage::CopyBlockToRenderTarget(*Session->GetTarget(), TileFormat, *Block, Clipped, bSRGB);
    EditedTiles.Invalidate(Clipped);
    Session->Invalidate(Clipped);
}

bool
FOdysseyRasterBlockEdit::Commit()
{
    const TArray<FIntRect> Rects = GetEditedRects();
    if (!IsActive() || Rects.IsEmpty())
    {
        End();
        return false;
    }

    // The session target already holds every updated area: the tiles are written from it.
    return Session->Commit(Rects);
}

void
FOdysseyRasterBlockEdit::End()
{
    Session->End();
}
