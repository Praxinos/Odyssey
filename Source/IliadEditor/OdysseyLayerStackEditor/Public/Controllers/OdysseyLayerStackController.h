// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UOdysseyLayerStack;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackController
{
public:
    FOdysseyLayerStackController(UOdysseyLayerStack* iLayerStack);

public:
    UOdysseyLayerStack* GetLayerStack() const;

private:
    UOdysseyLayerStack* mLayerStack;
};
