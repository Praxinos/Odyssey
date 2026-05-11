// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianeLayer.h"

#include "ArianeLayerDrawing.generated.h"

struct FArianeObject;
struct FArianePath;
class UMaterialInterface;

UENUM()
enum class EArianeLayerDrawingOrigin : uint8
{
    Layer,
    Surface
};

UENUM()
enum class EArianeLayerDrawingOrientation : uint8
{
    LayerXY,
    LayerYZ,
    LayerZX,
    View,
};

UCLASS()
class ARIANE_API UArianeLayerDrawing : public UArianeLayer
{
    GENERATED_BODY()

public:
    ~UArianeLayerDrawing();
    UArianeLayerDrawing();

    FArianeObject* GetRootObject();

    FArianePath* AllocPath( UMaterialInterface* MaterialInterface );
    FArianeObject* AllocObject();
    TArray<FInstancedStruct>& GetInstancedObjects();
    virtual void Update( bool bInteractive ) override;
    FArianeObject* GetObject( const FGuid& InGuid );
    void DeleteInstancedObject( FArianeObject* Object );

    // debug
    void PrintPointers();
    void PostLoad();
    void ResetHierarchy();
    EArianeLayerDrawingOrigin GetDrawingOrigin();
    void SetDrawingOrigin( EArianeLayerDrawingOrigin InDrawingOrigin );
    EArianeLayerDrawingOrientation GetDrawingOrientation();
    void SetDrawingOrientation( EArianeLayerDrawingOrientation InDrawingOrientation );
    virtual void PostEditUndo() override;

    void IncrementMaterial( UMaterialInterface* MaterialInterface );
    void DecrementMaterial( UMaterialInterface* MaterialInterface );
    const TMap<UMaterialInterface*, uint32>& GetUsedMaterials();

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootObjectInvalidated();
    void UpdateBounds();

public:
    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedObjects;

public:
    mutable FCriticalSection InstancedObjectsAccessRW;

protected:
    UPROPERTY( EditAnywhere )
    FArianeObjectID RootObjectID;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrigin DrawingOrigin;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrientation DrawingOrientation;

protected:
    TMap<UMaterialInterface*, uint32> UsedMaterials;
};
