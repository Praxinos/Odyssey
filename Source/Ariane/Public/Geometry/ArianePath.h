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
struct FArianeSegmentCubic;
struct FArianeVertex;
class UMaterial;
class UArianeImage;
struct FArianePath;
class UMaterialInterface;

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

class ARIANE_API FArianePathGeometry3D : public FArianeObjectGeometry3D
{
    public:
        ~FArianePathGeometry3D();
        FArianePathGeometry3D( FArianePath* InPath );

        virtual void Build() override;
        FArianePath* GetPath();

    protected:
        void BuildSegmentAsTube( FArianeSegment* Segment
                               , double T0
                               , double T1
                               , FVector& InOutPreviousPerpendicularVector );
        void BuildSegmentAsFlat( FArianeSegment* Segment
                               , double SegmentT0
                               , double SegmentT1
                               , FVector& InOutPreviousPerpendicularVector );
        FVector GetTangentVectorAt( FArianeSegment* Segment
                                  , double T
                                  , bool bNormalize );
        FVector GetLeavingVectorAt( FArianeSegment* Segment
                                  , double T );
};

struct ARIANE_API FArianePathInvalidationFlags : FArianeObjectInvalidationFlags
{
    private:
        typedef FArianeObjectInvalidationFlags Super;

    public:
        static const uint32 StaticClass() { return 0xb2a965bc; }; // value is crc32 FArianePathInvalidationFlags
        virtual uint32 GetClass() override { return StaticClass(); };
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
    virtual bool HasBaseClass( uint32 iBaseClassID );

public:
    struct ARIANE_API Chain
    {
        ~Chain();
        Chain( FArianePath* Path, FArianeVertex* UnchainedVertex );

        void IterateSegments( TFunction<bool( FArianeVertex*, FArianeSegment*)> Callback ) const;
        double GetLength();

    public:
        FArianeVertex* LeadingVertex;
        TArray<FArianeSegment*> Segments;
        double Length;
        //::ULIS::FRectD mBBox;
    };

public:
    virtual ~FArianePath();
    FArianePath();
    FArianePath( UArianeImage* InImage
               , const FName& InName
               , EArianeAllocationModel InAllocationModel
               , FArianePathInvalidationFlags* InInvalidationFlags = nullptr );

public:
    /** overriden from ArianeObject */
    virtual void UpdateShape( EUpdateFlags UpdateFlags ) override;
    /** overriden from ArianeObject */
    virtual void UpdateBoundingBox( EUpdateFlags UpdateFlags ) override;
    /** overriden from ArianeObject */
    virtual void PostLoad() override;
    /** overriden from ArianeObject */
    virtual void PostEditUndo() override;
    virtual void ExportProperties( FArianeObject* DestObject ) override;
    virtual void Added() override;
    virtual void Removed() override;

    /**
     * @brief Alloc a vertex. Nb: the vertex is a allocated inside a FInstancedStruct,
     * hence supports the unreal reflection system.
     * @param iPosition vertex's position
     * @param InNormal vertex's normal vector (perpendicular to the plan it was drawn on)
     * @param InRadius vertex's radius
     * @return a pointer to the allocated vertex
     */
    FArianeVertex* AllocVertex( const FVector& InPosition
                              , const FVector& InNormal
                              , double InRadius
                              , EArianeAllocationModel InAllocationModel );

    /**
     * @brief Alloc a segment. Nb: the segment is a allocated inside a FInstancedStruct,
     * hence supports the unreal reflection system.
     * @param Vertex0 first vertex
     * @param Vertex1 second vertex
     * @return a pointer to the allocated segment
     */
    FArianeSegment* AllocSegment( FArianeVertex* Vertex0
                                , FArianeVertex* Vertex1
                                , EArianeAllocationModel InAllocationModel );

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
     * @param bUnallocate true to free the memory used by this vertex, false otherwise.
     */
    void RemoveVertex( FArianeVertex* iVertex, bool bUnallocate = true );

    /**
     * @brief Remove a segment from this path.
     * @param Segment the segment.
     * @param bUnallocate true to free the memory used by this segment, false otherwise.
     */
    void RemoveSegment( FArianeSegment* Segment, bool bUnallocate = true );

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

    /** Invalidate all segments */
    void InvalidateAllVertices();

    /** Get all chains (i.e all continguous sets of segments) */
    const TArray<Chain>& GetChains();

    /** Set Line Type **/
    void SetLineType( EArianePathLineType LineType );

    /** Get Line Type **/
    EArianePathLineType GetLineType();

    /** Get Path Color **/
    const FColor& GetColor();

    /** Set Path Color **/
    void  SetColor( const FColor& InColor );

    /** Set Path Material **/
    void SetMaterial( UMaterialInterface* InMaterialInterface );

    /** Get Path Material **/
    UMaterialInterface* GetMaterial();

    FArianeSegmentCubic* AllocCubicSegment( FArianeVertex* Vertex0
                                          , double Handle0X
                                          , double Handle0Y
                                          , double Handle0Z
                                          , double Handle1X
                                          , double Handle1Y
                                          , double Handle1Z
                                          , FArianeVertex* Vertex1
                                          , EArianeAllocationModel InAllocationModel );
    FArianeSegmentCubic* AllocCubicSegment( FArianeVertex* Vertex0
                                          , const FVector& Handle0
                                          , const FVector& Handle1
                                          , FArianeVertex* Vertex1
                                          , EArianeAllocationModel InAllocationModel );
    void AlterRadius( double RatioRadius );

    // returns true if the path is empty after vertex removal
    bool DeleteVertex( const TArray<FArianeVertex*>& VerticesToRemove
                     , TArray<FArianeVertex*>* OutRemovedVertices
                     , TArray<FArianeSegment*>* OutRemovedSegments
                     , TArray<FArianeSegment*>* OutAddedSegments );
    virtual void Animate( const FArianeKeyedObject* KeyedObject, const FArianeKeyedObject* NextKeyedObject, float T ) override;

    uint32 GetCubicSegmentCount();
    uint32 GetLinearSegmentCount();

protected:
    void FindChains();
    virtual FArianeObject* CopyShape( const FCopyArgs& CopyArgs ) override;
    virtual void CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate ) override;

private:
    static FArianeVertex* GetStitchingVertex( FArianeVertex* Vertex
                                            , FArianeSegment* Segment
                                            , const TArray<FArianeVertex*>& PickedVertexArray
                                            , FVector& OutHandle );

protected:


    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedVertices;

    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedSegments;

    UPROPERTY( EditAnywhere )
    EArianePathLineType LineType;

    UPROPERTY( EditAnywhere )
    FColor Color;

    UPROPERTY( EditAnywhere )
    UMaterialInterface* MaterialInterface;

    UPROPERTY( EditAnywhere, Transient )
    UMaterialInstanceDynamic* DynamicMaterialInstance;

protected:
    TArray<FArianeVertexID> Vertices;
    TArray<FArianeSegmentID> Segments;
    FArianePathGeometry3D Geometry3D;
    TArray<FArianeSegment*> InvalidatedSegments;
    TArray<FArianeVertex*> InvalidatedVertices;
    TArray<Chain> Chains;
    uint32 CubicSegmentCount;
    uint32 LinearSegmentCount;
};
