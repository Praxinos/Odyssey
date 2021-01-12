// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "IMeshPainter.h"
#include "OdysseyViewportDrawingEditorGUI.h"
#include "OdysseyViewportDrawingEditorController.h"
#include "IStylusState.h"
#include "OdysseyStrokePoint.h"
#include "MeshPaintTypes.h"
#include "Engine/StaticMesh.h"
#include "OdysseyMeshPaintRendering.h"
#include "OdysseyViewportDrawingEditorViewportClient.h"

#include "IOdysseyStylusInputModule.h"

class UOdysseyViewportDrawingEditorSettings;
class IMeshPaintGeometryAdapter;
struct FAssetData;
class SWidget;

struct FPaintableTexture;
struct FPaintTexture2DData;
struct FTexturePaintMeshSectionInfo;


/** Struct representing the selected settings for a mesh
 *	It allow us to remember which settings were selected when we come back to a previously selected actor
 */
struct FInstanceTexturePaintSettings
{

	FInstanceTexturePaintSettings()
		: mSelectedTexture(nullptr)
	{}

	FInstanceTexturePaintSettings(UTexture2D* iSelectedTexture)
		: mSelectedTexture(iSelectedTexture)
	{}

	void operator=(const FInstanceTexturePaintSettings& iSrcSettings)
	{
		mSelectedTexture = iSrcSettings.mSelectedTexture;
	}

	UTexture2D* mSelectedTexture;
};


/** Mesh Based Painting
 * Batched element parameters for texture paint shaders used for paint blending and paint mask generation
 */
class FOdysseyMeshPaintBatchedElementParameters: public FBatchedElementParameters
{
public:
    /** Binds vertex and pixel shaders for this element */
    virtual void BindShaders(FRHICommandList& RHICmdList,FGraphicsPipelineStateInitializer& GraphicsPSOInit,ERHIFeatureLevel::Type InFeatureLevel,const FMatrix& InTransform,const float InGamma,const FMatrix& ColorWeights,const FTexture* Texture) override
    {
        OdysseyMeshPaintRendering::SetMeshPaintShaders(RHICmdList,GraphicsPSOInit,InFeatureLevel,InTransform,InGamma,ShaderParams);
    }

public:

    /** Shader parameters */
    OdysseyMeshPaintRendering::FOdysseyMeshPaintShaderParameters ShaderParams;
};

/** Painter class used by the level viewport mesh painting mode */
class FOdysseyViewportDrawingEditorPainter : public IMeshPainter
                                           , public IStylusMessageHandler
{
	//TODO: why ?
	friend class SOdysseyViewportDrawingEditorGUI;

protected:
	/** destructor */
	~FOdysseyViewportDrawingEditorPainter();

	/** constructor */
	FOdysseyViewportDrawingEditorPainter();

public:
	/** Initialization method (called by Get() static method) */
	void Initialize();

	/** Finalization Method called on destruction */
	void Finalize();

public:
	/** The singleton Getter */
	static FOdysseyViewportDrawingEditorPainter* Get();

public:
	/** Gets the UICommandList */
	TSharedPtr<FUICommandList> GetUICommandList();

	/** Gets the Editor Controller associated with this painter */ //TODO: Is this mandatory ?
	TSharedPtr<FOdysseyViewportDrawingEditorController> GetController() const;

	/** Returns the maximum LOD index for the mesh we paint */
	int32 GetMaxLODIndexToPaint() const;

	/** Returns the maximum UV index for the mesh we paint */
	int32 GetMaxUVIndexToPaint() const;

	/** Returns the number of texture that require a commit. */
	int32 GetNumberOfPendingPaintChanges() const;

	void OnStylusInputChanged(TSharedPtr<IStylusInputInterfaceInternal> iStylusInput);

    bool IsCapturedByStylus() { return mIsCapturedByStylus; }
	
public:
	/** Begin IMeshPainter overrides */
	virtual void Render(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI) override;
	virtual bool Paint(FViewport* iViewport, const FVector& iCameraOrigin, const FVector& iRayOrigin, const FVector& iRayDirection) override;
	virtual bool Paint(FViewport* iViewport, const FVector& iCameraOrigin, const TArrayView<TPair<FVector, FVector>>& iRays) override;
	virtual void Refresh() override;
	virtual void AddReferencedObjects(FReferenceCollector& iCollector) override;
	virtual void FinishPainting() override;	
	virtual void Reset() override;
	virtual TSharedPtr<IMeshPaintGeometryAdapter> GetMeshAdapterForComponent(const UMeshComponent* iComponent) override;
	virtual void Tick(FEditorViewportClient* iViewportClient, float iDeltaTime) override;
    virtual bool InputKey(FEditorViewportClient* InViewportClient,FViewport* InViewport,FKey InKey,EInputEvent InEvent) override;
	virtual const FHitResult GetHitResult(const FVector& iOrigin, const FVector& iDirection) override;
	virtual void ActorSelected(AActor* iActor) override;
	virtual void ActorDeselected(AActor* iActor) override;
	virtual UPaintBrushSettings* GetBrushSettings() override;
	virtual UMeshPaintSettings* GetPainterSettings() override;
	virtual TSharedPtr<SWidget> GetWidget() override;
	/** End IMeshPainter overrides */

protected:
	/** Begin IMeshPainter protected overrides */

	/** Override from IMeshPainter used for applying actual painting */
	virtual bool PaintInternal(const FVector& iCameraOrigin, const TArrayView<TPair<FVector, FVector>>& iRays, EMeshPaintAction iPaintAction, float iPaintStrength) override;

	/** End IMeshPainter overrides */

private:
	/** Registers the paint commands (UICommandList) */
	void RegisterTexturePaintCommands();

	/** Unregisters the paint commands (UICommandList) */
	void UnregisterTexturePaintCommands();

	/** Per triangle action function used for retrieving triangle eligible for texture painting */
	void GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex);

	/** Retrieves per-instnace texture paint settings instance for the given component */
	FInstanceTexturePaintSettings& AddOrRetrieveInstanceTexturePaintSettings(UMeshComponent* iComponent);

	/** Functions for retrieving and resetting cached paint data */
	void CacheSelectionData(); //TODO: Check how these 3 functions work together and if they're needed or needs to be replaced by something better
	void CacheTexturePaintData();
	void ResetPaintingState();

	/** Checks whether or not the current selection contains components which reference the same (static/skeletal)-mesh */
	bool ContainsDuplicateMeshes(TArray<UMeshComponent*>& iComponents) const;

	/** Returns the instance of ComponentClass found in the current Editor selection */
	template<typename ComponentClass>
	TArray<ComponentClass*> GetSelectedComponents() const;

private:
	/** Callbacks */
	/** Checks whether or not the given asset should not be shown in the list of textures to paint on */
	bool ShouldFilterTextureAsset(const FAssetData& iAssetData) const;
	
	/** Callback for when the user changes the texture to paint on */
	void PaintTextureChanged(const FAssetData& iAssetData);

protected:  
	/** Texture Based Painting Methods */
    
    void StartPaintingTextureBased(UMeshComponent* iMeshComponent,const IMeshPaintGeometryAdapter& iGeometryInfo);
    
    /** Paints on a texture */
    void PaintTextureBased(const FHitResult& iHitResult,
        TArray<FTexturePaintTriangleInfo>& iInfluencedTriangles,
        const IMeshPaintGeometryAdapter& iGeometryInfo);

    void FinishPaintingTextureBased();

protected:
	/** Mesh Based Painting Methods*/

    void StartPaintingMeshBased(UMeshComponent* iMeshComponent,const IMeshPaintGeometryAdapter& iGeometryInfo);

    /** Paints on a texture */
    void PaintMeshBased(const FHitResult& iHitResult,
        TArray<FTexturePaintTriangleInfo>& iInfluencedTriangles,
        const IMeshPaintGeometryAdapter& iGeometryInfo);

    void FinishPaintingMeshBased();

private:

	/**
	* Used to get a reference to data entry associated with the texture.  Will create a new entry if one is not found.
	*
	* @param	inTexture 		The texture we want to retrieve data for.
	* @return					Returns a reference to the paint data associated with the texture.  This reference
	*								is only valid until the next change to any key in the map.  Will return NULL only when inTexture is NULL.
	*/
	FPaintTexture2DData* GetPaintTargetData(const UTexture2D* iTexture);

	/**
	* Used to add an entry to to our paint target data.
	*
	* @param	inTexture 		The texture we want to create data for.
	* @return					Returns a reference to the newly created entry.  If an entry for the input texture already exists it will be returned instead.
	*								Will return NULL only when inTexture is NULL.   This reference is only valid until the next change to any key in the map.
	*
	*/
	FPaintTexture2DData* AddPaintTargetData(UTexture2D* iTexture);

	/**
	* Used to get the original texture that was overridden with a render target texture.
	*
	* @param	inTexture 		The render target that was used to override the original texture.
	* @return					Returns a reference to texture that was overridden with the input render target texture.  Returns NULL if we don't find anything.
	*
	*/
	UTexture2D* GetOriginalTextureFromRenderTarget(UTextureRenderTarget2D* iTexture);
	
	/**
	* Used to commit all paint changes to corresponding target textures.
	* @param	bShouldTriggerUIRefresh		Flag to trigger a UI Refresh if any textures have been modified (defaults to true)
	*/
	void CommitAllPaintedTextures();

	/** Clears all texture overrides, removing any pending texture paint changes */
	void ClearAllTextureOverrides();

	/** Sets all required texture overrides for the mesh component using the adapter */
	void SetAllTextureOverrides(const IMeshPaintGeometryAdapter& iGeometryInfo, UMeshComponent* iMeshComponent);

	/** Set a specific texture override using a mesh adapter */
	void SetSpecificTextureOverrideForMesh(const IMeshPaintGeometryAdapter& iGeometryInfo, UTexture2D* iTexture);

	/** Used to tell the texture paint system that we will need to restore the rendertargets */
	void RestoreRenderTargets();

private:
	/** Forces a specific LOD level to be rendered for the selected mesh components */
	void ApplyForcedLODIndex(int32 iForcedLODIndex);

	/** Updates the paint targets based on property changes on actors in the scene */
	void UpdatePaintTargets(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent);

	void SaveModifiedTextures();
	void Cleanup();

private:
    virtual void OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex ) override;

protected:	
    /** Widget representing the state and settings for the painter */
	TSharedPtr<SOdysseyViewportDrawingEditorGUI> mWidget;

    /** Widget representing the state and settings for the painter */
    TSharedPtr<FOdysseyViewportDrawingEditorController> mController;

	/** Painting settings */
	UOdysseyViewportDrawingEditorSettings* mPaintSettings;

	/** Basic set of brush settings */
	UPaintBrushSettings* mBrushSettings;

	/** Texture paint state */
	/** Textures eligible for painting retrieved from the current selection */
	TArray<FPaintableTexture> mPaintableTextures;

    /** Texture paint: The mesh components that we're currently painting */
	UMeshComponent* mTexturePaintingCurrentMeshComponent;

	/** The original texture that we're painting */
	UTexture2D* mPaintingTexture2D;

    /** The temporary texture in which we store the strokeBuffer pixels */
    UTexture2D* mStrokeBufferTexture2D;

    /** The strokeBuffer pixels applied to 3D */
    UTexture2D* mStrokeBufferTexture3D;

    /** Temporary render target used to draw incremental paint to */
    UTextureRenderTarget2D* mBrushRenderTargetTexture;

	/** Cached / stored instance texture paint settings for selected components */
	TMap<UMeshComponent*, FInstanceTexturePaintSettings> mComponentToTexturePaintSettingsMap;

	/** Stores data associated with our paint target textures */
	TMap< UTexture2D*, FPaintTexture2DData > mPaintTargetData;

	/** Texture paint: Will hold a list of texture items that we can paint on */
	TArray<FTextureTargetListInfo> mTexturePaintTargetList;

	/** True if we need to generate a texture seam mask used for texture dilation */
	bool mDoGenerateSeamMask;
	
	/** Used to store a flag that will tell the tick function to restore data to our rendertargets after they have been invalidated by a viewport resize. */
	bool mDoRestoreRenTargets;

	/** A map of the currently selected actors against info required for painting (selected material index etc)*/
	TMap<TWeakObjectPtr<AActor>, FMeshSelectedMaterialInfo> mCurrentlySelectedActorsMaterialInfo;

	/** The currently selected actor, used to refer into the Map of Selected actor info */
	TWeakObjectPtr<AActor> mActorBeingEdited;
	// End texture paint state

	// Painter state
	/** Flag for updating cached data */
	bool mDoRefreshCachedData;
	/** Map of geometry adapters for each selected mesh component */
	TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>> mComponentToAdapterMap;
	// End painter state

	/** Mesh components within the current selection which are eligible for painting */
	TArray<UMeshComponent*> mPaintableComponents;

	/** UI command list object */
	TSharedPtr<FUICommandList> mUICommandList;

    FViewport* mFocusedViewport;
    //PATCH: Temporary viewportClient created for overriding highPrecision mouse events (useful for mac)
    FOdysseyViewportDrawingEditorViewportClient* mTemporaryViewportClient;

    FOdysseyStrokePoint mLastEvent;
	FOdysseyStrokePoint mPreviousEvent;

    bool mIsGoingToDraw;
	bool mIsCapturedByStylus;
	std::chrono::steady_clock::time_point   mStylusLastEventTime;

    FVector2D mBeginPosition;

    TArray<FKey>                            mKeysPressed;
};
