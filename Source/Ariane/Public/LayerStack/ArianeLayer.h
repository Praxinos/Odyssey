// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane headers
#include "ArianeLayerInvalidationFlags.h"

#include "ArianeLayer.generated.h"

class UArianeLayerStack;
class UArianeLayerFolder;

UCLASS(Abstract)
class ARIANE_API UArianeLayer : public USceneComponent
{
    GENERATED_BODY()

public:
    ~UArianeLayer();
    UArianeLayer();

    DECLARE_MULTICAST_DELEGATE_OneParam( FOnUpdateDelegate, bool );

    //void SetVisible( bool bInVisible );
    //bool IsVisible( bool bHierarchical );
    void SetLocked( bool bInLocked );
    bool IsLocked( bool bHierarchical );
    virtual void Invalidate( const FArianeLayerInvalidationFlags& InInvalidationFlags );
    virtual void Update( bool bInteractive );

#if WITH_EDITORONLY_DATA
    virtual bool IsSelectedInEditor() const override;
    void SetSelected( bool bInSelected );
#endif

    bool IsInvalidated();
    void SetInvalidated( bool bInInvalidate );
    const FBoxSphereBounds& GetBounds();
    UArianeLayerStack* GetLayerStack();
    UArianeLayerFolder* GetRootFolder();
    FOnUpdateDelegate& OnPreUpdateDelegate();
    FOnUpdateDelegate& OnPostUpdateDelegate();
    void SetParentFolder( UArianeLayerFolder* InParentFolder );
    UArianeLayerFolder* GetParentFolder();

protected:
    //UPROPERTY()
    //bool bVisible;
    UPROPERTY()
    UArianeLayerFolder* ParentFolder;

    UPROPERTY()
    bool bLocked;

#if WITH_EDITORONLY_DATA
    UPROPERTY(Transient)
    bool bSelected;
#endif

protected:
    bool bInvalidated;
    FBoxSphereBounds Bounds;
    FArianeLayerInvalidationFlags* InvalidationFlags;
    FOnUpdateDelegate OnPreUpdate;
    FOnUpdateDelegate OnPostUpdate;
};
