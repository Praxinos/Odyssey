// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Channels/MovieSceneChannelHandle.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "KeyDrawParams.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

//---

struct FMetaSubKey
{
    FMovieSceneChannelHandle    mChannelHandle;
    FKeyHandle                  mKeyHandle;
    FKeyDrawParams              mKeyDrawParam;
    FFrameTime                  mOffset;
};

struct FMetaKey
{
    enum class EFlags
    {
        kNone,
        kPartial    = 1 << 0,
    };

    TArray<FMetaSubKey>     mSubKeys;
    EFlags                  mFlags;
    FKeyDrawParams          mMetaKeyDrawParam;
};

struct FMetaChannelProxy
{
    TMap<FFrameNumber, FMetaKey> mMetaKeys;
};
