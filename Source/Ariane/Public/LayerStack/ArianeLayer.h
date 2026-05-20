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

#if WITH_EDITOR
    DECLARE_MULTICAST_DELEGATE( FOnTransformChanged );
#endif

    void SetLocked( bool bInLocked );
    bool IsLocked( bool bHierarchical );
    virtual void Invalidate( const FArianeLayerInvalidationFlags& InInvalidationFlags );
    virtual void Update( bool bInteractive );

    // Overriden from USceneComponent::OnUpdateTransform
    virtual void OnUpdateTransform(EUpdateTransformFlags Flags, ETeleportType Teleport) override;

#if WITH_EDITOR
    virtual bool IsSelectedInEditor() const override;
    void SetSelected( bool bInSelected );
#endif

    bool IsInvalidated();
    void SetInvalidated( bool bInInvalidate );

    /**
     * @brief Get the boundaries of the layer (i.e its content)
     * @return the boundaries
     */
    const FBoxSphereBounds& GetBounds();

    /**
     * @brief Get the layer stack associated to this layer
     * @return the layer stack
     */
    UArianeLayerStack* GetLayerStack();

    /**
     * @brief Get the root folder, i.e the top-level folder
     * @return the root folder
     */
    UArianeLayerFolder* GetRootFolder();

    /** The delegate run just before the update **/
    FOnUpdateDelegate& OnPreUpdateDelegate();

    /** The delegate run right after the update **/
    FOnUpdateDelegate& OnPostUpdateDelegate();

    /**
     * @brief Set the parent folder
     * @param InParentFolder the parent folder
     */
    void SetParentFolder( UArianeLayerFolder* InParentFolder );

    /**
     * @brief Get the parent folder
     * @return the parent folder
     */
    UArianeLayerFolder* GetParentFolder();

    /** The delegate run when the transform is changed **/
    UArianeLayer::FOnTransformChanged& GetOnTransformChangedDelegate();

protected:
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

#if WITH_EDITORONLY_DATA
    FOnTransformChanged OnTransformChanged;
#endif
};
