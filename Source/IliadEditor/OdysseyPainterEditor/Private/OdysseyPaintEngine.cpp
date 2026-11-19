// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaintEngine.h"

#include "Editor/TransBuffer.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"

#include "OdysseyInvalidTileMap.h"
#include "OdysseyRasterBlockEdit.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

namespace
{
/** Edits are composed by whole tiles, as the edit session commits them. */
constexpr int32 PaintTileSize = 64;
}

FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
    UnbindBeforeRedoUndo();
    if (mEdit)
    {
        mEdit->End();
    }
}

FOdysseyPaintEngine::FOdysseyPaintEngine()
    : mIsBeforeUndoBound(false)
{
}

void
FOdysseyPaintEngine::Edit(TSharedPtr<FOdysseyRasterBlockEdit> iEdit)
{
    if (mEdit == iEdit)
        return;

    //Automatically commit the previous edit
    Commit(mPreviousBlendParameters);

    if (!iEdit)
    {
        if (mPaintBlock)
            mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock());

        mPaintBlock = nullptr;
        mInvalidRects.Empty();
        return;
    }

    mEdit = iEdit;
    const ::ULIS::FBlock& editedBlock = mEdit->GetBlock().Get();
    if ( !mPaintBlock || mPaintBlock->Width() != editedBlock.Width() || mPaintBlock->Height() != editedBlock.Height() || mPaintBlock->Format() != editedBlock.Format() )
    {
        mPaintBlock = MakeShared<::ULIS::FBlock>(editedBlock.Width(), editedBlock.Height(), editedBlock.Format());
        mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock( &FOdysseyPaintEngine::PaintBlockChanged, static_cast<void*>(this) ));
    }

    //Clear the paintblock before anything
    ClearPaintBlock();
}

void
FOdysseyPaintEngine::SetMaskBlock(TSharedPtr<::ULIS::FBlock> iMaskBlock)
{
    mMaskBlock = iMaskBlock;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyPaintEngine::PaintBlock()
{
    return mPaintBlock;
}

TSharedPtr<FOdysseyRasterBlockEdit>
FOdysseyPaintEngine::GetEdit() const
{
    return mEdit;
}

void
FOdysseyPaintEngine::Update(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mEdit)
        return;

    if (!mEdit->IsActive())
    {
        //The edit ended by itself, e.g. another edit of the content began or the content was undone
        Abort();
        return;
    }

    //If blend parameters are different from the previous one used
    //Force refreshing all edited tiles, instead of just newly edited tiles
    if ( iBlendParameters != mPreviousBlendParameters )
        mInvalidRects.Append(::ULISUtils::ToULISRectIs(mEdit->GetEditedRects()));

    UpdateEditedBlock(iBlendParameters);
    mPreviousBlendParameters = iBlendParameters;
}

bool
FOdysseyPaintEngine::Commit(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mEdit)
        return false;

    Update(iBlendParameters);
    if (!mEdit)
        return false;

    UnbindBeforeRedoUndo();
    ClearPaintBlock();

    TSharedPtr<FOdysseyRasterBlockEdit> edit = MoveTemp(mEdit);
    return edit->Commit();
}

void
FOdysseyPaintEngine::Reset()
{
    ClearPaintBlock();
    if (mEdit)
        mInvalidRects.Append(::ULISUtils::ToULISRectIs(mEdit->GetEditedRects()));
}

void
FOdysseyPaintEngine::Abort()
{
    if (!mEdit)
        return;

    UnbindBeforeRedoUndo();
    ClearPaintBlock();
    ReleaseEdit();
}

//static
void
FOdysseyPaintEngine::PaintBlockChanged( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo )
{
    FOdysseyPaintEngine* paintEngine = static_cast< FOdysseyPaintEngine* >( iInfo );

    //Remember the changed areas, so that the next call of Update() composes them
    TArray<::ULIS::FRectI> rects(iRects, iNumRects);
    paintEngine->mInvalidRects.Append(rects);
}

void
FOdysseyPaintEngine::ClearPaintBlock()
{
    if (!mPaintBlock)
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mPaintBlock->Format());

    //Finish any pending operations before clearing the paint block
    ctx.Finish();
    ctx.Clear(*mPaintBlock);
    ctx.Finish();

    mInvalidRects.Empty();
}

void
FOdysseyPaintEngine::ReleaseEdit()
{
    TSharedPtr<FOdysseyRasterBlockEdit> edit = MoveTemp(mEdit);
    if (edit)
        edit->End();
}

bool
FOdysseyPaintEngine::UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters)
{
    const TSharedRef<::ULIS::FBlock>& originalBlock = mEdit->GetOriginalBlock();
    const TSharedRef<::ULIS::FBlock>& editedBlock = mEdit->GetBlock();

    //Finish any pending operations before updating the blocks
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(editedBlock->Format());
    ctx.Finish();

    if ( mInvalidRects.IsEmpty() )
        return false;

    BindBeforeRedoUndo();

    //Compose whole tiles, as the edit session commits them
    FOdysseyInvalidTileMap tileMap(PaintTileSize, editedBlock->Width(), editedBlock->Height());
    tileMap.Invalidate(::ULISUtils::ToIntRects(mInvalidRects));
    mInvalidRects.Empty();
    const TArray<FIntRect> invalidRects = tileMap.InvalidRects();
    if (invalidRects.IsEmpty())
        return false;

    ::ULIS::eBlendMode blendMode;
    ::ULIS::eAlphaMode alphaMode;
    ULISUtils::GetULISBlendingModeFromBlendMode(iBlendParameters.GetColorBlendMode(), iBlendParameters.GetAlphaBlendMode(), blendMode, alphaMode);

    TArray<::ULIS::FEvent> eventMask;
    if (mMaskBlock)
    {
        ::ULIS::FEvent eventFilter = FULISEventBuilder().RetainBlock(mMaskBlock).RetainBlock(mPaintBlock).Build();
        ctx.FilterInto(
            []( const ::ULIS::FPixel& iSrcPixel,  ::ULIS::FPixel& iDstPixel, uint64 iNumPixels )
            {
                for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                {
                    iDstPixel.SetAlphaF(iDstPixel.AlphaF() * iSrcPixel.GreyF());
                }
            }
            , *mMaskBlock
            , *mPaintBlock
            , ::ULIS::FRectI::Auto
            , ::ULIS::FVec2I(0)
            , ::ULIS::FSchedulePolicy::MultiScanlines
            , 0
            , nullptr
            , &eventFilter
        );
        eventMask.Add(eventFilter);
    }

    for ( const FIntRect& invalidRect : invalidRects )
    {
        const ::ULIS::FRectI rect = ::ULISUtils::ToULISRectI(invalidRect);

        //Restore the original pixels, then blend the stroke over them
        ::ULIS::FEvent copyEvent;
        ctx.Copy(*originalBlock, *editedBlock, rect, rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, eventMask.Num(), eventMask.GetData(), &copyEvent);
        ctx.Blend(*mPaintBlock, *editedBlock, rect, rect.Position(), blendMode, alphaMode, iBlendParameters.GetOpacity() / 100.f, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &copyEvent, nullptr);
    }
    ctx.Finish();

    mEdit->Update(invalidRects);
    return true;
}

void
FOdysseyPaintEngine::BindBeforeRedoUndo()
{
    if (mIsBeforeUndoBound || !GUnrealEd)
        return;

    UTransBuffer* transBuffer = Cast<UTransBuffer>(GUnrealEd->Trans);
    if ( transBuffer )
        transBuffer->OnBeforeRedoUndo().AddRaw(this, &FOdysseyPaintEngine::OnBeforeRedoUndo);

    mIsBeforeUndoBound = true;
}

void
FOdysseyPaintEngine::UnbindBeforeRedoUndo()
{
    if (!mIsBeforeUndoBound || !GUnrealEd)
        return;

    UTransBuffer* transBuffer = Cast<UTransBuffer>(GUnrealEd->Trans);
    if ( transBuffer )
        transBuffer->OnBeforeRedoUndo().RemoveAll(this);

    mIsBeforeUndoBound = false;
}

void
FOdysseyPaintEngine::OnBeforeRedoUndo( const FTransactionContext& /*TransactionContext*/ )
{
    Abort();
}
