// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyDiskCache.h"
#include <ULIS>
#include "blend2d.h"

class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;

class ODYSSEYIMAGING_API FOdysseyVectorBlock : public TSharedFromThis<FOdysseyVectorBlock>
{

public:
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInvalidated, const TArray<::ULIS::FRectI>& iRects, bool /*iIsInteractive*/)
    FOnInvalidated& OnInvalidated();

public:
    // Construction / Destruction
    ~FOdysseyVectorBlock();
    FOdysseyVectorBlock();

public:
    /**
     * @brief Inits the Vector block to link it to the specified Vector Engine, and size/format
     *
     * @param iId
     * @param iEngine
     * @param iWidth
     * @param iHeight
     * @param iFormat
     */
    void Init(const FGuid& iId, FOdysseyVectorEngine* iEngine, int iWidth, int iHeight, ::ULIS::eFormat iFormat);

    /**
     * @brief Get the block Width
     *
     * @return int
     */
    int GetWidth() const;

    /**
     * @brief Get the block Height
     *
     * @return int
     */
    int GetHeight() const;

    /**
     * @brief Get the block Format
     *
     * @return ::ULIS::eFormat
     */
    ::ULIS::eFormat GetFormat() const;

    /**
     * @brief Get the Block object
     *
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
     */
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlock( uint64 iDrawingFlags );

    /**
     * @brief Get the Block object
     *
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
     */
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetHUDBlock();

    /**
     * @brief Renders the Scene into the internal block and returns the block
     *
     * @return TSharedPtr<::ULIS::FBlock>
     */
    TSharedPtr<::ULIS::FBlock> Render( uint64 iDrawingFlags );

    /**
     * @brief Sets the render flags passed to the vector engine
     */
    void SetRenderFlags(uint64 iRenderFlags);

    /**
     * @brief Gets the render flags passed to the vector engine
     */
    uint64 GetRenderFlags() const;

private:
    enum eBlockState
    {
        kCacheUpToDate,
        kNeedsRender,
        kCacheInvalid
    };

    static void CleanupBlock(uint8* iData, void* iInfo);
    static void CleanupHUDBlock(uint8* iData, void* iInfo);
    void Render(::ULIS::FBlock& ioBlock, uint64 iDrawingFlags);
    void RenderHUD(::ULIS::FBlock& ioBlock);
    void OnVectorEngineSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void Invalidate(bool iIsInteractive);
    void SetState(eBlockState iState);

private:
    //The stable block for which edition is finished
    FGuid mId;
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mHUDBlock; //Loaded on demand
    FOdysseyVectorEngine* mEngine;
    int mWidth;
    int mHeight;
    ::ULIS::eFormat mFormat;
    //uint64 mRenderFlags; //See DRAWING_IGNORECOLOR for example

    //
    // OPTIMIZATIONS
    //

    FCriticalSection mMutex;

    struct FBlockData
    {
        FGuid mId;
        ::ULIS::eFormat mFormat;
        TSharedPtr<BLImage> mBLImage;
        TSharedPtr<BLContext> mBLContext;
        FUniqueBuffer mBuffer;
        //eBlockState mState;
        bool mNeedsCache;
    };

    struct FHUDBlockData
    {
        TSharedPtr<BLImage> mBLImage;
        TSharedPtr<BLContext> mBLContext;
    };

    //eBlockState mState;
    bool mNeedsRender;
    FBlockData* mBlockData;
    FHUDBlockData* mHUDBlockData;
    FOnInvalidated mOnInvalidated;
};
