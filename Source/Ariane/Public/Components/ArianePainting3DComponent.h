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

#include "ArianePainting3DComponent.generated.h"

class FArianeGeometryProxy;
class FArianePath;
class FArianeVertex;
class FArianePathGeometry3D;
class FArianeSegment;

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
        virtual FBoxSphereBounds CalcBounds( const FTransform& LocalToWorld ) const override;

    public:
        void AddPath( FArianePath* iPath );
        const TArray<FArianePathGeometry3D*>& GetPathMeshs();
        void BuildPathMeshs();

    protected:
        //ULineBatchComponent* LineBatchComponent;
        TArray<FArianePathGeometry3D*> PathMeshs;
};

class ARIANE_API FArianePathGeometry3D
{
    public:
        ~FArianePathGeometry3D();
        FArianePathGeometry3D( UArianePainting3DComponent* InPainting3DComponent, FArianePath* InPath );

        void Build();

        const FStaticMeshVertexBuffers& GetVertexBuffers() const;
        const FRawStaticIndexBuffer& GetIndexBuffer() const;
        FArianePath* GetPath();
        FLocalVertexFactory& GetVertexFactory();

    protected:
        void BuildSegment( FArianeSegment* Segment
                         , FVector& InOutPreviousPerpendicularVector );
        void InitVertexFactory();
        FVector GetPerpendicularVector( FArianeVertex* Vertex
                                      , FVector& PreviousVector
                                      , bool bNormalize );
        FVector GetTangentVectorAt( FArianeSegment* Segment
                                  , const FVector& PerpendicularVector
                                  , double T
                                  , bool bNormalize );
        FVector GetTangentVectorAt( FArianeSegment* Segment
                                  , double T
                                  , bool bNormalize );

    protected:
        UArianePainting3DComponent* Painting3DComponent; // to retrieve the up vector
        FArianePath* Path;

        FStaticMeshVertexBuffers VertexBuffers;
        FRawStaticIndexBuffer IndexBuffer;
        FLocalVertexFactory VertexFactory;
};

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
