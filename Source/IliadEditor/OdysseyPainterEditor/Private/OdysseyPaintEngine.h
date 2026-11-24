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
 *
 * Lifecycle:
 * - BeginEdit(): starts painting into an edit of a content.
 * - DiscardStroke(): cancels the uncommitted changes; the edit stays active.
 * - Commit(): writes the stroke into the content (undoable) and ends the edit; the paint block is kept for the next stroke.
 * - EndEdit(): stops editing completely: cancels the uncommitted changes, ends the edit and frees the paint block.
 *   Tools call it when they are unloaded.
 * An edit that ends by itself (another edit of the content began, undo or redo) is discarded at the next Update().
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPaintEngine
{
public:
    /** Ends the edit, if any. */
    virtual ~FOdysseyPaintEngine();

    FOdysseyPaintEngine();

public:
    /**
     * Starts painting into an edit. A previous edit, if any, ends without writing anything: tools commit their edits explicitly.
     * The paint block is cleared, and sized and formatted as the edited block.
     */
    void BeginEdit(TSharedRef<FOdysseyRasterBlockEdit> iEdit);

    /** Sets the selection mask: only its selected pixels are painted. Null paints everywhere. */
    void SetMaskBlock(TSharedPtr<::ULIS::FBlock> iMaskBlock);

    /** Returns the block tools draw the stroke into; null before the first edit and after EndEdit(). */
    TSharedPtr<::ULIS::FBlock> PaintBlock();

    /** Returns the edit painted into; null once committed or ended. */
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

    /** Cancels the uncommitted changes and keeps the edit: the next update shows the original pixels again, e.g. to draw the stroke again. */
    void DiscardStroke();

    /** Stops editing completely: cancels the uncommitted changes, ends the edit (the views show the content again) and frees the paint block. */
    void EndEdit();

private:
    static void PaintBlockChanged( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo );

    void ClearPaintBlock();

    /** Ends the edit without writing it and keeps the paint block, which tools may still hold during a stroke. */
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
