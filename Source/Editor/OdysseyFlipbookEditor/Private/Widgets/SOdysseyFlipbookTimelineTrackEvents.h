// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyTimelineFrameListEvents.h"
#include "PaperFlipbook.h"

DECLARE_DELEGATE(FOnStructureChanged)
DECLARE_DELEGATE(FOnFlipbookChanged)
DECLARE_DELEGATE_OneParam(FOnKeyframeRemoved, FPaperFlipbookKeyFrame&)
DECLARE_DELEGATE_OneParam(FOnKeyframeAdded, FPaperFlipbookKeyFrame&)
DECLARE_DELEGATE_OneParam(FOnSpriteCreated, UPaperSprite*)
DECLARE_DELEGATE_OneParam(FOnTextureCreated, UTexture2D*)
