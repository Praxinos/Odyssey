// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "UObject/Object.h"
// Ariane headers
#include "ArianeLayerInvalidationFlags.h"

#include "ArianeLayer.generated.h"

class UArianeLayerStack;
class UArianeLayer;
class UArianeLayerFolder;
class UArianeLayerDrawing;

UCLASS(Abstract)
class ARIANE_API UArianeLayer : public USceneComponent
{
    GENERATED_BODY()

public:
    enum class ETraversalReturnValue{ Continue, IgnoreChildren, Stop };

public:
    ~UArianeLayer();
    UArianeLayer();

    //DECLARE_MULTICAST_DELEGATE_OneParam( FOnUpdateDelegate, bool );

#if WITH_EDITOR
    DECLARE_MULTICAST_DELEGATE( FOnTransformChanged );
#endif

    /**
     * @brief Set the layer as locked or unlocked
     * @param bInLocked true for locking, false otherwise
     */
    void SetLocked( bool bInLocked );

    /**
     * @brief Get the locking status of the layer
     * @param bHierarchical true if checking the whole hierarchical chain is needed, false otherwise
     * @return true or false
     */
    bool IsLocked( bool bHierarchical );

    /**
     * @brief Invalidate the layer
     * @param InInvalidationFlags invalidation flags
     */
    virtual void Invalidate( const FArianeLayerInvalidationFlags& InInvalidationFlags );

    /**
     * @brief Update the layer
     * @param bInteractive is the update interactive (during a mouse drag)
     */
    virtual void Update( bool bInteractive );

    // Overriden from USceneComponent::OnUpdateTransform
    virtual void OnUpdateTransform(EUpdateTransformFlags Flags, ETeleportType Teleport) override;

#if WITH_EDITOR
    virtual bool IsSelectedInEditor() const override;
    void SetSelected( bool bInSelected );
#endif

    /**
     * @brief Set the invalidation status of the layer
     * @return true or false
     */
    void SetInvalidatedInParentFolder( bool bInInvalidatedInParentFolder );

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
    //FOnUpdateDelegate& OnPreUpdateDelegate();

    /** The delegate run right after the update **/
    //FOnUpdateDelegate& OnPostUpdateDelegate();

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

    FArianeLayerInvalidationFlags* GetInvalidationFlags();

    void TraverseBackwards( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback );
    void PreEditUndo();
    void PostEditUndo();
    void PostLoad();


protected:
    ETraversalReturnValue TraverseBackwards_Private( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback );

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
    bool bInvalidatedInParentFolder;
    FArianeLayerInvalidationFlags* InvalidationFlags;
    //FOnUpdateDelegate OnPreUpdate;
    //FOnUpdateDelegate OnPostUpdate;

#if WITH_EDITORONLY_DATA
    FOnTransformChanged OnTransformChanged;
#endif
};
