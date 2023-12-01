// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaPlayerFactory.h"

#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "Animation"

FOdysseyAnimationMediaPlayerFactory::FOdysseyAnimationMediaPlayerFactory()
{
    // supported platforms
    mSupportedPlatforms.Add(TEXT("Windows"));
    mSupportedPlatforms.Add(TEXT("Mac"));
    mSupportedPlatforms.Add(TEXT("Linux"));

    // supported schemes
    mSupportedUriSchemes.Add("odysseyanimation");
}

bool
FOdysseyAnimationMediaPlayerFactory::CanPlayUrl(const FString& Url, const IMediaOptions* /*Options*/, TArray<FText>* /*OutWarnings*/, TArray<FText>* OutErrors) const
{
    FString Scheme;
    FString Location;

    // check scheme
    if (!Url.Split(TEXT("://"), &Scheme, &Location, ESearchCase::CaseSensitive))
    {
        if (OutErrors != nullptr)
        {
            OutErrors->Add(LOCTEXT("media-player-factory.errors.no-url-scheme-found", "No URL scheme found"));
        }

        return false;
    }

    if (!mSupportedUriSchemes.Contains(Scheme))
    {
        if (OutErrors != nullptr)
        {
            OutErrors->Add(FText::Format(LOCTEXT("media-player-factory.errors.url-scheme-not-supported", "The URL scheme '{0}' is not supported"), FText::FromString(Scheme)));
        }

        return false;
    }

    return true;
}

TSharedPtr<IMediaPlayer, ESPMode::ThreadSafe>
FOdysseyAnimationMediaPlayerFactory::CreatePlayer(IMediaEventSink& EventSink)
{
    TSharedPtr<FOdysseyAnimationMediaPlayer> player = MakeShared<FOdysseyAnimationMediaPlayer, ESPMode::ThreadSafe>(EventSink);
    player->Init();
    return player;
}

FText
FOdysseyAnimationMediaPlayerFactory::GetDisplayName() const
{
	return LOCTEXT("media-player-factory.display-name", "Odyssey Animation");
}

FName
FOdysseyAnimationMediaPlayerFactory::GetPlayerName() const
{
    static FName PlayerName(TEXT("OdysseyAnimation"));
	return PlayerName;
}

FGuid
FOdysseyAnimationMediaPlayerFactory::GetPlayerPluginGUID() const
{
    static FGuid PlayerPluginGUID(0xb3da1256, 0x7366410b, 0xacfdc6e3, 0x54ff7e5d);
	return PlayerPluginGUID;
}

const TArray<FString>&
FOdysseyAnimationMediaPlayerFactory::GetSupportedPlatforms() const
{
	return mSupportedPlatforms;
}

bool
FOdysseyAnimationMediaPlayerFactory::SupportsFeature(EMediaFeature Feature) const
{
    return Feature == EMediaFeature::VideoSamples;
        //|| Feature == EMediaFeature::AudioSamples;
}


#undef LOCTEXT_NAMESPACE