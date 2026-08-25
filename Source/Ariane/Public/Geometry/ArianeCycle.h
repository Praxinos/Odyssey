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
#include "ArianePoint.h"
#include "ArianeGraph.h"
// Earcut headers
#include "mapbox/earcut.hpp"

#include "ArianeCycle.generated.h"

struct FArianeSegment;
struct FArianeSegmentCubic;
struct FArianeVertex;
class UMaterial;
class UArianeLayerDrawing;
struct FArianeCycle;
class UMaterialInterface;

namespace mapbox { namespace util {

template <> struct nth<0, FVector2D> { static auto get(const FVector2D& p) { return p.X; } };
template <> struct nth<1, FVector2D> { static auto get(const FVector2D& p) { return p.Y; } };

}} // namespace mapbox::util

struct ARIANE_API FArianeCycleInvalidationFlags : FArianeObjectInvalidationFlags
{
    private:
        typedef FArianeObjectInvalidationFlags Super;

    public:
        static const uint32 StaticClass() { return  0xcb3b9b90; }; // value is crc32 FArianeCycleInvalidationFlags
        virtual uint32 GetClass() override { return StaticClass(); };
        virtual bool HasBaseClass( uint32 BaseClass ) const override;

    public:
        virtual FArianeCycleInvalidationFlags& AND( const FArianeObjectInvalidationFlags& RHS ) override;
        virtual FArianeCycleInvalidationFlags& OR( const FArianeObjectInvalidationFlags& RHS ) override;
        virtual FArianeCycleInvalidationFlags& SetAll() override;
        virtual FArianeCycleInvalidationFlags& Clear() override;
        virtual bool HasAny() override;

    public:
        FArianeCycleInvalidationFlags& SetPointAltered()  { PointAltered  = 1; return *this; };

    public:
        uint32 PointAltered  : 1  = 0;
};

class FArianeCycleVertexBuffer : public FVertexBuffer
{
public:
    uint32 VertexCount = 4; // minimum default value to have a valid buffer at Init

    void Resize( uint32 InBufferSize, FRHICommandListBase& RHICmdList );
    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
};

class ARIANE_API FArianeCycleGeometry3D : public FArianeObjectGeometry3D
{
    public:
        ~FArianeCycleGeometry3D();
        FArianeCycleGeometry3D( FArianeCycle* InCycle );

        virtual void Build() override;
        FArianeCycle* GetCycle();
};

UENUM(BlueprintType)
enum class EArianeCycleFittingRule : uint8
{
    FitToPlane = 0,
    FitToPaths = 1,
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianeCycle : public FArianeObject
{
GENERATED_BODY()

public:
    static uint32 StaticClass() { return 0x99a570f4; }; // value is crc32 FArianeCycle
    virtual uint32 GetClass() override { return StaticClass(); };
    virtual bool HasBaseClass( uint32 iBaseClassID );

public:
    virtual ~FArianeCycle();
    FArianeCycle();
    FArianeCycle( UArianeLayerDrawing* InDrawingLayer
                , const FName& InName
                , EArianeAllocationModel InAllocationModel
                , FArianeCycleInvalidationFlags* InInvalidationFlags = nullptr );

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

    /** Get Path Color **/
    const FColor& GetColor();

    /** Set Path Color **/
    void  SetColor( const FColor& InColor );

    /** Set Path Material **/
    void SetMaterial( UMaterialInterface* InMaterialInterface );

    /** Get Path Material **/
    UMaterialInterface* GetMaterial();
    FArianeCycleGeometry3D& GetGeometry3D();
    TArray<int32>& GetEarcutIndices();
    TArray<FDynamicMeshVertex>& GetModelVertexCache();
    void BuildModelVertexCache();
    void ImportGraphCycle( FArianeGraph* Graph, FArianeGraph::FCycle* GraphCycle, EArianeCycleFittingRule FittingRule );

protected:
    virtual FArianeObject* CopyShape( const FCopyArgs& CopyArgs ) override;
    virtual void CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate ) override;
    static inline uint32 EvaluateSectionPointCount( FArianeGraph::FSection* Section );
    static uint32 EvaluateContourPointCount( FArianeGraph::FCycle* Cycle );
    static void EvaluateGraphCyclePointCount( FArianeGraph::FCycle* Cycle
                                            , std::vector<std::vector<FVector2D>>& EarcutContours
                                            , TArray<FArianePoint>& Points );
    void ContourToCoords( FArianeGraph* Graph
                        , FArianeGraph::FCycle* GraphCycle
                        , std::vector<FVector2D>& EarcutContour
                        , TArray<FArianePoint>& OutPoints
                        , EArianeCycleFittingRule FittingRule );
    void GraphCycleToCoords(  FArianeGraph* Graph
                            , FArianeGraph::FCycle* GraphCycle
                            , std::vector<std::vector<FVector2D>>& EarcutContours
                            , TArray<FArianePoint>& OutPoints
                            , EArianeCycleFittingRule FittingRule );
    FVector GetNodeFittedPosition( FArianeGraph* Graph, FArianeGraph::FNode* Node, EArianeCycleFittingRule FittingRule );

protected:
    UPROPERTY( EditAnywhere )
    TArray<FVector> Vertices;

    UPROPERTY( EditAnywhere )
    TArray<int> Indices;

    UPROPERTY( EditAnywhere )
    FColor Color;

    UPROPERTY( EditAnywhere )
    UMaterialInterface* MaterialInterface;

    UPROPERTY( EditAnywhere )
    TArray<int32> EarcutIndices;

    UPROPERTY( EditAnywhere )
    TArray<FArianePoint> Points;

protected:
    FArianeCycleGeometry3D Geometry3D;
    TArray<FDynamicMeshVertex> ModelVertexCache;
};
