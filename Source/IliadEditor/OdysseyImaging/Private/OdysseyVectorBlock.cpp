// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorBlock.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include "ULISLoaderModule.h"

#define FOdysseyVectorBlock_CACHE_NAME TEXT("OdysseyVectorBlock")
#define FOdysseyVectorBlock_CACHE_VERSION TEXT("79ED1F6D43774CB1B8BE2766ED7F0120")

FOdysseyVectorBlock::FOnInvalidated&
FOdysseyVectorBlock::OnInvalidated()
{
    return mOnInvalidated;
}

FOdysseyVectorBlock::~FOdysseyVectorBlock()
{
    if (mVectorCell)
        mVectorCell->OnRequestRedrawDelegate().RemoveAll( this );
}

FOdysseyVectorBlock::FOdysseyVectorBlock()
    : mBlockData(nullptr)
{

}

void
FOdysseyVectorBlock::Init(const FGuid& iId, TSharedPtr<FOdysseyVectorCell> iRoot, int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    mId = iId;
    mWidth = iWidth;
    mHeight = iHeight;
    mFormat = iFormat;
    mNeedsRender = false;
    mVectorCell = iRoot;
    mSanitizedRect = ::ULIS::FRectI( 0, 0, mWidth, mHeight );
    mBLImage = MakeShared<BLImage>(mWidth, mHeight, BL_FORMAT_PRGB32);
    mBLContext = MakeShared<BLContext>();

    mVectorCell->OnRequestRedrawDelegate().AddRaw( this, &FOdysseyVectorBlock::OnVectorRootRequestRedraw );
}

int
FOdysseyVectorBlock::GetWidth() const
{
    return mWidth;
}

int
FOdysseyVectorBlock::GetHeight() const
{
    return mHeight;
}

::ULIS::eFormat
FOdysseyVectorBlock::GetFormat() const
{
    return mFormat;
}
/*
void
FOdysseyVectorBlock::SetRenderFlags(uint64 iRenderFlags)
{
    if (iRenderFlags == mRenderFlags)
        return;

    mRenderFlags = iRenderFlags;
    Invalidate(false);
}

uint64
FOdysseyVectorBlock::GetRenderFlags() const
{
    return mRenderFlags;
}
*/

void
FOdysseyVectorBlock::Render( ::ULIS::FBlock& ioBlock, const ::ULIS::FRectI& iRect, uint64 iDrawingFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::Render);

    if( mVectorCell.IsValid() )
    {
        //Render in a BLImage (also resets the internal invalidation rectangle)
        BLContextCreateInfo createInfo{};
        createInfo.thread_count = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
        mBLContext->begin(*mBLImage, createInfo);
        mEngine.Render( mBLContext.Get(), iRect, mVectorCell.Get(), iDrawingFlags);
        mBLContext->end();

        if ( iRect.Area() )
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::Render::ConvertBlock);
            //Get a ULIS block pointing to the BLImage
            BLImageData imgData;
            mBLImage->get_data(&imgData);
            ::ULIS::FBlock renderBlock((uint8*)imgData.pixel_data, mWidth, mHeight, ULIS::Format_BGRA8);

            //Unpremultiply the render block
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULIS::Format_BGRA8);
            ctx.Unpremultiply(renderBlock, iRect);
            ctx.Finish();

            //Convert the right ULIS block in the expected ULIS Format
            ctx.ConvertFormat(renderBlock, ioBlock, iRect, ::ULIS::FVec2I(iRect.x, iRect.y ) );
            ctx.Finish();
        }
    }
}

void
FOdysseyVectorBlock::RenderHUD( ::ULIS::FBlock& ioBlock )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::RenderHUD);

    if( mVectorCell.IsValid() )
    {
        mEngine.RenderHUD( mHUDBlockData->mBLContext.Get(), mVectorCell->GetScene() );

        {
            TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::Render::ConvertHUDBlock);
            //Get a ULIS block pointing to the BLImage
            BLImageData imgData;
            mHUDBlockData->mBLImage->get_data(&imgData);
            ::ULIS::FBlock renderBlock((uint8*)imgData.pixel_data, mWidth, mHeight, ULIS::Format_BGRA8);

            //Unpremultiply the render block
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULIS::Format_BGRA8);
            ctx.Unpremultiply(renderBlock);
            ctx.Finish();

            //Convert the right ULIS block in the expected ULIS Format
            ctx.ConvertFormat(renderBlock, ioBlock);
            ctx.Finish();
        }
    }
}

TSharedPtr<::ULIS::FBlock>
FOdysseyVectorBlock::Render( uint64 iDrawingFlags )
{
    TSharedPtr<::ULIS::FBlock> block = GetBlock( iDrawingFlags );

    if ( !block )
        return nullptr;

     // Only one render at a time
    FScopeLock Lock(&mMutex);

    if (mNeedsRender)
    {
        Render(*block, mSanitizedRect, iDrawingFlags );

        TSharedPtr<::ULIS::FBlock> hudBlock = mHUDBlock.Pin();
        if ( hudBlock )
            RenderHUD(*hudBlock );

        mNeedsRender = false;
        mBlockData->mNeedsCache = true;
    }

    return block;
}

void
FOdysseyVectorBlock::CleanupBlock(uint8* iData, void* iInfo)
{
    FBlockData* blockData = static_cast<FBlockData*>(iInfo);

    // End BLContext operations on the BLImage
    //blockData->mBLContext.Get()->end();

    if ( blockData->mNeedsCache )
    {
        FOdysseyDiskCache cache(FOdysseyVectorBlock_CACHE_NAME, FOdysseyVectorBlock_CACHE_VERSION);
        FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(blockData->mBuffer.GetView());
        cache.Save(blockData->mId.ToString(), sharedBuffer);
    }

    //we have the responsability to delete the block data
    if (blockData->mBuffer.IsOwned())
    {
        //Data is owned by the sharedBuffer
    }
    else
    {
        //Data is owned by the block
        ::ULIS::OnCleanup_FreeMemory(iData, iInfo);
    }

    delete blockData;
}



void
FOdysseyVectorBlock::CleanupHUDBlock(uint8* iData, void* iInfo)
{
    FHUDBlockData* blockData = static_cast<FHUDBlockData*>(iInfo);

    // End BLContext operations on the BLImage
    blockData->mBLContext.Get()->end();

    //Data is owned by the block
    ::ULIS::OnCleanup_FreeMemory(iData, iInfo);

    delete blockData; //will also delete the associated BLImage
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyVectorBlock::GetHUDBlock()
{
    FScopeLock Lock(&mMutex);

    if ( mWidth <= 0 || mHeight <= 0 )
        return nullptr;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block;
    block = mHUDBlock.Pin();
    if ( block )
        return block;

    mHUDBlockData = new FHUDBlockData();
    mHUDBlockData->mBLImage = MakeShared<BLImage>(mWidth, mHeight, BL_FORMAT_PRGB32);
    mHUDBlockData->mBLContext = MakeShared<BLContext>();
    // Starts BLContext operations on the BLImage
    BLContextCreateInfo createInfo{};
    createInfo.thread_count = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    mHUDBlockData->mBLContext.Get()->begin(*mHUDBlockData->mBLImage.Get(), createInfo);

    block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, mFormat);
    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyVectorBlock::CleanupHUDBlock, mHUDBlockData));
    RenderHUD(*block);
    mHUDBlock = block;

    return block;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyVectorBlock::GetBlock( uint64 iDrawingFlags)
{
    FScopeLock Lock(&mMutex);

    if ( mWidth <= 0 || mHeight <= 0 )
        return nullptr;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block;
    block = mBlock.Pin();
    if ( block )
        return block;

    mBlockData = new FBlockData();
    mBlockData->mId = mId;
    mBlockData->mFormat = mFormat;

    //FUniqueBuffer buffer;
    FOdysseyDiskCache cache(FOdysseyVectorBlock_CACHE_NAME, FOdysseyVectorBlock_CACHE_VERSION);
    if ( cache.Load(mId.ToString(), mBlockData->mBuffer) )
    {
        //ULIS Block : Used externally to read/blend the pixels in Unreal
        block = MakeShared<::ULIS::FBlock>((uint8*)mBlockData->mBuffer.GetData(), mWidth, mHeight, mFormat);
        mBlockData->mNeedsCache = false;
    }
    else
    {
        block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, mFormat);
        mBlockData->mBuffer = FUniqueBuffer::MakeView(block->Bits(), block->BytesTotal());
        Render(*block, mSanitizedRect, iDrawingFlags);
        mBlockData->mNeedsCache = true;
        mNeedsRender = false;
    }

    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyVectorBlock::CleanupBlock, mBlockData));

    mBlock = block;

    return block; //return the currently loaded block, the receiver can release it whenever he wants
}

void
FOdysseyVectorBlock::OnVectorRootRequestRedraw( FOdysseyVectorGroupPaint* ignored, uint64 iSignalFlags )
{
    Invalidate( mVectorCell->GetInvalidatedRect()
              , iSignalFlags & FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

/*
void
FOdysseyVectorBlock::SetState(eBlockState iState)
{
    TSharedPtr<::ULIS::FBlock> block = mBlock.Pin();
    if (block)
    {
        mBlockData->mState = iState;
    }
    mState = iState;
}
*/

void
FOdysseyVectorBlock::Invalidate( const ::ULIS::FRectD& iRect, bool iIsInteractive)
{
    ::ULIS::FRectD sanitizedRectD = mEngine.SanitizeRect( iRect, mWidth, mHeight );

    mSanitizedRect = sanitizedRectD;

    if (!mNeedsRender)
    {
        //Remove block from cache and invalidate cache
        FOdysseyDiskCache cache(FOdysseyVectorBlock_CACHE_NAME, FOdysseyVectorBlock_CACHE_VERSION);
        cache.Remove(mId.ToString());
        mNeedsRender = true;
    }

    //mEngine->Invalidate( FOdysseyVectorEngine::INVALIDATE_DEFAULT );
    //SetState(kNeedsRender);
    mOnInvalidated.Broadcast( { mSanitizedRect }, iIsInteractive );
}

FOdysseyVectorEngine&
FOdysseyVectorBlock::GetEngine()
{
    return mEngine;
}
