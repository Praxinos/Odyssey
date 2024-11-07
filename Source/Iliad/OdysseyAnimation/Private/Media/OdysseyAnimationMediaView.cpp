// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaView.h"

#include "Media/OdysseyAnimationMediaPlayer.h"

FOdysseyAnimationMediaView::FOdysseyAnimationMediaView()
{
}

void
FOdysseyAnimationMediaView::Init(TWeakPtr<FOdysseyAnimationMediaPlayer> iPlayer)
{
    mPlayer = iPlayer;
}

void
FOdysseyAnimationMediaView::OnOpen()
{

}

void
FOdysseyAnimationMediaView::OnClose()
{

}
