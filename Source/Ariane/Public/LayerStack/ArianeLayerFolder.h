// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianeLayer.h"

#include "ArianeLayerFolder.generated.h"

UCLASS()
class ARIANE_API UArianeLayerFolder : public UArianeLayer
{
    GENERATED_BODY()

public:
    enum class TraversalReturnValue{ Continue, IgnoreChildren, Stop };

public:
    ~UArianeLayerFolder();
    UArianeLayerFolder();

    void SetExpanded( bool bInExpanded );
    bool IsExpanded();
    const TArray<UArianeLayer*>& GetChildLayers();
    void AddChildLayer( UArianeLayer* Orphan );
    void RemoveChildLayer( UArianeLayer* Child );
    void InvalidateChildLayer( UArianeLayer* Child );
    virtual void Update( bool bInteractive ) override;

public:
    void Traverse( TFunction<TraversalReturnValue(UArianeLayer*)> Callback );

protected:
    TraversalReturnValue Traverse_Private( TFunction<TraversalReturnValue(UArianeLayer*)> Callback );
    void UpdateBounds();

protected:
    // Even though unreal has the list of attached Actors, it does not take the order into consideration.
    // so we have to have our own list
    UPROPERTY()
    TArray<UArianeLayer*> ChildLayers;

    UPROPERTY()
    bool bExpanded;

protected:
    TArray<UArianeLayer*> InvalidatedChildLayers;
};
