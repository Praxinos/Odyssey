// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyAnimationPalettesUserData.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationUserDataUtils.h"

UOdysseyAnimationPalettesUserData*
UOdysseyAnimationPalettesUserData::FindOrCreate(const UOdysseyAnimation* iAnimation)
{
    UOdysseyAnimationPalettesUserData* userData = OdysseyAnimationUserData::FindOrAdd<UOdysseyAnimationPalettesUserData>(iAnimation);
    if (userData)
        userData->MigrateLegacyData();

    return userData;
}

void
UOdysseyAnimationPalettesUserData::MigrateLegacyData()
{
    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(GetOuter());
    if (!animation || !animation->HasLegacyPalettes())
        return;

    for (UOdysseyPaletteSet* paletteSet : animation->TakeLegacyPalettes())
        Palettes.AddUnique(paletteSet);
}
