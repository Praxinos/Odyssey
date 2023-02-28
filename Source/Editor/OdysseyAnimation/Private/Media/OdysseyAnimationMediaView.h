// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaView.h"

class FOdysseyAnimationMediaView
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
