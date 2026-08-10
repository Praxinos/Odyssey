// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyCachedRenderTarget.h"

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
    void Add(const UOdysseyCachedRenderTarget* iCachedRenderTarget);

    /**
     * Removes a destroyed CachedRenderTarget
     */
    void Remove(const UOdysseyCachedRenderTarget* iCachedRenderTarget);

    /**
     * Moves the CachedRenderTarget to the top of the list corresponding to NewState
     * If needed, will release some memory by caching old Rendertargets
     */
    void UpdateCacheState(const UOdysseyCachedRenderTarget* iCachedRenderTarget, UOdysseyCachedRenderTarget::eCacheState OldState, UOdysseyCachedRenderTarget::eCacheState NewState);

    void Touch(const UOdysseyCachedRenderTarget* iCachedRenderTarget);

private:
    void UpdateCachedRenderTargets();
    void UpdateCachedImages();

private:
    TArray<const UOdysseyCachedRenderTarget*> mCachedRenderTargets;
    TArray<const UOdysseyCachedRenderTarget*> mCachedImages;
    TArray<const UOdysseyCachedRenderTarget*> mCachedDDCs;
};
