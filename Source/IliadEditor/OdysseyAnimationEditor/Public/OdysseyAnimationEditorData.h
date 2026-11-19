// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"

class UOdysseyAnimation;
class UOdysseyAnimationLayerStack;
class UOdysseyPaletteSet;

/**
 * Editor-only content of a UOdysseyAnimation.
 *
 * The runtime animation cannot expose editor types, so its editor content is stored in asset user data
 * owned by this module. These accessors hide those user data: they create them when needed, and move
 * the legacy content still stored on the animation into them.
 */
namespace OdysseyAnimationEditor
{
    /**
     * @brief Returns the layer stack of an animation, creating an empty one if needed
     *
     * @param Animation The animation owning the layer stack
     * @return The layer stack of the animation, or nullptr if Animation is null
     */
    ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationLayerStack* GetLayerStack(const UOdysseyAnimation* Animation);

    /**
     * @brief Returns the palette sets used by the vector layers of an animation
     *
     * @param Animation The animation owning the palette sets
     * @return The palette sets of the animation, which can be modified
     */
    ODYSSEYANIMATIONEDITOR_API TArray<UOdysseyPaletteSet*>& GetPalettes(UOdysseyAnimation& Animation);

    /**
     * @brief Returns the paths to the tool collections used with an animation
     *
     * @param Animation The animation owning the tool collections
     * @return The tool collection paths of the animation, which can be modified
     */
    ODYSSEYANIMATIONEDITOR_API TArray<FSoftObjectPath>& GetToolCollections(UOdysseyAnimation& Animation);
}
