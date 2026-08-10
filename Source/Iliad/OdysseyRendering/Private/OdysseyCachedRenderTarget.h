// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Engine/TextureRenderTarget2D.h"


class FOdysseyCachedRenderTargetManager
{
public:
    static FOdysseyCachedRenderTargetManager& Get();

private:
    FOdysseyCachedRenderTargetManager();

public:
    /**
     * Adds a newly created CachedRenderTarget
     * Will store CachedRenderTarget into mCachedDDCs
     */
    void Add(FOdysseyCachedRenderTarget* iCachedRenderTarget);

    /**
     * Removes a destroyed CachedRenderTarget
     */
    void Remove(FOdysseyCachedRenderTarget* iCachedRenderTarget);

    /**
     * Moves the CachedRenderTarget to the top of the list corresponding to NewState
     * If needed, will release some memory by caching old Rendertargets
     */
    void UpdateState(FOdysseyCachedRenderTarget* iCachedRenderTarget, FOdysseyCachedRenderTarget::eState OldState, FOdysseyCachedRenderTarget::eState NewState);

private:
    TArray<FOdysseyCachedRenderTarget*> mCachedRenderTargets;
    TArray<FOdysseyCachedRenderTarget*> mCachedImages;
    TArray<FOdysseyCachedRenderTarget*> mCachedDDCs;
}

class FOdysseyCachedRenderTarget
{
public:
    enum class eState
    {
        RetainedRenderTarget, //The RenderTarget and the FImage are loaded, and the RenderTarget cannot be unloaded for now
        RenderTarget, //The RenderTarget and the FImage are loaded
        Image, //Only the FImage is loaded, the Render Target has been unloaded
        DDC //Data is only present in the DDC, the Render Target and the FImage have been unloaded
    };

public:
    ~FOdysseyCachedRenderTarget();
    FOdysseyCachedRenderTarget();
    FOdysseyCachedRenderTarget(int iWidth, int iHeight, ETextureRenderTargetFormat iRenderTargetFormat);

public:
    /**
     * Force the RenderTarget to stay in memory
     * The RenderTarget will not be released until a call to Unlock
     */
    void Lock();

    /**
     * Allows the RenderTarget to be released
     * Does not release the RenderTarget right away
     */
    void Unlock();

    /**
     * Returns the RenderTarget
     * Will load render target if not already loaded
     */
    TStrongObjectPtr<UTextureRenderTarget2D> GetRenderTarget();

public:
    /**
     * Returns the State
     */
    eState GetState() const;

    /**
     * Copy the content of the RenderTarget into the mImage
     * Does not release RenderTarget
     */
    void CopyRenderTargetToImage();

    /**
     * Copy the content of the RenderTarget into the mImage
     * Release RenderTarget after the copy is done
     */
    void CacheRenderTargetToImage();

    /**
     * Copy the content of mImage into the DDC
     * Release mImage after the copy is done
     * Calls CacheRenderTargetToImage() if mImage is not up to date yet
     */
    void CacheImageToDDC();

public:
    // Getters

    /**
     * Gets the Width of the Image
     * @return Width of the tilemap
     */
    int GetWidth() const;

    /**
     * Gets the Height of the Image
     * @return Height of the tilemap
     */
    int GetHeight() const;

    /**
     * Gets the Render Target Format
     */
    ETextureRenderTargetFormat GetRenderTargetFormat() const;

public:
    void Serialize(FArchive& Ar);

private:
    int mWidth = 0;
    int mHeight = 0;
    ETextureRenderTargetFormat mRenderTargetFormat = RTF_RGBA8;

    TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget;
    FImage mImage;
    FGuid mId; //DDC Key
};
