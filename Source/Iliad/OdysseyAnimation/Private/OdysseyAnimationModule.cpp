// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationModule.h"

#include "IMediaModule.h"

#include "Media/OdysseyAnimationMediaPlayerFactory.h"
#include "OdysseyAnimation.h"
#include "OdysseyTelemetryModule.h"

//---

void FOdysseyAnimationModule::StartupModule()
{
    RegisterMedia();
    RegisterTelemetry();
}

void FOdysseyAnimationModule::ShutdownModule()
{
    UnregisterMedia();
    UnregisterTelemetry();
}

//---

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

//---

void
FOdysseyAnimationModule::RegisterTelemetry()
{
    FOdysseyTelemetryModule::Get().RegisterAssetClassToTrackForCreation( UOdysseyAnimation::StaticClass() );
}

void
FOdysseyAnimationModule::UnregisterTelemetry()
{
    FOdysseyTelemetryModule::Get().UnregisterAssetClassToTrackForCreation( UOdysseyAnimation::StaticClass() );
}

//---

IMPLEMENT_MODULE(FOdysseyAnimationModule, OdysseyAnimation);
