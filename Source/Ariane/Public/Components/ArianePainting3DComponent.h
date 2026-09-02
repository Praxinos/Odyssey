// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "PrimitiveSceneProxy.h"
#include "StaticMeshResources.h"
#include "LocalVertexFactory.h"
#include "RenderResource.h"
#include "RawIndexBuffer.h"
// Ariane Headers
#include "ArianePath.h" // for EArianePathLineType
#include "ArianePainting3DComponentEditorInterface.h"
// Odyssey Headers
#include "OdysseyPalette.h"

#include "ArianePainting3DComponent.generated.h"

class FArianeGeometryProxy;
struct FArianeObject;
struct FArianePath;
struct FArianeVertex;
class FArianePathGeometry3D;
struct FArianeSegment;
class UArianeLayerFolder;
class UArianeLayer;
class UArianeLayerDrawing;
class UArianeLayerStack;
class UOdysseyPalette;
class UOdysseyPaletteSet;
class UOdysseyPaletteEntryColor;
class UArianePainting3DStaticMeshComponent;
class UMaterialInstanceDynamic;



UCLASS()
class ARIANE_API UArianePainting3DComponent : public USceneComponent
{
    GENERATED_BODY()

    DECLARE_MULTICAST_DELEGATE_OneParam( FOnUpdateDelegate, bool );

public:
    ~UArianePainting3DComponent();
    UArianePainting3DComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void PostLoad() override;


    #if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& event ) override;

    virtual void PostEditUndo() override;
    #endif

    void ResetHierarchy();
    virtual void OnComponentDestroyed( bool bDestroyingHierarchy ) override;
    void DeleteInstancedObject( FArianeObject* Object );

    UArianeLayerStack* GetLayerStack();
    const TArray<UOdysseyPaletteSet*>& GetPaletteSets() const;
    UOdysseyPaletteEntryColor* GetCurrentPaletteColorEntry() const;
    void SetCurrentPaletteSet( const FGuid& InCurrentPaletteSet );
    FGuid GetCurrentPaletteSet() const;
    void AddPaletteSet( UOdysseyPalette* iPalette );
    void RemovePaletteSet( UOdysseyPaletteSet* PaletteSet );
    void SetPaletteSet( FGuid Index, UOdysseyPaletteSet* PaletteSet );
    void SetCurrentPaletteColorEntry( UOdysseyPaletteEntryColor* Entry, FGuid Set );
    virtual void OnRegister() override;
    //const TArray<UMaterialInterface*>& GetUsedMaterials();
    // overrides UMeshComponent::GetNumMaterials()
    //virtual int32 GetNumMaterials() const override;
    // overrides UMeshComponent::GetMaterial()
    //virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
    // overrides UMeshComponent::SetMaterial()
    //virtual void SetMaterial( int32 ElementIndex, UMaterialInterface* Material ) override;
    // overrides UMeshComponent::GetUsedMaterials()
    //virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials = false) const override;

    void Init();
    FOnUpdateDelegate& OnPreUpdateDelegate();
    FOnUpdateDelegate& OnPostUpdateDelegate();
    void Update( bool bInteractive );
    FColor GetHUDForegroundColor();
    void SetEditorInterface( IArianePainting3DComponentEditorInterface* InEditorInterface );
    void ConvertToStaticMesh();
    UArianePainting3DStaticMeshComponent* GetStaticMeshComponent();
    UMaterialInstanceDynamic* GetDefaultMaterial();

private:
    virtual FBoxSphereBounds CalcBounds( const FTransform& LocalToWorld ) const override;

protected:
    UPROPERTY()
    UArianeLayerStack* LayerStack;

    UPROPERTY()
    TArray<UOdysseyPaletteSet*> PaletteSets;

    UPROPERTY()
    UMaterialInstanceDynamic* DefaultMaterial;

    UPROPERTY()
    int FileVersion = 1;


protected:
    UOdysseyPaletteEntryColor* CurrentPaletteColorEntry;
    FGuid CurrentPaletteSet;
    //TArray<UMaterialInterface*> UsedMaterials;
    FOnUpdateDelegate OnPreUpdate;
    FOnUpdateDelegate OnPostUpdate;
    IArianePainting3DComponentEditorInterface* EditorInterface;
    UArianePainting3DStaticMeshComponent* StaticMeshComponent;
};
