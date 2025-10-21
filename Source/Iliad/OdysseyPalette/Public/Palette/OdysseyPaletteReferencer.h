// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "UObject/Interface.h"
#include "OdysseyPaletteReferencer.generated.h"

class UOdysseyPalette;

UINTERFACE(Blueprintable)
class ODYSSEYPALETTE_API UOdysseyPaletteReferencer : public UInterface
{
    GENERATED_BODY()
};

class ODYSSEYPALETTE_API IOdysseyPaletteReferencer
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    virtual void OnRefreshReferencedPalette(UOdysseyPalette* iPalette) = 0;
#endif
};
