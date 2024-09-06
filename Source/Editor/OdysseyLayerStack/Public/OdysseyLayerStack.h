// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPerformanceMode.h"
#include "OdysseyImageRenderingAbility.h"
#include "OdysseyLayerStack.generated.h"

UCLASS(Abstract, BlueprintType, config=EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYLAYERSTACK_API UOdysseyLayerStack
    : public UObject
	, public FOdysseyImageRenderingAbility
{
    GENERATED_BODY()

public:

    /* Called when the current layer changed */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentLayerChanged, UOdysseyLayerStack*)

    /* Called when the Layer hierarchy changed at some point */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnHierarchyChanged, UOdysseyLayerStack*);

public:
    //Delegates

    /**
     * @brief Returns the CurrentLayerChanged delegate
     */
    static FOnCurrentLayerChanged& OnCurrentLayerChanged();

    /**
     * @brief Returns the HierarchyChanged delegate
     */
    static FOnHierarchyChanged& OnHierarchyChanged();

public:
    //Layer Class Support
    //UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    //TArray<UClass*> FindSupportedCustomLayerClasses() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    bool SupportsLayerClass(UClass* Class) const;

public:
	//GETTER/SETTER
	UFUNCTION(BlueprintSetter)
	void CurrentLayerBlueprintSetter(TSoftObjectPtr<UOdysseyLayer> Layer);

public:
    //Layers management

    /**
     * @brief Adds a Layer of LayerType to the LayerStack as child of ParentLayer at IndexInParent
     * 
     * @param LayerType has to be a layer type compatible with this layerstack
     * @param ParentLayer has to be a layer from this layerstack or nullptr
     * @param IndexInParent 
     * @return UOdysseyLayer* 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack", meta=(DeterminesOutputType="LayerType"))
    UOdysseyLayer* AddLayer(TSubclassOf<UOdysseyLayer> LayerType, UOdysseyLayer* ParentLayer = nullptr, int IndexInParent = 0);

    /**
     * @brief Removes Layer from the LayerStack
     * 
     * @param Layer has to be a layer from this layerstack
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    void RemoveLayer(UOdysseyLayer* Layer);

    /**
     * @brief Removes Layers from the LayerStack
     * 
     * @param Layers has to be layers from this layerstack
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    void RemoveLayers(TArray<UOdysseyLayer*> Layers);

    /**
     * @brief Returns wether the given layer is contained in the layerstack
     * 
     * @param Layer 
     * @return bool
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    bool ContainsLayer(const UOdysseyLayer* Layer) const;

    /**
     * @brief Returns the root layers only
     * 
     * @return TArray<UOdysseyLayer*>
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    const TArray<UOdysseyLayer*>& GetRootLayers() const;

    /**
     * @brief Returns all Layers
     * 
     * @return TArray<UOdysseyLayer*>
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    TArray<UOdysseyLayer*> GetLayers() const;

    /**
     * @brief Duplicates Layer and position the resulting layer directly above Layer
     * 
     * @param Layer has to be a layer from this layerstack
     * @return UOdysseyLayer* 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack", meta=(DeterminesOutputType="Layer"))
    UOdysseyLayer* DuplicateLayer(UOdysseyLayer* Layer);

    /**
     * @brief Duplicates Layers and position the resulting layers directly above each Layers
     * 
     * @param Layers
     * @return UOdysseyLayer* 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack", meta=(DeterminesOutputType="Layer"))
    TArray<UOdysseyLayer*> DuplicateLayers(TArray<UOdysseyLayer*> Layers);

    /**
     * @brief Copies Layer and adds the resulting layer as child of ParentLayer at IndexInParent
     * 
     * @param Layer can be a layer from another layerstack
     * @param ParentLayer has to be a layer from this layerstack or nullptr
     * @param IndexInParent 
     * @return UOdysseyLayer* 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack", meta=(DeterminesOutputType="Layer"))
    UOdysseyLayer* CopyLayer(UOdysseyLayer* Layer, UOdysseyLayer* ParentLayer = nullptr, int IndexInParent = 0);

    /**
     * @brief Copies Layer and adds the resulting layer as child of ParentLayer at IndexInParent
     * 
     * @param Layer can be a layer from another layerstack
     * @param ParentLayer has to be a layer from this layerstack or nullptr
     * @param IndexInParent 
     * @return UOdysseyLayer* 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack", meta=(DeterminesOutputType="Layer"))
    TArray<UOdysseyLayer*> CopyLayers(TArray<UOdysseyLayer*> Layers, UOdysseyLayer* ParentLayer = nullptr, int IndexInParent = 0);

    /**
     * @brief Returns a set of layer classes that should be created when merging
     * 
     * @param iLayers 
     * @return TSet<UClass*> 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    TSet<UClass*> FindLayersMergeTypes(TArray<UOdysseyLayer*> Layers);

    /**
     * @brief Return wether the given layers can be merged together
     * 
     * @param iLayers 
     * @return true 
     * @return false 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    bool CanMergeLayers(TArray<UOdysseyLayer*> Layers);

    /**
     * @brief Merges the given layers together
     * 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    UOdysseyLayer* MergeLayers(TArray<UOdysseyLayer*> Layers);

    /**
     * @brief Return wether the given layer can be flattened
     * 
     * @param iLayer 
     * @return true 
     * @return false 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    bool CanFlattenLayer(UOdysseyLayer* Layer);

    /**
     * @brief Flattens the given layer
     * 
     * @param iLayer 
     * @return TArray<UOdysseyLayer*> 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    TArray<UOdysseyLayer*> FlattenLayer(UOdysseyLayer* Layer);

    /**
     * @brief Return wether the given layers can be flattened
     * 
     * @param iLayers 
     * @return true 
     * @return false 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    bool CanFlattenLayers(TArray<UOdysseyLayer*> Layers);

    /**
     * @brief Flattens the given layers
     * 
     * @param iLayers 
     * @return TArray<UOdysseyLayer*> 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    TArray<UOdysseyLayer*> FlattenLayers(TArray<UOdysseyLayer*> Layers);
    
    /**
     * @brief Returns wether the given layer can be moved in the given ParentLayer
     * 
     * @param Layer has to be a layer from this layerstack
     * @param ParentLayer has to be a layer from this layerstack or nullptr
     * @param IndexInParent 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    bool CanMoveLayer(UOdysseyLayer* Layer, UOdysseyLayer* ParentLayer) const;

	/**
	 * @brief Returns wether the given layers can be moved in the given ParentLayer
	 *
	 * @param Layers 
	 * @param ParentLayer has to be a layer from this layerstack or nullptr
	 * @param IndexInParent
	 */
	UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
	bool CanMoveLayers(TArray<UOdysseyLayer*> Layers, UOdysseyLayer* ParentLayer) const;

    /**
     * @brief Moves Layer to become child of ParentLayer at IndexInParent
     * 
     * @param Layer has to be a layer from this layerstack
     * @param ParentLayer has to be a layer from this layerstack or nullptr
     * @param IndexInParent 
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    void MoveLayer(UOdysseyLayer* Layer, UOdysseyLayer* ParentLayer = nullptr, int IndexInParent = 0);

	/**
	 * @brief Moves Layers to become children of ParentLayer at IndexInParent
	 *
	 * @param Layers 
	 * @param ParentLayer has to be a layer from this layerstack or nullptr
	 * @param IndexInParent
	 */
	UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
	void MoveLayers(TArray<UOdysseyLayer*> Layers, UOdysseyLayer* ParentLayer = nullptr, int IndexInParent = 0);

protected:
    //Property changed methods
    void CurrentLayerChanged();
    virtual void PropertyChanged(const FName& iPropertyName);public:
    
public:
	//Called by layers when there Parent or Children changed
    void HierarchyChanged();

public:
    // UObject overrides

	/**
	 * Called when a property on this object has been modified
	 *
	 * @param PropertyThatChanged the property that was modified
	 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * @brief Allows us to know which property changed on an undo/redo
     * PostEditChangeProperty does not inform us of that on undo/redo
     * 
     * @param TransactionEvent 
     */
    virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;

    virtual void PostInitProperties() override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;

protected:
    //Internal
    UOdysseyLayer* CreateLayer(UClass* iLayerType);

    void AddLayersToHierarchy(TArray<UOdysseyLayer*> iLayers, UOdysseyLayer* iParent, int iIndexInParent);

    void RemoveLayersFromHierarchy(TArray<UOdysseyLayer*> iLayers);

    UOdysseyLayer* CopyLayerInternal(UOdysseyLayer* iLayer, UOdysseyLayer* iParent, int iIndexInParent);
    
    void GetLayersUniqueParents(TArray<UOdysseyLayer*> iLayers, TArray<UOdysseyLayer*>& oParents);

public:
    //Default properties
    UPROPERTY(Transient)
    TSet<UClass*> CompatibleLayers; //Contains compatible C++ layer types
	
	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LayerStack", BlueprintSetter=CurrentLayerBlueprintSetter)
	TSoftObjectPtr<UOdysseyLayer> CurrentLayer;

    UPROPERTY()
    TObjectPtr<UOdysseyLayer> LayerRoot;

    UPROPERTY(Transient)
    TObjectPtr<UClass> LayerRootClass;
};