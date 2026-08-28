// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ArianeLayerStack.generated.h"

class UArianeLayer;
class UArianeLayerDrawing;
class UArianeLayerFolder;
class UArianeLayerFolder;
class UArianePainting3DComponent;

UCLASS()
class ARIANE_API UArianeLayerStack : public USceneComponent
{
    GENERATED_BODY()

    DECLARE_MULTICAST_DELEGATE( FOnHierarchyChanged );
    DECLARE_MULTICAST_DELEGATE( FOnSelectionChanged );

public:
    ~UArianeLayerStack();
    // Legacy compatibility
    UArianeLayerStack();

    // overrides
    virtual void OnComponentDestroyed( bool bDestroyingHierarchy ) override;
    virtual void PostLoad() override;

    /**
    * @brief Get the top-most (root) folder
    * @return the top-most (root) folder
    */
    UArianeLayerFolder* GetRootFolder();

    /** Remove selected layers **/
    void RemoveSelectedLayers();

    /** Select all layers **/
    void SelectAllLayers();

    /**
    * @brief Select a layer
    * @param bTriggerevent trigger selection changed delegates
    */
    void SelectLayer( UArianeLayer* Layer, bool bTriggerevent );

    /**
    * @brief Select a layer
    * @param bClearSelectionFirst true to clear the selection first, flase otherwise
    * @param bTriggerevent trigger selection changed delegates
    */
    void SelectLayers( const TArray<UArianeLayer*> LayerSelection, bool bClearSelectionFirst, bool bTriggerevent );

    /**
    * @brief Clears the layer selection
    * @param bTriggerevent trigger selection changed delegates
    */
    void ClearLayerSelection( bool bTriggerEvent );

    /**
    * @brief Get all selected layers
    * @return an array of selected layers
    */
    const TArray<UArianeLayer*>& GetSelectedLayers();

    /**
    * @brief Get the current layer (i.e the last selected layer)
    * @return the current layer
    */
    UArianeLayer* GetCurrentLayer();

    /**
    * @brief Create a new drawing layer
    * @param ParentLayerFolder the parent folder
    * @param bTriggerEvent trigger layer stack changed event
    * @return the newly created layer
    */
    UArianeLayerDrawing* CreateDrawingLayer( UArianeLayerFolder* ParentLayerFolder, bool bTriggerEvent );

    /**
    * @brief Create a new folder layer
    * @param ParentLayerFolder the parent folder
    * @param bTriggerEvent trigger layer stack changed event
    * @return the newly created layer
    */
    UArianeLayerFolder* CreateFolderLayer( UArianeLayerFolder* ParentLayerFolder, bool bTriggerEvent );

    /**
    * @brief Get the Painting3D Component
    * @return the Painting3D Component
    */
    UArianePainting3DComponent* GetPainting3DComponent();

    /**
    * @brief Get all layers of all type
    * @param the Painting3D Component
    */
    void GetLayers( TArray<UArianeLayer*>& OutLayers );

    void AddLayer( UArianeLayerFolder* FosterFolder, UArianeLayer* OrphanLayer, bool bTriggerEvent );

    void AppendSelectedTrees( TArray<UArianeLayer*>& SelectedTrees );
    void GetSelectedTrees( TArray<UArianeLayer*>& SelectedTrees );

    FOnHierarchyChanged& OnPreHierarchyChangedDelegate();
    FOnHierarchyChanged& OnPostHierarchyChangedDelegate();

    FOnSelectionChanged& OnPreSelectionChangedDelegate();
    FOnSelectionChanged& OnPostSelectionChangedDelegate();

    virtual void Serialize( FArchive& Ar ) override;
    void AddLayers( UArianeLayerFolder* FosterFolder, TArray<UArianeLayer*> OrphanLayers, bool bTriggerEvent );

#if WITH_EDITOR
    void PreEditUndo();
    void PostEditUndo();
#endif

protected:
    void SelectLayer_Private( UArianeLayer* Layer );

protected:
    UPROPERTY()
    UArianeLayerFolder* RootFolder;

    //UPROPERTY(Transient)
    TArray<UArianeLayer*> SelectedLayers;

protected:
    FOnHierarchyChanged OnPreHierarchyChanged;
    FOnHierarchyChanged OnPostHierarchyChanged;

    FOnSelectionChanged OnPreSelectionChanged;
    FOnSelectionChanged OnPostSelectionChanged;
};
