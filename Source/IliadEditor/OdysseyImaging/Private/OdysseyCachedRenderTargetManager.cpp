// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#include "OdysseyCachedRenderTargetManager.h"

FOdysseyCachedRenderTargetManager&
FOdysseyCachedRenderTargetManager::Get()
{
    static FOdysseyCachedRenderTargetManager instance;
    return instance;
}


FOdysseyCachedRenderTargetManager::FOdysseyCachedRenderTargetManager()
{
}

void
FOdysseyCachedRenderTargetManager::Add(const UOdysseyCachedRenderTarget* iCachedRenderTarget)
{
    switch(iCachedRenderTarget->GetCacheState())
    {
        case UOdysseyCachedRenderTarget::eCacheState::RenderTarget:
        {
            mCachedRenderTargets.Add(iCachedRenderTarget);
            UpdateCachedRenderTargets();
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::Image:
        {
            mCachedImages.Add(iCachedRenderTarget);
            UpdateCachedImages();
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::DDC:
        {
            mCachedDDCs.Add(iCachedRenderTarget);
        }
        break;
    }
}

void
FOdysseyCachedRenderTargetManager::Remove(const UOdysseyCachedRenderTarget* iCachedRenderTarget)
{
    switch(iCachedRenderTarget->GetCacheState())
    {
        case UOdysseyCachedRenderTarget::eCacheState::RenderTarget:
        {
            mCachedRenderTargets.Remove(iCachedRenderTarget);
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::Image:
        {
            mCachedImages.Remove(iCachedRenderTarget);
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::DDC:
        {
            mCachedDDCs.Remove(iCachedRenderTarget);
        }
        break;
    }
}

void
FOdysseyCachedRenderTargetManager::UpdateCacheState(const UOdysseyCachedRenderTarget* iCachedRenderTarget, UOdysseyCachedRenderTarget::eCacheState OldState, UOdysseyCachedRenderTarget::eCacheState NewState)
{
    switch(OldState)
    {
        case UOdysseyCachedRenderTarget::eCacheState::RenderTarget:
        {
            mCachedRenderTargets.Remove(iCachedRenderTarget);
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::Image:
        {
            mCachedImages.Remove(iCachedRenderTarget);
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::DDC:
        {
            mCachedDDCs.Remove(iCachedRenderTarget);
        }
        break;
    }

    switch(NewState)
    {
        case UOdysseyCachedRenderTarget::eCacheState::RenderTarget:
        {
            mCachedRenderTargets.Add(iCachedRenderTarget);
            UpdateCachedRenderTargets();
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::Image:
        {
            mCachedImages.Add(iCachedRenderTarget);
            UpdateCachedImages();
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::DDC:
        {
            mCachedDDCs.Add(iCachedRenderTarget);
        }
        break;
    }
}

void
FOdysseyCachedRenderTargetManager::Touch(const UOdysseyCachedRenderTarget* iCachedRenderTarget)
{
    //Move the CachedRenderTarget at first place
    switch(iCachedRenderTarget->GetCacheState())
    {
        case UOdysseyCachedRenderTarget::eCacheState::RenderTarget:
        {
            mCachedRenderTargets.Remove(iCachedRenderTarget);
            mCachedRenderTargets.Add(iCachedRenderTarget);
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::Image:
        {
            mCachedImages.Remove(iCachedRenderTarget);
            mCachedImages.Add(iCachedRenderTarget);
        }
        break;

        case UOdysseyCachedRenderTarget::eCacheState::DDC:
        {
            mCachedDDCs.Remove(iCachedRenderTarget);
            mCachedDDCs.Add(iCachedRenderTarget);
        }
        break;
    }
}

void
FOdysseyCachedRenderTargetManager::UpdateCachedRenderTargets()
{
    //Dont forget CachedRenderTarget->GetIsDrawing() keeps CachedRenderTarget loaded
}

void
FOdysseyCachedRenderTargetManager::UpdateCachedImages()
{

}
