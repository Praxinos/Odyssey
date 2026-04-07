// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeLayer.generated.h"

class UArianeLayerStack;
class UArianeLayerFolder;
class UArianeLayerFolder;

UCLASS(Abstract)
class ARIANE_API UArianeLayer : public USceneComponent
{
    GENERATED_BODY()

public:
    ~UArianeLayer();
    UArianeLayer();

    UArianeLayerFolder* GetParent();

    //void SetVisible( bool bInVisible );
    //bool IsVisible( bool bHierarchical );
    void SetLocked( bool bInLocked );
    bool IsLocked( bool bHierarchical );
    void SetSelected( bool bInSelected );
    virtual void Invalidate();
    virtual void Update();

    #if WITH_EDITOR
    virtual bool IsSelectedInEditor() const override;
    #endif
    bool IsInvalidated();
    void SetInvalidated( bool bInInvalidate );
    const FBoxSphereBounds& GetBounds();
    UArianeLayerStack* GetLayerStack();
    UArianeLayerFolder* GetRootFolder();

protected:
    //UPROPERTY()
    //bool bVisible;

    UPROPERTY()
    bool bLocked;

    UPROPERTY()
    bool bSelected;

protected:
    bool bInvalidated;
    FBoxSphereBounds Bounds;
};
