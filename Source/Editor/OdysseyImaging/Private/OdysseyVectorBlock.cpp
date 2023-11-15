// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorBlock.h"
#include "OdysseyVectorEngine.h"

#define FOdysseyVectorBlock_CACHE_NAME TEXT("OdysseyVectorBlock")
#define FOdysseyVectorBlock_CACHE_VERSION TEXT("79ED1F6D43774CB1B8BE2766ED7F0120")

FOdysseyVectorBlock::FOnInvalidated&
FOdysseyVectorBlock::OnInvalidated()
{
    return mOnInvalidated;
}

FOdysseyVectorBlock::~FOdysseyVectorBlock()
{
    FOdysseyVectorEngine::OnSignalDelegate().RemoveAll( this );
}

FOdysseyVectorBlock::FOdysseyVectorBlock()
    : mBlockData(nullptr)
{
    FOdysseyVectorEngine::OnSignalDelegate().AddRaw( this, &FOdysseyVectorBlock::OnVectorEngineSignal );
}

void
FOdysseyVectorBlock::Init(const FGuid& iId, FOdysseyVectorEngine* iEngine, int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    mId = iId;
    mEngine = iEngine;
    mWidth = iWidth;
    mHeight = iHeight;
    mFormat = iFormat;
    mNeedsRender = false;
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
FOdysseyVectorBlock::Render(::ULIS::FBlock& ioBlock, uint64 iDrawingFlags )
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::Render);
    ::ULIS::FRectI invalidatedRect = mEngine->GetInvalidatedRect();

    //Render in a BLImage (also resets the internal invalidation rectangle)
    mEngine->Render(mBlockData->mBLContext.Get(), iDrawingFlags);

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::Render::ConvertBlock);
        //Get a ULIS block pointing to the BLImage
        BLImageData imgData;
        mBlockData->mBLImage->getData(&imgData);
        ::ULIS::FBlock renderBlock((uint8*)imgData.pixelData, mWidth, mHeight, ULIS::Format_BGRA8);

        //Unpremultiply the render block
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULIS::Format_BGRA8);
        ctx.Unpremultiply(renderBlock, invalidatedRect );
        ctx.Finish();

        //Convert the right ULIS block in the expected ULIS Format
        ctx.ConvertFormat(renderBlock, ioBlock, invalidatedRect, ::ULIS::FVec2I( invalidatedRect.x, invalidatedRect.y ) );
        ctx.Finish();
    }
}

void
FOdysseyVectorBlock::RenderHUD(::ULIS::FBlock& ioBlock)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::RenderHUD);
    mEngine->RenderHUD( mHUDBlockData->mBLContext.Get() );

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::Render::ConvertHUDBlock);
        //Get a ULIS block pointing to the BLImage
        BLImageData imgData;
        mHUDBlockData->mBLImage->getData(&imgData);
        ::ULIS::FBlock renderBlock((uint8*)imgData.pixelData, mWidth, mHeight, ULIS::Format_BGRA8);

        //Unpremultiply the render block
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULIS::Format_BGRA8);
        ctx.Unpremultiply(renderBlock);
        ctx.Finish();

        //Convert the right ULIS block in the expected ULIS Format
        ctx.ConvertFormat(renderBlock, ioBlock);
        ctx.Finish();
    }
}

TSharedPtr<::ULIS::FBlock>
FOdysseyVectorBlock::Render( uint64 iDrawingFlags )
{
    TSharedPtr<::ULIS::FBlock> block = GetBlock( iDrawingFlags );
    if ( !block )
        return nullptr;

    if (mNeedsRender)
    {
        Render(*block, iDrawingFlags );

        TSharedPtr<::ULIS::FBlock> hudBlock = mHUDBlock.Pin();
        if ( hudBlock )
            RenderHUD(*hudBlock);

        mNeedsRender = false;
    }

    return block;
}

void
FOdysseyVectorBlock::CleanupBlock(uint8* iData, void* iInfo)
{
    FBlockData* blockData = static_cast<FBlockData*>(iInfo);

    // End BLContext operations on the BLImage
    blockData->mBLContext.Get()->end();

    if ( blockData->mNeedsCache )
    {
        FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
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
    createInfo.threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    mHUDBlockData->mBLContext.Get()->begin(*mHUDBlockData->mBLImage.Get(), createInfo);

    block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, mFormat);
    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyVectorBlock::CleanupHUDBlock, mHUDBlockData));
    RenderHUD(*block);
    mHUDBlock = block;

    return block;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyVectorBlock::GetBlock(uint64 iDrawingFlags)
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

    //Blend2D block : Used internally to render the Vector Scene into a pixel block
    mBlockData->mBLImage = MakeShared<BLImage>(mWidth, mHeight, BL_FORMAT_PRGB32);
    mBlockData->mBLContext = MakeShared<BLContext>();
    // Starts BLContext operations on the BLImage
    BLContextCreateInfo createInfo{};
    createInfo.threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    mBlockData->mBLContext.Get()->begin(*mBlockData->mBLImage.Get(), createInfo);

    //FUniqueBuffer buffer;
    FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
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
        Render(*block, iDrawingFlags);
        mBlockData->mNeedsCache = true;
        mNeedsRender = false;
    }

    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyVectorBlock::CleanupBlock, mBlockData));
    
    mBlock = block;

    return block; //return the currently loaded block, the receiver can release it whenever he wants
}

void
FOdysseyVectorBlock::OnVectorEngineSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags )
{
    if (!mEngine || mEngine->GetScene() != iScene)
        return;

    if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW )
    {
        Invalidate(iSignalFlags & FOdysseyVectorEngine::SIGNAL_INTERACTIVE);
    }
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
FOdysseyVectorBlock::Invalidate(bool iIsInteractive)
{
    if (!mNeedsRender)
    {
        //Remove block from cache and invalidate cache
        FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
        cache.Remove(mId.ToString());
        mNeedsRender = true;
        
        TSharedPtr<::ULIS::FBlock> block = mBlock.Pin();
        if (block)
            mBlockData->mNeedsCache = true;
    }

    mEngine->Invalidate();
    //SetState(kNeedsRender);
    mOnInvalidated.Broadcast(iIsInteractive);
}
