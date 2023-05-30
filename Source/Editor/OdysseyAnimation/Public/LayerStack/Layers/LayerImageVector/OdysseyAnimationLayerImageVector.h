// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Image/OdysseyBlendingMode.h"

#include <ULIS>

#include "OdysseyVector.h"

#include "OdysseyAnimationLayerImageVector.generated.h"

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerImageVector
    : public UOdysseyAnimationLayer
{
    GENERATED_BODY()

public:
    ~UOdysseyAnimationLayerImageVector();
    UOdysseyAnimationLayerImageVector();

public:
    FOdysseyVectorEngine* GetEngine();
    FOdysseyVectorScene* GetScene();

public:
    FOdysseyVectorScene* mScene;
    FOdysseyVectorEngine* mVEngine;
};
