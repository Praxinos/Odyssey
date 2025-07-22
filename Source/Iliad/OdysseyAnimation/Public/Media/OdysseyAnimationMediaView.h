// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IMediaView.h"

class ODYSSEYANIMATION_API FOdysseyAnimationMediaView
    : public IMediaView
{
public:
    FOdysseyAnimationMediaView();

public:
    void Init(TWeakPtr<class FOdysseyAnimationMediaPlayer> iPlayer);
    void OnOpen();
    void OnClose();

private:
    TWeakPtr<class FOdysseyAnimationMediaPlayer> mPlayer;
};
