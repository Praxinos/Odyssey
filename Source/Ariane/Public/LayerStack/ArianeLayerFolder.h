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
    const TArray<UArianeLayer*>& GetChildren();
    void AddChild( UArianeLayer* Orphan );
    void RemoveChild( UArianeLayer* Child );
    void InvalidateChild( UArianeLayer* Child );
    virtual void Update() override;

public:
    void Traverse( TFunction<TraversalReturnValue(UArianeLayer*)> Callback );

protected:
    TraversalReturnValue Traverse_Private( TFunction<TraversalReturnValue(UArianeLayer*)> Callback );
    void UpdateBounds();

protected:
    UPROPERTY()
    TArray<UArianeLayer*> Children;

    UPROPERTY()
    bool bExpanded;

protected:
    TArray<UArianeLayer*> InvalidatedChildren;
};
