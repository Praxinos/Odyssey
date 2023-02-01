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
{
}

void
FOdysseyPaintEngine::RasterBlock(TSharedPtr<FOdysseyRasterBlock> iRasterBlock)
{
    if (mRasterBlock == iRasterBlock)
        return;

    //Automatically commit the previous block
    Commit(mPreviousBlendParameters);
    if (!iRasterBlock)
    {
        mRasterBlock->OnEditableBlockChanged().RemoveAll(this);
        mRasterBlock->OnBlockPtrChanged().RemoveAll(this);

        mRasterBlock = nullptr;
        if (mPaintBlock)
            mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock());
        
        mEditedBlock = nullptr;
        mPaintBlock = nullptr;
        //mOriginalBlock = nullptr;
        return;
    }

    mRasterBlock = iRasterBlock;
    mRasterBlock->OnEditableBlockChanged().AddRaw(this, &::FOdysseyPaintEngine::OnEditedBlockChanged);
    mRasterBlock->OnBlockPtrChanged().AddRaw(this, &::FOdysseyPaintEngine::OnBlockPtrChanged);

    //Realloc to match RasterBlock size and format
    mEditedBlock = mRasterBlock->GetEditableBlock();
    
    if ( !mPaintBlock || mPaintBlock->Size() != mEditedBlock->Size() || mPaintBlock->Format() != mEditedBlock->Format())// ||
         //!mOriginalBlock || mOriginalBlock->Size() != mEditedBlock->Size() || mOriginalBlock->Format() != mEditedBlock->Format() )
    {
        mPaintBlock = MakeShared<::ULIS::FBlock>(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        //mOriginalBlock = MakeShared<::ULIS::FBlock>(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock( &FOdysseyPaintEngine::PaintBlockChanged, static_cast<void*>(this) ));

        //Create InvalidMaps
        int tileSize = 64; //Could be a config variable one day, or retrieved from mRasterBlock
        mInvalidMap = FULISInvalidTileMap(tileSize, mEditedBlock->Width(), mEditedBlock->Height());
        mUpdatedMap = FULISInvalidTileMap(tileSize, mEditedBlock->Width(), mEditedBlock->Height());
    }

    //Clear the paintblock before anything
    ClearPaintBlock();

    //Prepare the Original Block
    //CopyEditedBlockToOriginalBlock();
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
    if ( blendParameters != mPreviousBlendParameters )
        mInvalidMap.Invalidate(mUpdatedMap.InvalidTiles());

    //Update the EditedBlock content
    if (!UpdateEditedBlock(blendParameters))
        return;
}

void
FOdysseyPaintEngine::Commit(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mRasterBlock)
        return;

    //Update the EditedBlock content
    Update(iBlendParameters);

    //Refresh the Original block to match the EditedBlock
    //CopyEditedBlockToOriginalBlock();

    //Clear the Paint Block
    ClearPaintBlock();

    //Validate all the interactive modifications that has been done
    mRasterBlock->Invalidate({}, false);
}

void
FOdysseyPaintEngine::Abort()
{
    if (!mRasterBlock)
        return;

    //Clear the Paint Block
    ClearPaintBlock();

    //Restore the editedblock to its original state before edition
    //RestoreEditedBlock();
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
    paintEngine->mInvalidMap.Invalidate(rects);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyPaintEngine::PaintBlock()
{
    return mPaintBlock;
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

    mInvalidMap.Clear();
}

bool
FOdysseyPaintEngine::UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters)
{
    if( !mRasterBlock )
        return false;

    //Finish any pending operations before updating the blocks
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mEditedBlock->Format());
    ctx.Finish();
    if ( mInvalidMap.InvalidTiles().Num() <= 0 )
        return false;

    const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> original = mRasterBlock->GetBlock();
    TArray<::ULIS::FRectI> invalidRects = mInvalidMap.InvalidRects();

    //Blend Paint Block over OriginalBlock
    for( const ::ULIS::FRectI& rect : invalidRects )
    {
        ::ULIS::FEvent eventCopy;
        ctx.Copy( *original, *mEditedBlock, rect, rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventCopy );
        ctx.Flush();

        ::ULIS::FEvent eventBlend;
        ctx.Blend(*mPaintBlock, *mEditedBlock, rect, rect.Position(), ::ULIS::eBlendMode(iBlendParameters.BlendingMode), ::ULIS::eAlphaMode(iBlendParameters.AlphaMode), iBlendParameters.Opacity / 100.f, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventCopy );
    }    
    ctx.Finish();

    mRasterBlock->Invalidate(invalidRects, true);
    mUpdatedMap.Invalidate(mInvalidMap.InvalidTiles());
    mInvalidMap.Clear();
    return true;
}

void
FOdysseyPaintEngine::OnEditedBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    Abort();
}

void
FOdysseyPaintEngine::OnBlockPtrChanged()
{
    Abort();
}
