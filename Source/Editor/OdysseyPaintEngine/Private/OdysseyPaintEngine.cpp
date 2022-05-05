// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaintEngine.h"

#include "OdysseyBlendParameters.h"
#include "ULISLoaderModule.h"

#include <chrono>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
    delete mOriginalBlock;
    delete mPaintBlock;
}

FOdysseyPaintEngine::FOdysseyPaintEngine()
    //Blocks
    : mEditedBlock( nullptr )
    , mPaintBlock( nullptr )
	, mOriginalBlock( nullptr )

    //Internal
    , mPaintBlockInvalidTileMap()
    , mEditedBlockInvalidTileMap()
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Paint Engine API

void
FOdysseyPaintEngine::Update(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mEditedBlock)
        return;

    //Apply bEraserMode if active
    FOdysseyBlendParameters blendParameters = iBlendParameters;
    if (blendParameters.bEraserMode)
    {
        blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::eAlphaMode::Alpha_Erase);
        blendParameters.BlendingMode = EOdysseyBlendingMode(::ULIS::eBlendMode::Blend_Back);
    }

    //Execute PreUpdate delegate to retrieve potentially overriden blendparameters
    if (mOnPreUpdateDelegate.IsBound())
        blendParameters = mOnPreUpdateDelegate.Execute(blendParameters);

    //If blend parameters are different from the previous one used
    //Force refreshing all edited tiles, instead of just newly edited tiles
    if (blendParameters != mPreviousBlendParameters)
    {
        TArray<::ULIS::FRectI> rects = mEditedBlockInvalidTileMap.InvalidRects();
        mPaintBlockInvalidTileMap.Invalidate(rects);
    }

    //Update the EditedBlock content
    if (!UpdateEditedBlock(blendParameters))
        return;

    //Clear the PaintBlock
    
    TArray<::ULIS::FRectI> rects = mPaintBlockInvalidTileMap.InvalidRects();
    mEditedBlockInvalidTileMap.Invalidate(rects);
    mPaintBlockInvalidTileMap.Clear();
    mResetInvalidTileMap.Clear();

    mOnUpdateDelegate.Broadcast(rects);
}

void
FOdysseyPaintEngine::Commit(const FOdysseyBlendParameters& iBlendParameters)
{
    if (!mEditedBlock)
        return;

    //Update the EditedBlock content
    Update(iBlendParameters);

    //Clear the tile maps
    TArray<::ULIS::FRectI> rects = mEditedBlockInvalidTileMap.InvalidRects();
    mEditedBlockInvalidTileMap.Clear();

    //Inform the paintEngine has commited
    mOnCommitDelegate.Broadcast(rects);

    //Refresh the Original block to match the EditedBlock
    CopyEditedBlockToOriginalBlock();

    //Clear the Paint Block
    ClearPaintBlock();
}

void
FOdysseyPaintEngine::Reset()
{
    if (!mEditedBlock)
        return;

    //Clear the Paint Block
    ClearPaintBlock();

    //Refresh the Original block to match the EditedBlock
    CopyEditedBlockToOriginalBlock();
    
    //Set Invalid Tile Map, so that the EditedBlock can refresh the right tiles on the next call of Update()
    TArray<::ULIS::FRectI> rects = mEditedBlockInvalidTileMap.InvalidRects();
    mResetInvalidTileMap.Invalidate(rects);
    mEditedBlockInvalidTileMap.Clear();
    mPaintBlockInvalidTileMap.Clear();
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
    paintEngine->mPaintBlockInvalidTileMap.Invalidate(rects);
}

void
FOdysseyPaintEngine::Block(::ULIS::FBlock* iBlock)
{
    if (mEditedBlock == iBlock)
        return;

    //Automatically commit the previous block
    Commit(mPreviousBlendParameters);

    //Remove Callback from the previous block
    if (mPaintBlock)
        mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock());

    mEditedBlock = iBlock;
    if (!mEditedBlock)
    {
        delete mPaintBlock;
        delete mOriginalBlock;
        mPaintBlock = nullptr;
        mOriginalBlock = nullptr;

        mOnBlockChangedDelegate.Broadcast();
        return;
    }
    
    if ( !mPaintBlock || mPaintBlock->Size()    != mEditedBlock->Size() || mPaintBlock->Format()    != mEditedBlock->Format() ||
         !mOriginalBlock || mOriginalBlock->Size() != mEditedBlock->Size() || mOriginalBlock->Format() != mEditedBlock->Format() )
    {
        delete mPaintBlock;
        delete mOriginalBlock;

        //Realloc to match EditedBlock size and format
        mPaintBlock = new ::ULIS::FBlock(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        mOriginalBlock = new ::ULIS::FBlock(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());

        //Create InvalidMaps
        int tileSize = 64; //Could be a config variable one day
        mPaintBlockInvalidTileMap = FOdysseyInvalidTileMap(tileSize, mPaintBlock->Width(), mPaintBlock->Height());
        mEditedBlockInvalidTileMap = FOdysseyInvalidTileMap(tileSize, mEditedBlock->Width(), mEditedBlock->Height());
        mResetInvalidTileMap = FOdysseyInvalidTileMap(tileSize, mEditedBlock->Width(), mEditedBlock->Height());
    }

    //Prepare the Original Block
    CopyEditedBlockToOriginalBlock();

    //Clear the paintblock before anything
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mPaintBlock->Format());
    ctx.Clear(*mPaintBlock);
    ctx.Finish();

    //Set callback for paint block invalidation
    mPaintBlock->OnInvalid(::ULIS::FOnInvalidBlock( &FOdysseyPaintEngine::PaintBlockChanged, static_cast<void*>(this) ));

    mOnBlockChangedDelegate.Broadcast();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

::ULIS::FBlock*
FOdysseyPaintEngine::PaintBlock()
{
    return mPaintBlock;
}

::ULIS::FBlock*
FOdysseyPaintEngine::OriginalBlock()
{
    return mOriginalBlock;
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
}

void
FOdysseyPaintEngine::CopyEditedBlockToOriginalBlock()
{
    if (!mEditedBlock || !mOriginalBlock)
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( mEditedBlock->Format() );
    //Finish any pending operations before Copying the edited block
    ctx.Finish();
    ctx.Copy(*mEditedBlock, *mOriginalBlock);
    ctx.Finish();
}

bool
FOdysseyPaintEngine::UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters)
{
    if( !mPaintBlock || !mEditedBlock || !mOriginalBlock )
        return false;

    ::ULIS::FBlock& original = *mOriginalBlock;
    ::ULIS::FBlock& edited = *mEditedBlock;
    ::ULIS::FBlock& paint = *mPaintBlock;

    //Finish any pending operations before updating the blocks
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mOriginalBlock->Format());
    ctx.Finish();

    //Merge mPaintBlockInvalidTileMap and mResetInvalidTileMap
    FOdysseyInvalidTileMap tileMap(mPaintBlockInvalidTileMap.TileSize(), mPaintBlockInvalidTileMap.Width(), mPaintBlockInvalidTileMap.Height());
    tileMap.Invalidate(mPaintBlockInvalidTileMap.InvalidTiles());
    tileMap.Invalidate(mResetInvalidTileMap.InvalidTiles());

    TArray<FIntPoint> changedTiles = tileMap.InvalidTiles();
    if (changedTiles.Num() <= 0)
        return false;

    //Blend Paint Block over OriginalBlock
    for( int i = 0; i < changedTiles.Num(); i++ )
    {
        ::ULIS::FRectI changedRect = tileMap.GetTileRect(changedTiles[i]);

        ::ULIS::FVec2I pos( changedRect.Position() );
        ::ULIS::FEvent eventCopy;
        ctx.Copy( original, edited, changedRect, changedRect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventCopy );
        ctx.Flush();

        if (!mPaintBlockInvalidTileMap.IsValidTile(changedTiles[i]))
        {
            ::ULIS::FEvent eventBlend;
            ctx.Blend( paint, edited, changedRect, pos, ::ULIS::eBlendMode(iBlendParameters.BlendingMode), ::ULIS::eAlphaMode(iBlendParameters.AlphaMode), iBlendParameters.Opacity / 100.f, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventCopy, &eventBlend );
        }
    }    
    ctx.Finish();

    if (changedTiles.Num() > 0)
    {
        TArray<::ULIS::FRectI> rects = tileMap.InvalidRects();
        mEditedBlock->Dirty(rects.GetData(), changedTiles.Num());
    }

    return true;
}
