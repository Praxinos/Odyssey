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
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianePath.h" // for EArianePathLineType

#include "ArianePainting3DComponent.generated.h"

class FArianeGeometryProxy;
struct FArianeObject;
struct FArianePath;
struct FArianeVertex;
class FArianePathGeometry3D;
struct FArianeSegment;
class UArianeLayerFolder;

class ARIANE_API FArianeGeometryProxy : public FPrimitiveSceneProxy
{
    public:
        ~FArianeGeometryProxy();
        FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel, UArianePainting3DComponent* InPainting3DComponent );

        virtual SIZE_T GetTypeHash() const override;
        virtual uint32 GetMemoryFootprint( void ) const override;

        virtual FPrimitiveViewRelevance GetViewRelevance( const FSceneView* View ) const override;
        virtual void GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                           , const FSceneViewFamily& ViewFamily
                                           , uint32 VisibilityMap
                                           , FMeshElementCollector& Collector) const override;
        void InitVertexFactory();
        virtual void DrawStaticElements( FStaticPrimitiveDrawInterface * PDI ) override;

    protected:
        UArianePainting3DComponent* Painting3DComponent;
};

UCLASS()
class ARIANE_API UArianePainting3DComponent : public UMeshComponent
{
    GENERATED_BODY()

    public:
        ~UArianePainting3DComponent();
        UArianePainting3DComponent();

    protected:
        virtual void BeginPlay() override;

    public:
        virtual void TickComponent( float DeltaTime
                                  , ELevelTick TickType
                                  , FActorComponentTickFunction* ThisTickFunction ) override;
        virtual void PostLoad() override;
        virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

        #if WITH_EDITOR
        virtual void PostEditChangeProperty( FPropertyChangedEvent& event ) override;

        virtual void PostEditUndo() override;
        #endif

        // debug
        void PrintPointers();

        void ResetHierarchy();

        FArianeObject* GetRootObject();
        void DeleteInstancedObject( FArianeObject* Object );

    private:
        virtual FBoxSphereBounds CalcBounds( const FTransform& LocalToWorld ) const override;

    public:
        FArianePath* AllocPath( EArianePathLineType InLineType );
        FArianeObject* AllocObject();
        TArray<FInstancedStruct>& GetInstancedObjects();
        void Update();
        FArianeObject* GetObject( const FGuid& InGuid );

    public:
        UPROPERTY( EditAnywhere )
        TArray<FInstancedStruct> InstancedObjects;

        UPROPERTY( EditAnywhere )
        FArianeObjectID RootObjectID;

        TArray<UMaterialInterface*> UsedMaterials;

        UPROPERTY()
        UArianeLayerFolder* RootFolder;

    public:
        mutable FCriticalSection InstancedObjectsAccessRW;
};
