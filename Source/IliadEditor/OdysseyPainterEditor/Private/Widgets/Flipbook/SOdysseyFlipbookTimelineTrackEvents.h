// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbook.h"
#include "SOdysseyFlipbookTimelineFrameListEvents.h"
#include "Texture/SOdysseyTextureConfigureWindow.h"

DECLARE_DELEGATE(FOnStructureChanged)
DECLARE_DELEGATE(FOnFlipbookChanged)
DECLARE_DELEGATE_OneParam(FOnKeyframeRemoved, FPaperFlipbookKeyFrame&)
DECLARE_DELEGATE_OneParam(FOnKeyframeAdded, FPaperFlipbookKeyFrame&)
DECLARE_DELEGATE_OneParam(FOnSpriteCreated, UPaperSprite*)
DECLARE_DELEGATE_TwoParams(FOnTextureCreated, UTexture2D*, FOdysseyTextureConfiguration)
