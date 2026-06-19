// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaintEngine.h"

#include "OdysseyBlendParameters.h"
#include "OdysseyRasterBlock.h"
#include "ULISLoaderModule.h"
#include "Editor/TransBuffer.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "ULISEventBuilder.h"
#include "ULISUtils.h"

#include <chrono>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
}

FOdysseyPaintEngine::FOdysseyPaintEngine()
    : mRasterBlock(nullptr)
    , mIsBeforeUndoBound(false)
{
}

void
FOdysseyPaintEngine::RasterBlock(TSharedPtr<FOdysseyRasterBlock> iRasterBlock)
{
    if (mRasterBlock == iRasterBlock)
        return;

    //Automatically commit the previous block
    Commit(mPreviousBlendParameters);

    if (mRasterBlock)
        mRasterBlock->OnBlockChanged().RemoveAll(this);

    if (!iRasterBlock)
    {
        mRasterBlockMutator.SetRasterBlock(nullptr);
        mRasterBlock = nullptr;
        if (mPaintBlock)
            mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock());

        mPaintBlock = nullptr;
        mInvalidRects.Empty();
        return;
    }

    mRasterBlock = iRasterBlock;
    mRasterBlock->OnBlockChanged().AddRaw(this, &FOdysseyPaintEngine::OnRasterBlockChanged);
    mRasterBlockMutator.SetRasterBlock(iRasterBlock);

    if ( !mPaintBlock || mPaintBlock->Width() != mRasterBlock->GetWidth() || mPaintBlock->Height() != mRasterBlock->GetHeight() || mPaintBlock->Format() != mRasterBlock->GetFormat() )
    {
        mPaintBlock = MakeShared<::ULIS::FBlock>(mRasterBlock->GetWidth(), mRasterBlock->GetHeight(), mRasterBlock->GetFormat());
        mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock( &FOdysseyPaintEngine::PaintBlockChanged, static_cast<void*>(this) ));
    }

    //Clear the paintblock before anything
    ClearPaintBlock();
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyPaintEngine::GetRasterBlock() const
{
    return mRasterBlock;
}

void
FOdysseyPaintEngine::SetMaskBlock(TSharedPtr<::ULIS::FBlock> iMaskBlock)
{
    mMaskBlock = iMaskBlock;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Paint Engine API

FOdysseyBlendParameters
FOdysseyPaintEngine::AdjustBlendParameters(const FOdysseyBlendParameters& iBlendParameters)
{
    FOdysseyBlendParameters blendParameters = iBlendParameters;
    if (blendParameters.bEraserMode)
    {
        blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::eAlphaMode::Alpha_Erase);
        blendParameters.BlendingMode = EOdysseyBlendingMode(::ULIS::eBlendMode::Blend_Back);
    }

    return blendParameters;
}

void
FOdysseyPaintEngine::Update(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mRasterBlock)
        return;

    //Apply bEraserMode if active
    FOdysseyBlendParameters blendParameters = AdjustBlendParameters(iBlendParameters);

    //If blend parameters are different from the previous one used
    //Force refreshing all edited tiles, instead of just newly edited tiles
    if ( blendParameters != mPreviousBlendParameters )
        mInvalidRects.Append(::ULISUtils::ToULISRectIs(mRasterBlockMutator.GetInvalidTileMap().InvalidRects()));

    //Update the EditedBlock content
    if (!UpdateEditedBlock(blendParameters))
        return;
}

void
FOdysseyPaintEngine::Commit(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mRasterBlock)
        return;

    if ( mIsBeforeUndoBound && GUnrealEd )
    {
        UTransBuffer* transBuffer = Cast<UTransBuffer>(GUnrealEd->Trans);
        if ( transBuffer )
            transBuffer->OnBeforeRedoUndo().RemoveAll(this);

        mIsBeforeUndoBound = false;
    }

    //Update the EditedBlock content
    Update(iBlendParameters);

    //Clear the Paint Block
    ClearPaintBlock();

    //Validate all the interactive modifications that has been done
    mIsChangingBlock = true;
    mRasterBlockMutator.Commit();
    mIsChangingBlock = false;
}

void
FOdysseyPaintEngine::Abort()
{
    if (!mRasterBlock)
        return;

    if ( mIsBeforeUndoBound && GUnrealEd )
    {
        UTransBuffer* transBuffer = Cast<UTransBuffer>(GUnrealEd->Trans);
        if ( transBuffer )
            transBuffer->OnBeforeRedoUndo().RemoveAll(this);

        mIsBeforeUndoBound = false;
    }

    //Clear the Paint Block
    ClearPaintBlock();

    mIsChangingBlock = true;
    mRasterBlockMutator.Abort();
    mIsChangingBlock = false;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

//static
void
FOdysseyPaintEngine::PaintBlockChanged( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo )
{
    FOdysseyPaintEngine* paintEngine = static_cast< FOdysseyPaintEngine* >( iInfo );

    //Set Invalid Tile Map, so that the EditedBlock can refresh the right tiles on the next call of Update()
    TArray<::ULIS::FRectI> rects(iRects, iNumRects);
    paintEngine->mInvalidRects.Append(rects);
}

void
FOdysseyPaintEngine::OnRasterBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    if (mIsChangingBlock)
        return;

    Abort();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<::ULIS::FBlock>
FOdysseyPaintEngine::PaintBlock()
{
    return mPaintBlock;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------Internal - Blocks Management

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

bool
FOdysseyPaintEngine::UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters)
{
    if( !mRasterBlock )
        return false;

    //Finish any pending operations before updating the blocks
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mRasterBlock->GetFormat());
    ctx.Finish();

    if ( mInvalidRects.IsEmpty() )
        return false;

    if (!mIsBeforeUndoBound && GUnrealEd )
    {
        UTransBuffer* transBuffer = Cast<UTransBuffer>(GUnrealEd->Trans);
        if ( transBuffer )
            transBuffer->OnBeforeRedoUndo().AddRaw(this, &FOdysseyPaintEngine::OnBeforeRedoUndo);

        mIsBeforeUndoBound = true;
    }

    mIsChangingBlock = true;
    mRasterBlockMutator.ResetTilesFromRects(mInvalidRects);
    mRasterBlockMutator.EditTilesFromRects(
        mInvalidRects,
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap)
        {
            TArray<::ULIS::FRectI> invalidRects = ::ULISUtils::ToULISRectIs(iTileMap.InvalidRects());

            //Blend Paint Block over OriginalBlock
            TArray<::ULIS::FEvent> events;
            for ( const ::ULIS::FRectI& rect : invalidRects )
            {
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


                ::ULIS::FEvent blendEvent;
                ctx.Blend(*mPaintBlock, *iBlock, rect, rect.Position(), ::ULIS::eBlendMode(iBlendParameters.BlendingMode), ::ULIS::eAlphaMode(iBlendParameters.AlphaMode), iBlendParameters.Opacity / 100.f, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, eventMask.Num(), eventMask.GetData(), &blendEvent);
                events.Add(blendEvent);
            }
            return events;
        }
    );
    mIsChangingBlock = false;
    mInvalidRects.Empty();
    return true;
}

void
FOdysseyPaintEngine::OnBeforeRedoUndo( const FTransactionContext& /*TransactionContext*/ )
{
    Abort();
}
