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
    ~UArianeLayerFolder();
    UArianeLayerFolder();

    virtual void Update( bool bInteractive ) override;

    /**
     * @brief Set the expansion status of the folder's tree
     * @param bInExpanded true for expanded, false otherwise
     */
    void SetExpanded( bool bInExpanded );

    /** Get the expansion status of the folder's tree **/
    bool IsExpanded();

    /** Get child layers **/
    const TArray<UArianeLayer*>& GetChildLayers();

    /**
     * @brief Add a child layer to this folder
     * @param Orphan the child layer
     */
    void AddChildLayer( UArianeLayer* Orphan );

    /**
     * @brief Remove a child layer from this folder
     * @param Child the child layer
     */
    void RemoveChildLayer( UArianeLayer* Child );

    /**
     * @brief Invalidate a child layer
     * @param Child the child layer
     */
    void InvalidateChildLayer( UArianeLayer* Child );

    /**
     * @brief Recursively execute a function
     * @param Callback the function to run
     */
    void Traverse( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback );

    //virtual FBoxSphereBounds CalcBounds( const FTransform& LocalToWorld ) const override;

protected:
    ETraversalReturnValue Traverse_Private( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback );


protected:
    UPROPERTY()
    bool bExpanded;

    // Even though unreal has the list of attached Actors, it does not take the order into consideration.
    // so we have to have our own list. "Instanced" keyword will allow deep copy

    UPROPERTY(Instanced)
    TArray<UArianeLayer*> ChildLayers;

protected:
    TArray<UArianeLayer*> InvalidatedChildLayers;
};
