// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTextureRenderingAbility.h"
#include "Templates/SubclassOf.h"

#include "OdysseyLayerStack.generated.h"

class FOdysseyLayerCellSelection;

UCLASS(Abstract, HideDropdown, BlueprintType, config=EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYLAYERSTACK_API UOdysseyLayerStack
    : public UObject
    , public IOdysseyTextureRenderingAbility
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    /* Called when the current layer changed */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentLayerChanged, UOdysseyLayerStack*)
#endif

    /* Called when the Layer hierarchy changed at some point */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnHierarchyChanged, UOdysseyLayerStack*);

public:
    //Delegates

#if WITH_EDITOR
    /**
     * @brief Returns the CurrentLayerChanged delegate
     */
    static FOnCurrentLayerChanged& OnCurrentLayerChanged();
#endif

    /**
     * @brief Returns the HierarchyChanged delegate
     */
    static FOnHierarchyChanged& OnHierarchyChanged();

public:
    UOdysseyLayerStack();

public:
    //Layers management
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    bool SupportsLayerClass(UClass* Class) const;

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

    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack", meta=(DeterminesOutputType="LayerType"))
    TArray<UOdysseyLayer*> AddLayers(TSubclassOf<UOdysseyLayer> LayerType, UOdysseyLayer* ParentLayer = nullptr, int IndexInParent = 0, int Count = 1);

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

public:
    virtual int GetWidth() const { return -1; };
    virtual int GetHeight() const { return -1; };
    //Called by layers when there Parent or Children changed
    void HierarchyChanged();

public:
    // UObject overrides
    /**
     * @brief Allows us to know which property changed on an undo/redo
     * PostEditChangeProperty does not inform us of that on undo/redo
     *
     * @param TransactionEvent
     */
#if WITH_EDITOR
    virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;
#endif

    virtual void PostInitProperties() override;
    virtual void PostLoad() override;

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    virtual FInt32Range GetFrameRange() const override;

#if WITH_EDITOR
    void SetTimelineSplitterPosition(float iValue);
    float GetTimelineSplitterPosition() const;
    TSharedRef<FOdysseyLayerCellSelection> GetCellSelection() const;
#endif
    UOdysseyLayer* GetLayerRoot() const;
    TSubclassOf<UOdysseyLayer> GetLayerRootClass() const;

public:
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual TSharedPtr<FOdysseyTextureRenderer> BuildTextureRenderer(FFrameNumber iFrame, TMap<const IOdysseyTextureRenderingAbility*, FGuid>* iIds = nullptr) const override;
    //virtual void RenderToTexture_RenderThread(FRDGBuilder& iGraphBuilder, FRDGTextureRef iDestinationTexture, ERHIFeatureLevel::Type iFeatureLevel, FFrameNumber iFrame, const FMatrix& iSrcTransform, const FIntRect& iSrcRect, const FIntRect& iDstRect) const override;

protected:
    //Internal
    UOdysseyLayer* CreateLayer(UClass* iLayerType);
    UOdysseyLayer* CopyLayerInternal(UOdysseyLayer* iLayer, UOdysseyLayer* iParent, int iIndexInParent);
    void GetLayersUniqueParents(TArray<UOdysseyLayer*> iLayers, TArray<UOdysseyLayer*>& oParents);

public:
#if WITH_EDITOR
    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    void SetCurrentLayer(UOdysseyLayer* Layer);

    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    UOdysseyLayer* GetCurrentLayer() const;
#endif

    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    TArray<TSubclassOf<UOdysseyLayer>> GetSupportedLayerClasses() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|LayerStack")
    bool IsSRGB() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|LayerStack")
    void SetIsSRGB(bool Value);

protected:
    //Default properties
    UPROPERTY(Transient)
    TArray<TSubclassOf<UOdysseyLayer>> SupportedLayerClasses; //Contains compatible C++ layer types

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    TObjectPtr<UOdysseyLayer> CurrentLayer;
#endif

    UPROPERTY()
    TObjectPtr<UOdysseyLayer> LayerRoot;

    UPROPERTY(Transient)
    TObjectPtr<UClass> LayerRootClass;

    UPROPERTY(Transient)
    bool bIsSRGB = true;

#if WITH_EDITORONLY_DATA
    UPROPERTY(config)
    float TimelineSplitterPosition = 0.2f; //TODO: Move To Editor Only class
#endif

private:
#if WITH_EDITOR
    TSharedRef<FOdysseyLayerCellSelection> mCellSelection;
#endif
};
