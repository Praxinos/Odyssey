// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IMediaPlayerFactory.h"

class ODYSSEYANIMATION_API FOdysseyAnimationMediaPlayerFactory
    : public IMediaPlayerFactory
{
public:
    FOdysseyAnimationMediaPlayerFactory();

public:
    virtual bool CanPlayUrl(const FString& Url, const IMediaOptions* /*Options*/, TArray<FText>* /*OutWarnings*/, TArray<FText>* OutErrors) const override;
    virtual TSharedPtr<IMediaPlayer, ESPMode::ThreadSafe> CreatePlayer(IMediaEventSink& EventSink) override;
    virtual FText GetDisplayName() const override;
    virtual FName GetPlayerName() const override;
    virtual FGuid GetPlayerPluginGUID() const override;
    virtual const TArray<FString>& GetSupportedPlatforms() const override;
    virtual bool SupportsFeature(EMediaFeature Feature) const override;

private:
    /** List of platforms that the media player support. */
    TArray<FString> mSupportedPlatforms;

    /** List of supported URI schemes. */
    TArray<FString> mSupportedUriSchemes;
};
