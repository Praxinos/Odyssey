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

UCLASS()
class ARIANE_API UArianeLayerDrawing : public UArianeLayer
{
    GENERATED_BODY()

public:
    ~UArianeLayerDrawing();
    UArianeLayerDrawing();

    FArianeObject* GetRootObject();

    FArianePath* AllocPath();
    FArianeObject* AllocObject();
    TArray<FInstancedStruct>& GetInstancedObjects();
    virtual void Update() override;
    FArianeObject* GetObject( const FGuid& InGuid );
    void DeleteInstancedObject( FArianeObject* Object );

    // debug
    void PrintPointers();
    void PostLoad();
    void PostEditUndo();
    void ResetHierarchy();

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootInvalidated();
    void UpdateBounds();

public:
    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedObjects;

public:
    mutable FCriticalSection InstancedObjectsAccessRW;

protected:
    UPROPERTY( EditAnywhere )
    FArianeObjectID RootObjectID;
};
