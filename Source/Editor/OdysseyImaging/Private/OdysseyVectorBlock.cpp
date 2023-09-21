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
    mRenderFlags = 0;
    mRenderHUD = false;
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

void
FOdysseyVectorBlock::SetRenderFlags(uint64 iRenderFlags)
{
    if (iRenderFlags == mRenderFlags)
        return;

    mRenderFlags = iRenderFlags;
    Invalidate(false);
}

void
FOdysseyVectorBlock::SetRenderHUD(bool iRenderHUD)
{
    if (iRenderHUD == mRenderHUD)
        return;

    mRenderHUD = iRenderHUD;
    Invalidate(false);
}

uint64
FOdysseyVectorBlock::GetRenderFlags() const
{
    return mRenderFlags;
}

void
FOdysseyVectorBlock::Render(::ULIS::FBlock& ioBlock)
{   
	TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorBlock::Render);
    //Render in a BLImage
    mEngine->Render(mBlockData->mBLImage.Get(), mRenderFlags);

    if (mRenderHUD)
        mEngine->RenderHUD(mBlockData->mBLImage.Get());

    //Get a ULIS block pointing to the BLImage
    BLImageData imgData;
    mBlockData->mBLImage->getData(&imgData);
    ::ULIS::FBlock renderBlock((uint8*)imgData.pixelData, mWidth, mHeight, ULIS::Format_BGRA8);

    //Unpremultiply the render block
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULIS::Format_BGRA8);
    ctx.Unpremultiply(renderBlock);
    ctx.Finish();

    //Convert the right ULIS block in the expected ULIS Format
    ctx.ConvertFormat(renderBlock, ioBlock);
    ctx.Finish();
}

TSharedPtr<::ULIS::FBlock>
FOdysseyVectorBlock::Render()
{
    TSharedPtr<::ULIS::FBlock> block = GetBlock();
    if ( !block )
        return nullptr;

    if (mBlockData->mState == kNeedsRender)
    {
        Render(*block);
        mBlockData->mState = kCacheInvalid;
    }

    return block;
}

void
FOdysseyVectorBlock::CleanupBlock(uint8* iData, void* iInfo)
{
    FBlockData* blockData = static_cast<FBlockData*>(iInfo);

    if ( blockData->mState == kCacheInvalid )
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

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyVectorBlock::GetBlock()
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

    //FUniqueBuffer buffer;
    FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
    if ( cache.Load(mId.ToString(), mBlockData->mBuffer) )
    {
        //ULIS Block : Used externally to read/blend the pixels in Unreal
        block = MakeShared<::ULIS::FBlock>((uint8*)mBlockData->mBuffer.GetData(), mWidth, mHeight, mFormat);
        
        mBlockData->mState = kCacheUpToDate;
    }
    else
    {
        block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, mFormat);
        mBlockData->mBuffer = FUniqueBuffer::MakeView(block->Bits(), block->BytesTotal());
        Render(*block);

        mBlockData->mState = kCacheInvalid;
    }

    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyVectorBlock::CleanupBlock, mBlockData));
    
    mBlock = block;

    return block; //return the currently loaded block, the receiver can release it whenever he wants
}

void
FOdysseyVectorBlock::OnVectorEngineSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags )
{
    if (!mEngine || mEngine->GetScene() != iScene)
        return;

    if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW )
    {
        Invalidate(iSignalFlags & FOdysseyVectorEngine::SIGNAL_INTERACTIVE);
    }
}

void
FOdysseyVectorBlock::Invalidate(bool iIsInteractive)
{
    TSharedPtr<::ULIS::FBlock> block = mBlock.Pin();
    if (!block)
        return;

    if (mBlockData->mState == kCacheUpToDate)
    {
        //Remove block from cache and invalidate cache
        FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
        cache.Remove(mBlockData->mId.ToString());
    }
    mBlockData->mState = kNeedsRender;
    mOnInvalidated.Broadcast(iIsInteractive);
}
