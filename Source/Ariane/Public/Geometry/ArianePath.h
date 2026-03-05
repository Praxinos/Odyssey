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
#include "StructUtils/InstancedStruct.h"

#include "DynamicMeshBuilder.h"

// Ariane Headers
#include "ArianeID.h"
#include "ArianeObject.h"

#include "ArianePath.generated.h"

struct FArianeSegment;
struct FArianeVertex;
class UMaterial;
class UArianePainting3DComponent;
struct FArianePath;

class ARIANE_API FArianePathGeometry3D
{
    public:
        ~FArianePathGeometry3D();
        FArianePathGeometry3D( FArianePath* InPath );

        void Build();

        const FStaticMeshVertexBuffers& GetVertexBuffers() const;
        const FRawStaticIndexBuffer& GetIndexBuffer() const;
        FArianePath* GetPath();
        FLocalVertexFactory* GetVertexFactory();

    protected:
        void BuildSegmentAsTube( FArianeSegment* Segment
                               , FVector& InOutPreviousPerpendicularVector );
        void BuildSegmentAsFlat( FArianeSegment* Segment
                               , FVector& InOutPreviousPerpendicularVector );

        void InitVertexFactory( TArray<FModelVertex>& ModelVertices
                              , TArray<uint32>& Indices );
        FVector GetTangentVectorAt( FArianeSegment* Segment
                                  , FVector* OptionalPerpendicularVector
                                  , double T
                                  , bool bNormalize );

    protected:
        FArianePath* Path;

        FStaticMeshVertexBuffers VertexBuffers;
        FRawStaticIndexBuffer IndexBuffer;
        FLocalVertexFactory* VertexFactory;
};

struct ARIANE_API FArianePathInvalidationFlags : FArianeObjectInvalidationFlags
{
    private:
        typedef FArianeObjectInvalidationFlags Super;

    public:
        static const uint32 StaticClass() { return 0xb2a965bc; }; // value is crc32 FArianePathInvalidationFlags
        virtual uint32 GetClass() { return StaticClass(); };
        virtual bool HasBaseClass( uint32 BaseClass ) const override;

    public:
        virtual FArianePathInvalidationFlags& AND( const FArianeObjectInvalidationFlags& RHS ) override;
        virtual FArianePathInvalidationFlags& OR( const FArianeObjectInvalidationFlags& RHS ) override;
        virtual FArianePathInvalidationFlags& SetAll() override;
        virtual FArianePathInvalidationFlags& Clear() override;
        virtual bool HasAny() override;

    public:
        void ClearOwn( FArianePathInvalidationFlags& Flags );

    public:
        FArianePathInvalidationFlags& SetVertexGeometry()  { VertexGeometry  = 1; return *this; };
        FArianePathInvalidationFlags& SetSegmentGeometry() { SegmentGeometry = 1; return *this; };
        FArianePathInvalidationFlags& SetVertexTopology()  { VertexTopology  = 1; return *this; };
        FArianePathInvalidationFlags& SetSegmentTopology() { SegmentTopology = 1; return *this; };

    public:
        uint32 VertexGeometry  : 1  = 0;
        uint32 SegmentGeometry : 1  = 0;
        uint32 VertexTopology  : 1  = 0;
        uint32 SegmentTopology : 1  = 0;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianePath : public FArianeObject
{
    GENERATED_BODY()

    private:
        static const uint32 mStaticClass = 0xf13c7476; // value is crc32 FArianePath

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        //virtual bool HasBaseClass( uint32 iBaseClassID );

    public:
        virtual ~FArianePath();
        FArianePath();
        FArianePath( UArianePainting3DComponent* InPainting3DComponent );

    public:
        FArianeVertex* AllocVertex( const FVector& iPosition, const FVector& InNormal, double InRadius );
        FArianeSegment* AllocSegment( FArianeVertex* iVertex0, FArianeVertex* iVertex1 );

        void AddVertex( FArianeVertex* Vertex );
        void AddSegment( FArianeSegment* Segment );

        void RemoveVertex( FArianeVertex* iVertex, bool bRemoveFromInstancedVertices = true );
        void RemoveSegment( FArianeSegment* iSegment, bool bRemoveFromInstancedSegments = true );

        TArray<FArianeSegmentID>& GetSegments();
        TArray<FArianeSegment*>& GetInvalidatedSegments();
        TArray<FArianeVertexID>& GetVertices();

        bool Update( bool Recurse ) override;
        FArianePathGeometry3D& GetGeometry3D();
        virtual void UpdateBounds() override;
        FArianeVertex* GetVertexByGuid( const FGuid& InGuid );
        FArianeSegment* GetSegmentByGuid( const FGuid& InGuid );
        //virtual void PostLoad();

        virtual void PostEditUndo();
        void InvalidateSegment( FArianeSegment* Segment );

    protected:
        static void InvalidatePointerCache( TArray<FArianeVertexID>& VertexIDArray );
        static void InvalidatePointerCache( TArray<FArianeSegmentID>& SegmentIDArray );

    public:
        UPROPERTY( EditAnywhere )
        TArray<FArianeVertexID> Vertices;

        UPROPERTY( EditAnywhere )
        TArray<FArianeSegmentID> Segments;

        UPROPERTY( EditAnywhere )
        TArray<FInstancedStruct> InstancedVertices;

        UPROPERTY( EditAnywhere )
        TArray<FInstancedStruct> InstancedSegments;

    protected:
        FArianePathGeometry3D Geometry3D;

        TArray<FArianeSegment*> InvalidatedSegments;
};
