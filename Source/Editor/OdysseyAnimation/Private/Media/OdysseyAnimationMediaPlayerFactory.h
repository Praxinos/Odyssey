// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaPlayerFactory.h"

class FOdysseyAnimationMediaPlayerFactory
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
