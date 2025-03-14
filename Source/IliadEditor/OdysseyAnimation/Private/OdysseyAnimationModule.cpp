// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationModule.h"

#include "IMediaModule.h"
#include "Media/OdysseyAnimationMediaPlayerFactory.h"

void FOdysseyAnimationModule::StartupModule()
{
    RegisterMedia();
}

void FOdysseyAnimationModule::ShutdownModule()
{
    UnregisterMedia();
}

void
FOdysseyAnimationModule::RegisterMedia()
{
    // register player factory
    auto MediaModule = FModuleManager::LoadModulePtr<IMediaModule>("Media");

    if (MediaModule != nullptr)
        MediaModule->RegisterPlayerFactory(mAnimationMediaPlayerFactory);
}

void
FOdysseyAnimationModule::UnregisterMedia()
{
    // unregister player factory
    auto MediaModule = FModuleManager::GetModulePtr<IMediaModule>("Media");

    if (MediaModule != nullptr)
        MediaModule->UnregisterPlayerFactory(mAnimationMediaPlayerFactory);
}

IMPLEMENT_MODULE(FOdysseyAnimationModule, OdysseyAnimation);
