// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "OdysseyAnimationEditorData.generated.h"

class IOdysseyTextureRenderingAbility;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UOdysseyAnimationEditorData : public UInterface
{
    GENERATED_BODY()
};

/**
 * @brief Editor-only content attached to a UOdysseyAnimation as an asset user data.
 *
 * The runtime animation only plays its baked frames. In editor, it reaches its editable content
 * (the layer stack, owned by an editor module) through this interface, without depending on that module.
 */
class ODYSSEYANIMATION_API IOdysseyAnimationEditorData
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    /**
     * @brief Returns the rendering source of the editable animation content
     *
     * @return The source used to render, compose and bound the animation in editor, or nullptr if there is none
     */
    virtual const IOdysseyTextureRenderingAbility* GetAnimationRenderingSource() const = 0;

    /**
     * @brief Returns the name of the layer currently edited in the animation
     *
     * @return The current layer name, or an empty text if there is no current layer
     */
    virtual FText GetCurrentLayerName() const = 0;
#endif
};
