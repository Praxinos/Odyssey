// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaTracks.h"

class ODYSSEYANIMATION_API FOdysseyAnimationMediaTracks
    : public IMediaTracks
{
public:
    FOdysseyAnimationMediaTracks();

public:
    void Init(TWeakPtr<class FOdysseyAnimationMediaPlayer> iPlayer);
    void OnOpen();
    void OnClose();

protected:

    //~ IMediaTracks interface

    virtual bool GetAudioTrackFormat(int32 TrackIndex, int32 FormatIndex, FMediaAudioTrackFormat& OutFormat) const override;
    virtual int32 GetNumTracks(EMediaTrackType TrackType) const override;
    virtual int32 GetNumTrackFormats(EMediaTrackType TrackType, int32 TrackIndex) const override;
    virtual int32 GetSelectedTrack(EMediaTrackType TrackType) const override;
    virtual FText GetTrackDisplayName(EMediaTrackType TrackType, int32 TrackIndex) const override;
    virtual int32 GetTrackFormat(EMediaTrackType TrackType, int32 TrackIndex) const override;
    virtual FString GetTrackLanguage(EMediaTrackType TrackType, int32 TrackIndex) const override;
    virtual FString GetTrackName(EMediaTrackType TrackType, int32 TrackIndex) const override;
    virtual bool GetVideoTrackFormat(int32 TrackIndex, int32 FormatIndex, FMediaVideoTrackFormat& OutFormat) const override;
    virtual bool SelectTrack(EMediaTrackType TrackType, int32 TrackIndex) override;
    virtual bool SetTrackFormat(EMediaTrackType TrackType, int32 TrackIndex, int32 FormatIndex) override;

private:
    TWeakPtr<class FOdysseyAnimationMediaPlayer> mPlayer;
};
