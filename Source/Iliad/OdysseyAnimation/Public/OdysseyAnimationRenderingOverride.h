// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "OdysseyAnimationRenderingOverride.generated.h"

class IOdysseyTextureRenderingAbility;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UOdysseyAnimationRenderingOverride : public UInterface
{
    GENERATED_BODY()
};

/**
 * @brief Overrides, in editor, the rendering of the UOdysseyAnimation owning the asset user data implementing it
 *
 * At runtime, an animation only plays its baked frames. In editor, it renders, composes and bounds itself through
 * the rendering ability returned by the first of its asset user data implementing this interface, so that edits
 * are displayed live. The layer stack user data, owned by an editor module, implements it.
 */
class ODYSSEYANIMATION_API IOdysseyAnimationRenderingOverride
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    /**
     * @brief Returns the rendering ability rendering the animation in editor
     *
     * @return The overriding rendering ability, or nullptr to not override the animation rendering
     */
    virtual const IOdysseyTextureRenderingAbility* GetAnimationRenderingOverride() const = 0;
#endif
};
