// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaTracks.h"

#include "Media/OdysseyAnimationMediaPlayer.h"

#define LOCTEXT_NAMESPACE "Animation"

FOdysseyAnimationMediaTracks::FOdysseyAnimationMediaTracks()
{
}

void
FOdysseyAnimationMediaTracks::Init(TWeakPtr<FOdysseyAnimationMediaPlayer> iPlayer)
{
    mPlayer = iPlayer;
}

void
FOdysseyAnimationMediaTracks::OnOpen()
{
    TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
    if ( !player )
        return;

    player->GetEventSink().ReceiveMediaEvent(EMediaEvent::TracksChanged);
}

void
FOdysseyAnimationMediaTracks::OnClose()
{
    TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
    if ( !player )
        return;

    player->GetEventSink().ReceiveMediaEvent(EMediaEvent::TracksChanged);
}

int32
FOdysseyAnimationMediaTracks::GetNumTracks(EMediaTrackType TrackType) const
{
    switch(TrackType)
    {
        case EMediaTrackType::Video:
            return 1;

        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return 0;
}

int32
FOdysseyAnimationMediaTracks::GetNumTrackFormats(EMediaTrackType TrackType, int32 TrackIndex) const
{
    switch(TrackType)
    {
        case EMediaTrackType::Video:
            return 1;

        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return 0;
}

int32
FOdysseyAnimationMediaTracks::GetTrackFormat(EMediaTrackType TrackType, int32 TrackIndex) const
{
    //Gets selected format index for the given track type and track index
    switch(TrackType)
    {
        case EMediaTrackType::Video:
            return 0;

        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return INDEX_NONE;
}

int32
FOdysseyAnimationMediaTracks::GetSelectedTrack(EMediaTrackType TrackType) const
{
    switch(TrackType)
    {
        case EMediaTrackType::Video:
            return 0;

        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return INDEX_NONE;
}

FText
FOdysseyAnimationMediaTracks::GetTrackDisplayName(EMediaTrackType TrackType, int32 TrackIndex) const
{
    switch(TrackType)
    {
        case EMediaTrackType::Video:
            return LOCTEXT("media-tracks.default-video-track-name", "Video Track");

        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return FText::GetEmpty();
}

FString
FOdysseyAnimationMediaTracks::GetTrackLanguage(EMediaTrackType TrackType, int32 TrackIndex) const
{
    switch(TrackType)
    {
        case EMediaTrackType::Video:
        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return FString();
}

FString
FOdysseyAnimationMediaTracks::GetTrackName(EMediaTrackType TrackType, int32 TrackIndex) const
{
    switch(TrackType)
    {
        case EMediaTrackType::Video:
            return TEXT("VideoTrack");

        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }

    return FString();
}

bool
FOdysseyAnimationMediaTracks::GetAudioTrackFormat(int32 TrackIndex, int32 FormatIndex, FMediaAudioTrackFormat& OutFormat) const
{
    return false; // not supported yet
}

bool
FOdysseyAnimationMediaTracks::GetVideoTrackFormat(int32 TrackIndex, int32 FormatIndex, FMediaVideoTrackFormat& OutFormat) const
{
    TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
    if ( !player )
        return false;

    if (TrackIndex != 0 || FormatIndex != 0)
        return false;

    UOdysseyAnimation* animation = player->GetAnimation();
    if ( !animation )
        return false;

    OutFormat.Dim = FIntPoint(animation->GetWidth(), animation->GetHeight());
    OutFormat.FrameRate = animation->GetFramesPerSecond();
    OutFormat.FrameRates = TRange<float>::Inclusive(OutFormat.FrameRate, OutFormat.FrameRate);
    OutFormat.TypeName = TEXT("Animation");

    return true;
}

bool
FOdysseyAnimationMediaTracks::SelectTrack(EMediaTrackType TrackType, int32 TrackIndex)
{
    //no need to implement track selection as we never have more than one track for each type
    switch(TrackType)
    {
        case EMediaTrackType::Video:
        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return true;
}

bool
FOdysseyAnimationMediaTracks::SetTrackFormat(EMediaTrackType TrackType, int32 TrackIndex, int32 FormatIndex)
{
    //no need to implement format selection as we never have more than one format for each track
    switch(TrackType)
    {
        case EMediaTrackType::Video:
        case EMediaTrackType::Audio:
        case EMediaTrackType::Caption:
        case EMediaTrackType::Metadata:
        case EMediaTrackType::Script:
        case EMediaTrackType::Subtitle:
        case EMediaTrackType::Text:
        default:
            break;
    }
    return true;
}

#undef LOCTEXT_NAMESPACE
