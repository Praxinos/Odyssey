// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#include "OdysseyCachedRenderTarget.h"

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
FOdysseyCachedRenderTargetManager::Add(FOdysseyCachedRenderTarget* iCachedRenderTarget)
{
    switch(iCachedRenderTarget->GetState())
    {
        case FOdysseyCachedRenderTarget::eState::RetainedRenderTarget:
        case FOdysseyCachedRenderTarget::eState::RenderTarget:
        {
            mCachedRenderTargets.Add(iCachedRenderTarget);
            UpdateCachedRenderTargets();
        }
        break;

        case FOdysseyCachedRenderTarget::eState::Image:
        {
            mCachedImages.Add(iCachedRenderTarget);
            UpdateCachedImages();
        }
        break;

        case FOdysseyCachedRenderTarget::eState::DDC:
        {
            mCachedDDCs.Add(iCachedRenderTarget);
        }
        break;
    }
}

void
FOdysseyCachedRenderTargetManager::Remove(FOdysseyCachedRenderTarget* iCachedRenderTarget)
{
    switch(iCachedRenderTarget->GetState())
    {
        case FOdysseyCachedRenderTarget::eState::RetainedRenderTarget:
        case FOdysseyCachedRenderTarget::eState::RenderTarget:
        {
            mCachedRenderTargets.Remove(iCachedRenderTarget);
        }
        break;

        case FOdysseyCachedRenderTarget::eState::Image:
        {
            mCachedImages.Remove(iCachedRenderTarget);
        }
        break;

        case FOdysseyCachedRenderTarget::eState::DDC:
        {
            mCachedDDCs.Remove(iCachedRenderTarget);
        }
        break;
    }
}

void
FOdysseyCachedRenderTargetManager::UpdateState(FOdysseyCachedRenderTarget* iCachedRenderTarget, FOdysseyCachedRenderTarget::eState OldState, FOdysseyCachedRenderTarget::eState NewState)
{
    switch(OldState)
    {
        case FOdysseyCachedRenderTarget::eState::RetainedRenderTarget:
        case FOdysseyCachedRenderTarget::eState::RenderTarget:
        {
            mCachedRenderTargets.Remove(iCachedRenderTarget);
        }
        break;

        case FOdysseyCachedRenderTarget::eState::Image:
        {
            mCachedImages.Remove(iCachedRenderTarget);
        }
        break;

        case FOdysseyCachedRenderTarget::eState::DDC:
        {
            mCachedDDCs.Remove(iCachedRenderTarget);
        }
        break;
    }

    switch(NewState)
    {
        case FOdysseyCachedRenderTarget::eState::RetainedRenderTarget:
        case FOdysseyCachedRenderTarget::eState::RenderTarget:
        {
            mCachedRenderTargets.Add(iCachedRenderTarget);
            UpdateCachedRenderTargets();
        }
        break;

        case FOdysseyCachedRenderTarget::eState::Image:
        {
            mCachedImages.Add(iCachedRenderTarget);
            UpdateCachedImages();
        }
        break;

        case FOdysseyCachedRenderTarget::eState::DDC:
        {
            mCachedDDCs.Add(iCachedRenderTarget);
        }
        break;
    }
}

void
FOdysseyCachedRenderTargetManager::Touch(FOdysseyCachedRenderTarget* iCachedRenderTarget)
{
    //Move the CachedRenderTarget at first place
    switch(iCachedRenderTarget->GetState())
    {
        case FOdysseyCachedRenderTarget::eState::RetainedRenderTarget:
        case FOdysseyCachedRenderTarget::eState::RenderTarget:
        {
            mCachedRenderTargets.Remove(iCachedRenderTarget);
            mCachedRenderTargets.Add(iCachedRenderTarget);
        }
        break;

        case FOdysseyCachedRenderTarget::eState::Image:
        {
            mCachedImages.Remove(iCachedRenderTarget);
            mCachedImages.Add(iCachedRenderTarget);
        }
        break;

        case FOdysseyCachedRenderTarget::eState::DDC:
        {
            mCachedDDCs.Remove(iCachedRenderTarget);
            mCachedDDCs.Add(iCachedRenderTarget);
        }
        break;
    }
}

FOdysseyCachedRenderTarget::~FOdysseyCachedRenderTarget()
{
    FOdysseyCachedRenderTargetManager::Get().Remove(this);
}

FOdysseyCachedRenderTarget::FOdysseyCachedRenderTarget()
    : mId(FGuid::NewGuid())
{
    FOdysseyCachedRenderTargetManager::Get().Add(this);
}

FOdysseyCachedRenderTarget::FOdysseyCachedRenderTarget(int iWidth, int iHeight, ETextureRenderTargetFormat iRenderTargetFormat)
{
    FOdysseyCachedRenderTargetManager::Get().Add(this);
}

void
FOdysseyCachedRenderTarget::Lock()
{

}

void
FOdysseyCachedRenderTarget::Unlock()
{

}

TStrongObjectPtr<UTextureRenderTarget2D>
FOdysseyCachedRenderTarget::GetRenderTarget()
{
}

void
FOdysseyCachedRenderTarget::Touch()
{
    FOdysseyCachedRenderTargetManager::Get().Touch(this);
}

eState
FOdysseyCachedRenderTarget::GetState() const
{
    return mState;
}

void
FOdysseyCachedRenderTarget::CopyRenderTargetToImage()
{

}

void
FOdysseyCachedRenderTarget::CacheRenderTargetToImage()
{

}

void
FOdysseyCachedRenderTarget::CacheImageToDDC()
{

}

int
FOdysseyCachedRenderTarget::GetWidth() const
{
    return mWidth;
}

int
FOdysseyCachedRenderTarget::GetHeight() const
{
    return mHeight;
}

ETextureRenderTargetFormat
FOdysseyCachedRenderTarget::GetRenderTargetFormat() const
{
    return mRenderTargetFormat;
}

void
FOdysseyCachedRenderTarget::Serialize(FArchive& Ar)
{

}
