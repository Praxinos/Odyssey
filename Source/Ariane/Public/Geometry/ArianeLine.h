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
#include "ArianePrimitive.h"

#include "ArianeLine.generated.h"

struct FArianeSegmentCubic;
struct FArianeVertex;
class UArianeLayerDrawing;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeLine : public FArianePrimitive
{
    GENERATED_BODY()

public:
    static uint32 StaticClass() { return 0x2b7c4b8f; }; // value is crc32 FArianeLine
    virtual uint32 GetClass() override { return StaticClass(); };
    virtual bool HasBaseClass( uint32 iBaseClassID );

public:
    /**
     * @brief destructor.
     */
    virtual ~FArianeLine();
    FArianeLine();

    /**
     * @brief constructor.
     */
    FArianeLine( UArianeLayerDrawing* InDrawingLayer
               , const FName& InName
               , const FVector& InStartPoint
               , const FVector& InEndPoint
               , double InStrokeWidth
               , EArianeAllocationModel InAllocationModel );

    FVector GetStartPoint();
    FVector GetEndPoint();
    double GetStrokeWidth();

    void SetStartPoint( const FVector& InStartPoint );
    void SetEndPoint( const FVector& InEndPoint );
    void SetPoints( const FVector& InStartPoint, const FVector& InEndPoint );
    virtual void ResetGeometry() override;
    virtual void ReshapeGeometry() override;

protected:
    /**
     * @brief Copy this ellipse (for copy-paste operations).
     * @return a newly allocated line or path that looks the same as this line.
     */
    virtual FArianeObject* CopyShape( const FCopyArgs& CopyArgs ) override;

protected :
    UPROPERTY( EditAnywhere )
    FVector StartPoint;

    UPROPERTY( EditAnywhere )
    FVector EndPoint;
};
