// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "PainterEditor/OdysseyPainterEditorExtension.h"
#include "MeshPaintTypes.h"
#include "ISequencer.h"
#include "OdysseyHUDSystem.h"
#include "TickableEditorObject.h"
#include "Engine/Texture2D.h"

class FOdysseyPainterEditor;
class FOdysseyViewportDrawingEditorGUI;
class IMeshPaintGeometryAdapter;
class FOdysseyPainterEditorSource;
class IOdysseyViewportDrawingEditorAdapter;
class UMeshComponent;
class FEditorViewportClient;

UENUM()
enum EOdysseyViewportDrawingPaintingAdapterMethod
{
    OdysseyTextureBased           UMETA(DisplayName = "Texture Based"),
    OdysseyMeshBasedPlanar        UMETA(DisplayName = "Mesh Based (Planar)"),
    OdysseyMeshBasedSphere        UMETA(DisplayName = "Mesh Based (Sphere)"),
    OdysseyScreenBased            UMETA(DisplayName = "Screen Based"),
};

class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorExtension
    : public FOdysseyPainterEditorExtension
    , public FTickableEditorObject //Allows us to react to Tick events
{
public:
    DECLARE_MULTICAST_DELEGATE(FOdysseyPaintingAdapterChanged);

public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorExtension();
    FOdysseyViewportDrawingEditorExtension();
    FOdysseyViewportDrawingEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Initialize() override;
    virtual void Finalize() override;

    void InitializeRenderTarget();
    void FinalizeRenderTarget();

public:
    //Getters
    AActor* Actor() const;
    UMeshComponent* Component() const;
    UMaterialInterface* Material() const;
    UTexture* Texture() const;

    bool IsPlaneComponent() const;

    IOdysseyViewportDrawingEditorAdapter* GetOdysseyViewportDrawingEditorAdapter();

    // Delegates
    FOdysseyPaintingAdapterChanged& AdapterChangedDelegate();

    const TArray<UMeshComponent*>& SelectableComponents() const;
    void  SelectableMaterials( TArray<UMaterialInterface*>& ioSelectedMaterials ) const;
    const TArray<FPaintableTexture>& SelectableTextures() const;

    const TMap<TObjectPtr<UMeshComponent>, TSharedPtr<IMeshPaintGeometryAdapter>>& ComponentToAdapterMap() const;

    EOdysseyViewportDrawingPaintingAdapterMethod PaintingAdapterMethod() const;
    
    int32 GetUVIndexUsedByCurrentTexture();
    float  GetMeshComponentMaxSize() const;

    bool GetHUDPlaneParams(FVector& oPlaneTopLeft, double& oW, double& oH, FVector& oXAxis, FVector& oYAxis);
    bool GetDrawHUDParams(const FSceneView* View, FCanvas* Canvas, FOdysseyHUDSystem::FDrawHUDParams& oParams);
    bool ViewportToHUD(FEditorViewportClient* iViewportClient, const FVector2D& iViewportPoint, FVector2D& oHUDPoint);

public:
    //Setters
    void SetActor(AActor* iActor);
    void SetComponent(UMeshComponent* iComponent);
    void SetMaterial(UMaterialInterface* iMaterial);
    bool SetTexture(UTexture* iTexture, bool iWarnUserIfFailed);
    void SetPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod iNewMethod);

private:
    // Listeners
    void OnObjectPropertyChanged(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent);
    void OnSourceChanged();

private:
    // FTickableEditorObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( FOdysseyViewportDrawingEditorExtension, STATGROUP_Tickables); }

private:
    // Private Methods
    void SetTextureInternal(UTexture* iTexture);
    void ClearSelectableComponents();
    void UpdateSelectableComponents();
    void SelectDefaultComponent();

    void ClearSelectableTextures();
    void UpdateSelectableTextures();
    void SelectDefaultMaterial();
    void SelectDefaultTexture();

private:
    /** Sequencer related */
    void OnSequencersChanged();
    void OnSyncPaintingWithSequencer();
    void OnSyncPaintingWithSequencerMovieSceneChanged( EMovieSceneDataChangeType iChangedType );
    void DisableDelegatesSequencer();
    void EnableDelegatesSequencer();
    void SetAllDelegatesSequencers();
    void ClearAllDelegatesSequencers();
    
private:
    void OnAnimationPlayerCurrentTimeChanged();
    
    void SyncMediaPlayerWithAnimationFrame(int iFrame);
    void SyncMediaPlayerWithAnimationPlayer();
    void SyncMediaPlayerWithAnimationCurrentFrame();
    void SyncAnimationCurrentFrameWithMediaPlayer();

    bool EnsureMediaPlateIsOpened();

private:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    TSharedPtr<FOdysseyViewportDrawingEditorGUI> mGUI;
    FOdysseyPaintingAdapterChanged mAdapterChangedDelegate;

    /** Struct representing the selected settings for a mesh
     *    It allow us to remember which settings were selected when we come back to a previously selected actor
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

        UTexture* mSelectedTexture;
    };

    /** This one allows us to remember the selected settings for a given component (like knowing which texture of the component was selected) */
    TMap<UMeshComponent*, FInstanceTexturePaintSettings> mComponentToTexturePaintSettingsMap;
    EOdysseyViewportDrawingPaintingAdapterMethod mPaintingAdapterMethod;

    AActor* mActor;
    UMeshComponent* mComponent;
    UMaterialInterface* mMaterial; //Storage purposes only for the GUI -> Get path for it
    UTexture* mTexture; //Storage purposes only for the GUI -> Get path for it
    TArray<UMeshComponent*> mSelectableComponents;
    TArray<FPaintableTexture> mSelectableTextures;

    /** Map of geometry adapters for each selectable mesh component, so that we don't recreate a GeometryAdapter each time we select a mesh to paint */
    TMap<TObjectPtr<UMeshComponent>, TSharedPtr<IMeshPaintGeometryAdapter>> mComponentToAdapterMap;

    /** The corresponding render target for the Texture of the editor above */
    UTextureRenderTarget2D* mPaintingTexture2DRenderTarget;

    /** A render target to store the stroke pixels we want to stamp */
    UTextureRenderTarget2D* mStrokeBufferRenderTarget2D;

    /** A render target to store the pixels of the seams */
    UTextureRenderTarget2D* mSeamRenderTarget2D;

    TSharedPtr<FOdysseyPainterEditorSource> mCurrentSource;

    /** Temporary variable (until overrides are fixed) that keep the mip settings of the texture on which we draw*/
    TextureMipGenSettings mPreviousMipSettings;

    //Sequencers used thorough the editor. They may change the current actor selected, so we need to keep track of what they are doing
    TArray<TWeakPtr<ISequencer>> mSequencers;

    /** Painting Extension: describes the method by which we draw in the viewport */
    TSharedPtr<IOdysseyViewportDrawingEditorAdapter> mPaintingAdapter;

    /** Used to track the animation media being scrubbed */
    FTimespan mAnimationMediaTimespan;
};
