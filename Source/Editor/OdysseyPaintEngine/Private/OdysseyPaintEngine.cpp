// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaintEngine.h"

#include "OdysseyBlendParameters.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRectUtils.h"
#include "ULISLoaderModule.h"

#include <chrono>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
}

FOdysseyPaintEngine::FOdysseyPaintEngine()
    : mRasterBlock(nullptr)
    , mExpectsOnPixelsChanged(false)
{
}

void
FOdysseyPaintEngine::RasterBlock(UOdysseyRasterBlock* iRasterBlock)
{
    if (mRasterBlock == iRasterBlock)
        return;

    //Automatically commit the previous block
    Commit(mPreviousBlendParameters);
    if (!iRasterBlock)
    {
        mRasterBlock->OnPixelsChanged().RemoveAll(this);
        mRasterBlock->OnBlockChanged().RemoveAll(this);

        mRasterBlock = nullptr;
        if (mPaintBlock)
            mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock());
        
        mEditedBlock = nullptr;
        mPaintBlock = nullptr;
        mOriginalBlock = nullptr;
        return;
    }

    mRasterBlock = iRasterBlock;
    mRasterBlock->OnPixelsChanged().AddRaw(this, &::FOdysseyPaintEngine::OnPixelsChanged);
    mRasterBlock->OnBlockChanged().AddRaw(this, &::FOdysseyPaintEngine::OnBlockChanged);

    //Realloc to match RasterBlock size and format
    mEditedBlock = mRasterBlock->GetBlock();
    
    if ( !mPaintBlock || mPaintBlock->Size() != mEditedBlock->Size() || mPaintBlock->Format() != mEditedBlock->Format() ||
         !mOriginalBlock || mOriginalBlock->Size() != mEditedBlock->Size() || mOriginalBlock->Format() != mEditedBlock->Format() )
    {
        mPaintBlock = MakeShared<::ULIS::FBlock>(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        mOriginalBlock = MakeShared<::ULIS::FBlock>(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock( &FOdysseyPaintEngine::PaintBlockChanged, static_cast<void*>(this) ));
    }

    //Clear the paintblock before anything
    ClearPaintBlock();

    //Prepare the Original Block
    CopyEditedBlockToOriginalBlock();
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

    //Execute PreUpdate delegate to retrieve potentially overriden blendparameters
    if (mOnPreUpdateDelegate.IsBound())
        blendParameters = mOnPreUpdateDelegate.Execute(blendParameters);

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
    if (blendParameters != mPreviousBlendParameters)
        mInvalidRects.Append(mUpdatedRects);

    //Update the EditedBlock content
    if (!UpdateEditedBlock(blendParameters))
        return;

    //Clear the PaintBlock
    /* mUpdatedRects.Append(mInvalidRects);
    mOnUpdateDelegate.Broadcast(mInvalidRects);
    mInvalidRects.Empty(); */
}

void
FOdysseyPaintEngine::Commit(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mRasterBlock)
        return;

    //Update the EditedBlock content
    Update(iBlendParameters);

    //Refresh the Original block to match the EditedBlock
    CopyEditedBlockToOriginalBlock();

    //Clear the Paint Block
    ClearPaintBlock();

    //Validate all the interactive modifications that has been done
    mExpectsOnPixelsChanged = true;
    mRasterBlock->Commit();
    mExpectsOnPixelsChanged = false;

    //Inform the paintEngine has commited
    //mOnCommitDelegate.Broadcast(mUpdatedRects);
}

void
FOdysseyPaintEngine::Abort()
{
    if (!mRasterBlock)
        return;

    //Clear the Paint Block
    ClearPaintBlock();

    //Restore the editedblock to its original state before edition
    RestoreEditedBlock();

    //Set Invalid Tile Map, so that the EditedBlock can refresh the right tiles on the next call of Update()
    //TArray<::ULIS::FRectI> rects = mEditedBlockInvalidTileMap.InvalidRects();
    //mResetInvalidTileMap.Invalidate(rects);
    //mEditedBlockInvalidTileMap.Clear();
    //mPaintBlockInvalidTileMap.Clear();

    //UpdateEditedBlock(FOdysseyBlendParameters());

    //Refresh the Original block to match the EditedBlock
    //CopyEditedBlockToOriginalBlock();
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyPaintEngine::PaintBlock()
{
    return mPaintBlock;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyPaintEngine::OriginalBlock()
{
    return mOriginalBlock;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------Internal - Blocks Management

void
FOdysseyPaintEngine::ClearPaintBlock()
{
    if (!mRasterBlock)
        return;
    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mPaintBlock->Format());

    //Finish any pending operations before clearing the paint block
    ctx.Finish();
    ctx.Clear(*mPaintBlock);
    ctx.Finish();

    mInvalidRects.Empty();
}

void
FOdysseyPaintEngine::CopyEditedBlockToOriginalBlock()
{
    if (!mRasterBlock)
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( mEditedBlock->Format() );
    //Finish any pending operations before Copying the edited block
    ctx.Finish();
    ctx.Copy(*mEditedBlock, *mOriginalBlock);
    ctx.Finish();

    mUpdatedRects.Empty();
}

bool
FOdysseyPaintEngine::RestoreEditedBlock()
{
    if( !mRasterBlock )
        return false;

    //Finish any pending operations before updating the blocks
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mOriginalBlock->Format());
    ctx.Finish();

    if ( mUpdatedRects.Num() <= 0 )
        return false;

    mUpdatedRects = ::OdysseyRectUtils::ToNonOverlappingRects(mUpdatedRects);
    if (mUpdatedRects.Num() <= 0)
        return false;

    ::ULIS::FBlock& original = *mOriginalBlock;
    ::ULIS::FBlock& edited = *mEditedBlock;

    //Copy OriginalBlock into editedBlock
    for( const ::ULIS::FRectI& rect : mUpdatedRects )
    {
        ctx.Copy( original, edited, rect, rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient );
        ctx.Flush();
    }    
    ctx.Finish();

    mExpectsOnPixelsChanged = true;
    mRasterBlock->Update(mEditedBlock, mUpdatedRects, false);
    mExpectsOnPixelsChanged = false;
    mUpdatedRects.Empty();

    return true;
}

bool
FOdysseyPaintEngine::UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters)
{
    if( !mRasterBlock )
        return false;

    //Finish any pending operations before updating the blocks
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mOriginalBlock->Format());
    ctx.Finish();
    if ( mInvalidRects.Num() <= 0 )
        return false;

    mInvalidRects = ::OdysseyRectUtils::ToNonOverlappingRects(mInvalidRects);
    if (mInvalidRects.Num() <= 0)
        return false;

    mRasterBlock->Modify();

    ::ULIS::FBlock& original = *mOriginalBlock;
    ::ULIS::FBlock& edited = *mEditedBlock;
    ::ULIS::FBlock& paint = *mPaintBlock;

    //Blend Paint Block over OriginalBlock
    for( const ::ULIS::FRectI& rect : mInvalidRects )
    {
        ::ULIS::FEvent eventCopy;
        ctx.Copy( original, edited, rect, rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventCopy );
        ctx.Flush();

        ::ULIS::FEvent eventBlend;
        ctx.Blend( paint, edited, rect, rect.Position(), ::ULIS::eBlendMode(iBlendParameters.BlendingMode), ::ULIS::eAlphaMode(iBlendParameters.AlphaMode), iBlendParameters.Opacity / 100.f, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventCopy );
    }    
    ctx.Finish();

    mExpectsOnPixelsChanged = true;
    mRasterBlock->Update(mEditedBlock, mInvalidRects, true);
    mExpectsOnPixelsChanged = false;
    mUpdatedRects.Append(mInvalidRects);
    mInvalidRects.Empty();

    return true;
}

void
FOdysseyPaintEngine::OnPixelsChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    if (mExpectsOnPixelsChanged)
        return;

    Abort();
}

void
FOdysseyPaintEngine::OnBlockChanged()
{
    Abort();
}
