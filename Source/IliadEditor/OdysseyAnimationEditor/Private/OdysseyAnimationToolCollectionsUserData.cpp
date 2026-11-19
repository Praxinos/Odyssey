// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyAnimationToolCollectionsUserData.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationUserDataUtils.h"

UOdysseyAnimationToolCollectionsUserData*
UOdysseyAnimationToolCollectionsUserData::FindOrCreate(const UOdysseyAnimation* iAnimation)
{
    UOdysseyAnimationToolCollectionsUserData* userData = OdysseyAnimationUserData::FindOrAdd<UOdysseyAnimationToolCollectionsUserData>(iAnimation);
    if (userData)
        userData->MigrateLegacyData();

    return userData;
}

void
UOdysseyAnimationToolCollectionsUserData::MigrateLegacyData()
{
    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(GetOuter());
    if (!animation || !animation->HasLegacyToolCollections())
        return;

    for (const FSoftObjectPath& toolCollection : animation->TakeLegacyToolCollections())
        ToolCollections.AddUnique(toolCollection);
}
