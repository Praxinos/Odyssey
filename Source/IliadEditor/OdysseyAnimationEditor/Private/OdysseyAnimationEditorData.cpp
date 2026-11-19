// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyAnimationEditorData.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerStackUserData.h"
#include "OdysseyAnimationPalettesUserData.h"
#include "OdysseyAnimationToolCollectionsUserData.h"

namespace OdysseyAnimationEditor
{

UOdysseyAnimationLayerStack*
GetLayerStack(const UOdysseyAnimation* Animation)
{
    UOdysseyAnimationLayerStackUserData* userData = UOdysseyAnimationLayerStackUserData::FindOrCreate(Animation);
    return userData ? userData->LayerStack : nullptr;
}

TArray<UOdysseyPaletteSet*>&
GetPalettes(UOdysseyAnimation& Animation)
{
    return UOdysseyAnimationPalettesUserData::FindOrCreate(&Animation)->Palettes;
}

TArray<FSoftObjectPath>&
GetToolCollections(UOdysseyAnimation& Animation)
{
    return UOdysseyAnimationToolCollectionsUserData::FindOrCreate(&Animation)->ToolCollections;
}

} // namespace OdysseyAnimationEditor
