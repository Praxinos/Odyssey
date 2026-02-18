// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "PrimitiveSceneProxy.h"
#include "StaticMeshResources.h"
#include "LocalVertexFactory.h"
#include "RenderResource.h"
#include "RawIndexBuffer.h"
// Ariane Headers
#include "ArianeObject.h"

class FArianeSegment;
class FArianeVertex;
class UMaterial;
class UArianePainting3DComponent;
class FArianePath;

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
        void BuildSegmentAsTube( FArianeSegment* Segment
                               , FVector& InOutPreviousPerpendicularVector );
        void BuildSegmentAsFlat( FArianeSegment* Segment
                               , FVector& InOutPreviousPerpendicularVector );

        void InitVertexFactory();
        FVector GetTangentVectorAt( FArianeSegment* Segment
                                  , FVector* OptionalPerpendicularVector
                                  , double T
                                  , bool bNormalize );

    protected:
        UArianePainting3DComponent* Painting3DComponent; // to retrieve the up vector
        FArianePath* Path;

        FStaticMeshVertexBuffers VertexBuffers;
        FRawStaticIndexBuffer IndexBuffer;
        FLocalVertexFactory VertexFactory;
};

struct ARIANE_API FArianePathInvalidationFlags : FArianeObjectInvalidationFlags
{
    protected:
        virtual uint32 GetSize() const override { return sizeof( FArianePathInvalidationFlags ); };

    public:
        FArianePathInvalidationFlags& SetVertexGeometry()  { VertexGeometry  = 1; return *this; };
        FArianePathInvalidationFlags& SetSegmentGeometry() { SegmentGeometry = 1; return *this; };
        FArianePathInvalidationFlags& SetVertexTopology()  { VertexTopology  = 1; return *this; };
        FArianePathInvalidationFlags& SetSegmentTopology() { SegmentTopology = 1; return *this; };

    public:
        int VertexGeometry  : 1  = 0;
        int SegmentGeometry : 1  = 0;
        int VertexTopology  : 1  = 0;
        int SegmentTopology : 1  = 0;
};

class ARIANE_API FArianePath : public FArianeObject
{
    public:
        ~FArianePath();
        FArianePath( UArianePainting3DComponent* InPainting3DComponent );

    public:
        void AddVertex( FArianeVertex* iVertex );
        void AddSegment( FArianeSegment* iSegment );

        void RemoveVertex( FArianeVertex* iVertex );
        void RemoveSegment( FArianeSegment* iSegment );

        const TArray<FArianeSegment*>& GetSegments();
        const TArray<FArianeVertex*>& GetVertices();
        UMaterial* GetMaterial();
        bool Update( bool Recurse ) override;
        FArianePathGeometry3D* GetGeometry3D();
        virtual void UpdateBounds() override;

    protected:
        FArianePathGeometry3D* Geometry3D;
        TArray<FArianeSegment*> Segments;
        TArray<FArianeVertex*> Vertices;
        UMaterial* Material;

};
