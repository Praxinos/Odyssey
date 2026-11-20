// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

#include "OdysseyInvalidTileMap.h"
#include "OdysseyTiledImage.h"

#include <ULIS>

class FOdysseyRasterEditSession;

/**
 * Transitional: an edit of a raster content through CPU blocks, for tools that do not edit render targets yet.
 *
 * The edit holds the pixels of the content at its beginning and a working copy, both in the format the content owner expects.
 * The tool writes into the working block, then:
 * - Update() uploads changed areas into the edit session, which the views show in place of the content;
 * - Commit() writes every edited area into the tiled image, undoable within a transaction;
 * - End() discards the edit.
 *
 * Pending ULIS operations on the working block must be finished before calling Update() or Commit().
 * The edit ends by itself when its session ends, e.g. when another edit of the same content begins or the content is undone.
 * Remove it with the CPU tools: an edit of a render target is an FOdysseyRasterEditSession.
 */
class ODYSSEYIMAGING_API FOdysseyRasterBlockEdit
{
public:
    /** Ends the edit, if not committed or ended yet. */
    ~FOdysseyRasterBlockEdit();

    /**
     * Use Odyssey::RasterContentBlocks::BeginBlockEdit().
     * @param InSession The session showing the edit, holding pixels in the tile format.
     * @param InOriginal The pixels of the content at the beginning of the edit; the working block starts as a copy of it.
     * @param InTileFormat The format of the session pixels.
     * @param bInSRGB True when the 8-bit blocks hold sRGB encoded colors, decoded when uploaded into the session.
     */
    FOdysseyRasterBlockEdit(TSharedRef<FOdysseyRasterEditSession> InSession, TSharedRef<::ULIS::FBlock> InOriginal,
        EOdysseyTiledImageFormat InTileFormat, bool bInSRGB);

    FOdysseyRasterBlockEdit(const FOdysseyRasterBlockEdit&) = delete;
    FOdysseyRasterBlockEdit& operator=(const FOdysseyRasterBlockEdit&) = delete;

public:
    /**
     * Returns the pixels the tool composes its changes with: the pixels of the content at the beginning of the edit.
     * A tool may change them, e.g. to remove the pixels it moves; only the working block is shown and committed.
     */
    const TSharedRef<::ULIS::FBlock>& GetOriginalBlock() const;

    /** Returns the working block the tool writes into. */
    const TSharedRef<::ULIS::FBlock>& GetBlock() const;

    /** Returns the session showing the edit. */
    const FOdysseyRasterEditSession& GetSession() const;

    /** Returns false once committed or ended. */
    bool IsActive() const;

    /** Returns the areas updated since the beginning of the edit, as whole tiles clipped to the content bounds. */
    TArray<FIntRect> GetEditedRects() const;

    /**
     * Uploads areas of the working block into the session and tells the views they changed.
     * @param Rects The changed areas, in content pixels; clipped to the content bounds.
     */
    void Update(const TArray<FIntRect>& Rects);

    /**
     * Writes every edited area into the tiled image, undoable within a transaction, then ends the edit.
     * @return False when nothing was edited, or the edit already ended; the edit is ended anyway.
     */
    bool Commit();

    /** Ends the edit: its pixels are discarded and the views show the content again. */
    void End();

public:
    /** The size of the tiles an edit tracks and commits, in pixels: editors compose whole tiles. */
    static constexpr int32 TileSize = 64;

private:
    TSharedRef<FOdysseyRasterEditSession> Session;
    TSharedRef<::ULIS::FBlock> Original;
    TSharedRef<::ULIS::FBlock> Block;
    EOdysseyTiledImageFormat TileFormat;
    bool bSRGB;
    FOdysseyInvalidTileMap EditedTiles;
};
