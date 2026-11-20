// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/ITransaction.h"

#include "OdysseyBlendParameters.h"

#include <ULIS>

class FOdysseyRasterBlockEdit;

/**
 * Paints strokes into an edit of a raster content.
 *
 * Tools draw into the paint block, which holds the stroke only. Update() composes the original pixels of the edit with
 * the stroke into its working block, for the areas the stroke changed, and shows them through the edit session.
 * Commit() writes the edit into the content; Abort() discards it.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPaintEngine
{
public:
    /** Ends the edit, if any. */
    virtual ~FOdysseyPaintEngine();

    FOdysseyPaintEngine();

public:
    /**
     * Sets the edit to paint into; the previous edit, if any, is aborted: tools commit their edits explicitly.
     * The paint block is cleared, and sized and formatted as the edited block.
     * @param iEdit The edit, or null to stop painting.
     */
    void Edit(TSharedPtr<FOdysseyRasterBlockEdit> iEdit);

    /** Sets the selection mask: only its selected pixels are painted. Null paints everywhere. */
    void SetMaskBlock(TSharedPtr<::ULIS::FBlock> iMaskBlock);

    /** Returns the block tools draw the stroke into; null without an edit. */
    TSharedPtr<::ULIS::FBlock> PaintBlock();

    /** Returns the edit painted into; null once committed or aborted. */
    TSharedPtr<FOdysseyRasterBlockEdit> GetEdit() const;

    /**
     * Composes the original pixels with the stroke, for the areas the stroke changed since the previous update, and shows them.
     * When the blend parameters changed, every edited area is composed again.
     * When the edit ended by itself, e.g. another edit of the content began, the stroke is discarded.
     */
    void Update(const FOdysseyBlendParameters& iBlendParameters);

    /**
     * Updates, then writes the edit into the content, undoable within a transaction, and stops painting into it.
     * @return False when nothing was committed.
     */
    bool Commit(const FOdysseyBlendParameters& iBlendParameters);

    /** Discards the stroke and keeps the edit: the next update shows the original pixels again, e.g. to draw the stroke again. */
    void Reset();

    /** Discards the edit and the stroke: the views show the content again. */
    void Abort();

private:
    static void PaintBlockChanged( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo );

    void ClearPaintBlock();
    void ReleaseEdit();

    /** Composes the original pixels with the stroke into the working block, for the invalid areas. */
    bool UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters);

    void BindBeforeRedoUndo();
    void UnbindBeforeRedoUndo();
    void OnBeforeRedoUndo( const FTransactionContext& TransactionContext );

private:
    TSharedPtr<FOdysseyRasterBlockEdit> mEdit;

    /** The stroke only, as large as the edited block. */
    TSharedPtr<::ULIS::FBlock> mPaintBlock;
    TSharedPtr<::ULIS::FBlock> mMaskBlock;

    FOdysseyBlendParameters mPreviousBlendParameters;

    /** The areas of the paint block changed since the previous update. */
    TArray<::ULIS::FRectI> mInvalidRects;
    bool mIsBeforeUndoBound;
};
