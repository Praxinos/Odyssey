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
class UArianeLayerDrawing;
struct FArianePath;

UENUM(BlueprintType)
enum class EArianePathLineType : uint8
{
    Tube = 0,
    Flat = 1,
};

class FArianePathVertexBuffer : public FVertexBuffer
{
public:
    uint32 VertexCount = 4; // minimum default value to have a valid buffer at Init

    void Resize( uint32 InBufferSize, FRHICommandListBase& RHICmdList );
    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
};

class ARIANE_API FArianePathGeometry3D
{
    public:
        ~FArianePathGeometry3D();
        FArianePathGeometry3D( FArianePath* InPath );

        void Build();

        const uint32 GetVertexCount() const;
        const FRawStaticIndexBuffer& GetIndexBuffer() const;
        FArianePath* GetPath();
        FLocalVertexFactory* GetVertexFactory();

    protected:
        void BuildSegmentAsTube( FArianeSegment* Segment
                               , FVector& InOutPreviousPerpendicularVector );
        void BuildSegmentAsFlat( FArianeSegment* Segment
                               , FVector& InOutPreviousPerpendicularVector );

        void InitVertexFactory( TArray<FDynamicMeshVertex>& Vertices, TArray<uint32>& Indices );
        FVector GetTangentVectorAt( FArianeSegment* Segment
                                  , FVector* OptionalPerpendicularVector
                                  , double T
                                  , bool bNormalize );

    protected:
        FArianePath* Path;

        uint32 VertexCount;
        FPositionVertexBuffer PositionBuffer;
        FStaticMeshVertexBuffer StaticMeshVB;
        FColorVertexBuffer ColorBuffer;
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
        FArianePathInvalidationFlags& SetVertexAltered()  { VertexAltered  = 1; return *this; };
        FArianePathInvalidationFlags& SetSegmentAltered() { SegmentAltered = 1; return *this; };
        FArianePathInvalidationFlags& SetVertexAddedOrRemoved()  { VertexAddedOrRemoved  = 1; return *this; };
        FArianePathInvalidationFlags& SetSegmentAddedOrRemoved() { SegmentAddedOrRemoved = 1; return *this; };

    public:
        uint32 VertexAltered  : 1  = 0;
        uint32 SegmentAltered : 1  = 0;
        uint32 VertexAddedOrRemoved  : 1  = 0;
        uint32 SegmentAddedOrRemoved : 1  = 0;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianePath : public FArianeObject
{
GENERATED_BODY()

public:
    static uint32 StaticClass() { return 0xf13c7476; }; // value is crc32 FArianePath
    virtual uint32 GetClass() override { return StaticClass(); };
    //virtual bool HasBaseClass( uint32 iBaseClassID );

public:
    struct ARIANE_API Chain
    {
        ~Chain();
        Chain( FArianePath* Path, FArianeVertex* UnchainedVertex );

        void IterateSegments( TFunction<bool( FArianeVertex*, FArianeSegment*)> Callback ) const;

    public:
        FArianeVertex* LeadingVertex;
        TArray<FArianeSegment*> Segments;
        //::ULIS::FRectD mBBox;
    };

public:
    virtual ~FArianePath();
    FArianePath();
    FArianePath( UArianeLayerDrawing* InDrawingLayer );

public:
    /**
        * @brief Alloc a vertex. Nb: the vertex is a allocated inside a FInstancedStruct,
        * hence supports the unreal reflection system.
        * @param iPosition vertex's position
        * @param InNormal vertex's normal vector (perpendicular to the plan it was drawn on)
        * @param InRadius vertex's radius
        * @return a pointer to the allocated vertex
        */
    FArianeVertex* AllocVertex( const FVector& iPosition, const FVector& InNormal, double InRadius );

    /**
        * @brief Alloc a segment. Nb: the segment is a allocated inside a FInstancedStruct,
        * hence supports the unreal reflection system.
        * @param Vertex0 first vertex
        * @param Vertex1 second vertex
        * @return a pointer to the allocated segment
        */
    FArianeSegment* AllocSegment( FArianeVertex* Vertex0, FArianeVertex* Vertex1 );

    /**
     * @brief Add a vertex to this path.
     * @param Vertex the vertex.
     */
    void AddVertex( FArianeVertex* Vertex );

    /**
     * @brief Add a segment to this path.
     * @param Segment the segment.
     */
    void AddSegment( FArianeSegment* Segment );

    /**
     * @brief Remove a vertex from this path.
     * @param Vertex the vertex.
     * @param bRemoveFromInstancedVertices true to remove from allocated vertices, false otherwise.
     */
    void RemoveVertex( FArianeVertex* iVertex, bool bRemoveFromInstancedVertices = true );

    /**
     * @brief Remove a segment from this path.
     * @param Segment the segment.
     * @param bRemoveFromInstancedVertices true to remove from allocated segments, false otherwise.
     */
    void RemoveSegment( FArianeSegment* Segment, bool bRemoveFromInstancedSegments = true );

    /** Get all the vertices added to this path */
    TArray<FArianeVertexID>& GetVertices();

    /** Get all the invalidated vertices */
    TArray<FArianeSegment*>& GetInvalidatedVertices();

    /** Get all the segments added to this path */
    TArray<FArianeSegmentID>& GetSegments();

    /** Get all the invalidated segments */
    TArray<FArianeSegment*>& GetInvalidatedSegments();

    /** Get the 3D geometry buffers */
    FArianePathGeometry3D& GetGeometry3D();

    /**
     * @brief Get a vertex by its Guid
     * @param InGuid the Guid.
     * @return the first vertex with this Guid or nullptr if none
     */
    FArianeVertex* GetVertexByGuid( const FGuid& InGuid );

    /**
     * @brief Get a segment by its Guid
     * @param InGuid the Guid.
     * @return the first segment with this Guid or nullptr if none
     */
    FArianeSegment* GetSegmentByGuid( const FGuid& InGuid );

    /**
     * @brief Mark a vertex as invalidated
     * @param Vertex the vertex.
     */
    void InvalidateVertex( FArianeVertex* Vertex );

    /**
     * @brief Mark a segment as invalidated
     * @param Segment the segment.
     */
    void InvalidateSegment( FArianeSegment* Segment );

    /** Invalidate all segments */
    void InvalidateAllSegments();

    /** overriden from ArianeObject */
    virtual bool Update( bool Recurse, bool bClearFlags = true ) override;
    /** overriden from ArianeObject */
    virtual void UpdateBounds() override;
    /** overriden from ArianeObject */
    virtual void PostLoad() override;
    /** overriden from ArianeObject */
    virtual void PostEditUndo() override;

    void FindChains();
    const TArray<Chain>& GetChains();
    void SetLineType( EArianePathLineType LineType );
    EArianePathLineType GetLineType();
    const FColor& GetColor();
    void  SetColor( const FColor& InColor );
    virtual void ExportProperties( FArianeObject* DestObject ) override;


protected:
    UPROPERTY( EditAnywhere )
    TArray<FArianeVertexID> Vertices;

    UPROPERTY( EditAnywhere )
    TArray<FArianeSegmentID> Segments;

    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedVertices;

    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedSegments;

    UPROPERTY( EditAnywhere )
    EArianePathLineType LineType;

    UPROPERTY( EditAnywhere )
    FColor Color;

protected:
    FArianePathGeometry3D Geometry3D;

    TArray<FArianeSegment*> InvalidatedSegments;
    TArray<FArianeVertex*> InvalidatedVertices;
    TArray<Chain> Chains;
};
