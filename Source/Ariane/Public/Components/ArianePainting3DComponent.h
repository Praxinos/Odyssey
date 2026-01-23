// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "PrimitiveSceneProxy.h"

#include "ArianePainting3DComponent.generated.h"

class FArianeGeometryProxy;
struct FStaticMeshVertexBuffers;

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
        virtual void PostInitProperties() override;

        virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

    private:
        virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

    protected:
        FStaticMeshVertexBuffers mMeshVertexBuffers;
        ULineBatchComponent* mLineBatchComponent;

        // testing
    public:
        TArray<FVector> mVertices;
};

class ARIANE_API FArianeGeometryProxy : public FPrimitiveSceneProxy
{
    public:
        ~FArianeGeometryProxy();
        FArianeGeometryProxy( UArianePainting3DComponent* iPainting3DComponent );

        virtual SIZE_T GetTypeHash() const override;
        virtual uint32 GetMemoryFootprint( void ) const override;

        virtual FPrimitiveViewRelevance GetViewRelevance( const FSceneView* View ) const override;
        virtual void GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                           , const FSceneViewFamily& ViewFamily
                                           , uint32 VisibilityMap
                                           , FMeshElementCollector& Collector) const override;
    protected:

};
