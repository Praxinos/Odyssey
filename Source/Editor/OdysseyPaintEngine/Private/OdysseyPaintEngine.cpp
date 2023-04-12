// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaintEngine.h"

#include "OdysseyBlendParameters.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRectUtils.h"
#include "ULISLoaderModule.h"
#include "Editor/TransBuffer.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

#include <chrono>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
    if (GUnrealEd)
    {
        UTransBuffer* transBuffer = Cast<UTransBuffer>(GUnrealEd->Trans);
        if (transBuffer)
            transBuffer->OnBeforeRedoUndo().RemoveAll(this);
    }
}

FOdysseyPaintEngine::FOdysseyPaintEngine()
    : mRasterBlock(nullptr)
{
    if (GUnrealEd)
    {
        UTransBuffer* transBuffer = Cast<UTransBuffer>(GUnrealEd->Trans);
        if (transBuffer)
            transBuffer->OnBeforeRedoUndo().AddRaw(this, &FOdysseyPaintEngine::OnBeforeRedoUndo);
    }
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
        mRasterBlockMutator.SetRasterBlock(nullptr);
        //mRasterBlock->OnUndoableBlockChanged().RemoveAll(this);
        mRasterBlock->OnBlockPtrChanged().RemoveAll(this);
        mRasterBlock = nullptr;
        if (mPaintBlock)
            mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock());
        
        //mEditedBlock = nullptr;
        mPaintBlock = nullptr;
        mInvalidRects.Empty();
        //mOriginalBlock = nullptr;
        return;
    }

    mRasterBlock = iRasterBlock;
    mRasterBlockMutator.SetRasterBlock(iRasterBlock);
    //mRasterBlock->OnUndoableBlockChanged().AddRaw(this, &::FOdysseyPaintEngine::OnEditedBlockChanged);
    mRasterBlock->OnBlockPtrChanged().AddRaw(this, &::FOdysseyPaintEngine::OnBlockPtrChanged);

    //Realloc to match RasterBlock size and format
    //mEditedBlock = mRasterBlock->GetUndoableBlock();
    
    if ( !mPaintBlock || mPaintBlock->Width() != mRasterBlock->GetWidth() || mPaintBlock->Height() != mRasterBlock->GetHeight() || mPaintBlock->Format() != mRasterBlock->GetFormat() )// ||
    //if ( !mPaintBlock || mPaintBlock->Size() != mEditedBlock->Size() || mPaintBlock->Format() != mEditedBlock->Format())// ||
         //!mOriginalBlock || mOriginalBlock->Size() != mEditedBlock->Size() || mOriginalBlock->Format() != mEditedBlock->Format() )
    {
        mPaintBlock = MakeShared<::ULIS::FBlock>(mRasterBlock->GetWidth(), mRasterBlock->GetHeight(), mRasterBlock->GetFormat());
        //mOriginalBlock = MakeShared<::ULIS::FBlock>(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock( &FOdysseyPaintEngine::PaintBlockChanged, static_cast<void*>(this) ));

        //Create InvalidMaps
        //int tileSize = 64; //Could be a config variable one day, or retrieved from mRasterBlock
        //mInvalidMap = FULISInvalidTileMap(tileSize, mEditedBlock->Width(), mEditedBlock->Height());
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
        mInvalidRects.Append(mRasterBlockMutator.GetInvalidTileMap().InvalidRects());
        //mInvalidMap.Invalidate(mRasterBlockMutator.GetInvalidTileMap().InvalidTiles());

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

    //Clear the Paint Block
    ClearPaintBlock();

    //Validate all the interactive modifications that has been done
    mRasterBlockMutator.Commit();
    //mRasterBlock->CommitUndoableBlock();
}

void
FOdysseyPaintEngine::Abort()
{
    if (!mRasterBlock)
        return;

    //Clear the Paint Block
    ClearPaintBlock();

    mRasterBlockMutator.Abort();
    //mRasterBlock->ResetUndoableBlock();
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
    //paintEngine->mInvalidMap.Invalidate(rects);
    paintEngine->mInvalidRects.Append(rects);
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

    mRasterBlockMutator.ResetTilesFromRects(mInvalidRects);
    mRasterBlockMutator.EditTilesFromRects(
        mInvalidRects,
        FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
            [&](const FULISInvalidTileMap& iTileMap)
            {
                const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = mRasterBlock->GetBlock();
                TArray<::ULIS::FRectI> invalidRects = iTileMap.InvalidRects();

                //Blend Paint Block over OriginalBlock
                for ( const ::ULIS::FRectI& rect : invalidRects )
                {
                    ctx.Blend(*mPaintBlock, *block, rect, rect.Position(), ::ULIS::eBlendMode(iBlendParameters.BlendingMode), ::ULIS::eAlphaMode(iBlendParameters.AlphaMode), iBlendParameters.Opacity / 100.f, ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
                }
                ctx.Finish();
            }
        )
    );
    mInvalidRects.Empty();
    return true;
}

void
FOdysseyPaintEngine::OnBeforeRedoUndo( const FTransactionContext& /*TransactionContext*/ )
{
    Abort();
}

/*
void
FOdysseyPaintEngine::OnEditedBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    Abort();
}
*/

void
FOdysseyPaintEngine::OnBlockPtrChanged()
{
    Abort();
}
