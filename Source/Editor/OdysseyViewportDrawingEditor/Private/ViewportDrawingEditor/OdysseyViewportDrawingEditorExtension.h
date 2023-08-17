// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "PainterEditor/OdysseyPainterEditorExtension.h"
#include "MeshPaintTypes.h"

class FOdysseyPainterEditor;
class FOdysseyViewportDrawingEditorGUI;
class IMeshPaintGeometryAdapter;

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
{
public:
    DECLARE_MULTICAST_DELEGATE(FOdysseyPaintingTargetToPaintWillChange);
    DECLARE_MULTICAST_DELEGATE(FOdysseyPaintingTargetToPaintChanged);
    DECLARE_MULTICAST_DELEGATE(FOdysseyPaintingAdapterChanged);

public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorExtension();
    FOdysseyViewportDrawingEditorExtension();
    FOdysseyViewportDrawingEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Initialize() override;
    virtual void Finalize() override;

public:
    //Getters
    AActor* Actor() const;
    UMeshComponent* Component() const;
    UMaterialInterface* Material() const;
    UTexture2D* Texture() const;

    // Delegates
    FOdysseyPaintingTargetToPaintWillChange& TargetToPaintWillChangeDelegate();
    FOdysseyPaintingTargetToPaintChanged& TargetToPaintChangedDelegate();
    FOdysseyPaintingAdapterChanged& AdapterChangedDelegate();

    const TArray<UMeshComponent*>& SelectableComponents() const;
    void  SelectableMaterials( TArray<UMaterialInterface*>& ioSelectedMaterials ) const;
    const TArray<FPaintableTexture>& SelectableTextures() const;

    const TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>& ComponentToAdapterMap() const;

    EOdysseyViewportDrawingPaintingAdapterMethod PaintingAdapterMethod() const;
    FOdysseyViewportDrawingEditorGUI* GetGUI();
    
    int32 GetUVIndexUsedByCurrentTexture();
    float  GetMeshComponentMaxSize() const;

public:
    //Setters
    void SetActor(AActor* iActor);
    void SetComponent(UMeshComponent* iComponent);
    void SetMaterial(UMaterialInterface* iMaterial);
    void SetTexture(UTexture2D* iTexture);
    void SetPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod iNewMethod);

private:
    // Listeners
    void OnObjectPropertyChanged(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent);

private:
    // Private Methods
    void ClearSelectableComponents();
    void UpdateSelectableComponents();
    void SelectDefaultComponent();

    void ClearSelectableTextures();
    void UpdateSelectableTextures();
    void SelectDefaultMaterial();
    void SelectDefaultTexture();

private:
    TSharedPtr<FOdysseyViewportDrawingEditorGUI> mGUI;
    FOdysseyPaintingTargetToPaintChanged mTargetToPaintChangedDelegate;
    FOdysseyPaintingTargetToPaintWillChange mTargetToPaintWillChangeDelegate;
    FOdysseyPaintingAdapterChanged mAdapterChangedDelegate;

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

    /** This one allows us to remember the selected settings for a given component (like knowing which texture of the component was selected) */
	TMap<UMeshComponent*, FInstanceTexturePaintSettings> mComponentToTexturePaintSettingsMap;
    EOdysseyViewportDrawingPaintingAdapterMethod mPaintingAdapterMethod;

    AActor* mActor;
    UMeshComponent* mComponent;
    UMaterialInterface* mMaterial; //Storage purposes only for the GUI -> Get path for it
    TArray<UMeshComponent*> mSelectableComponents;
    TArray<FPaintableTexture> mSelectableTextures;

	/** Map of geometry adapters for each selectable mesh component, so that we don't recreate a GeometryAdapter each time we select a mesh to paint */
	TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>> mComponentToAdapterMap;
};
