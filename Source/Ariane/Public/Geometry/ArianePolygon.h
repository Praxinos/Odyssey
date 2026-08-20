// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianePrimitive.h"
#include "ArianeCoreEnums.h"

#include "ArianePolygon.generated.h"

struct FArianeVertex;
struct FArianeSegmentCubic;

USTRUCT(BlueprintType)
struct ARIANE_API FArianePolygon : public FArianePrimitive
{
    GENERATED_BODY()

public:
    static uint32 StaticClass() { return    0x4cc9662f; }; // value is crc32 FArianePolygon
    virtual uint32 GetClass() override { return StaticClass(); };
    virtual bool HasBaseClass( uint32 BaseClassID ) override;

    /**
     * @brief destructor.
     */
    virtual ~FArianePolygon();
    FArianePolygon();
    /**
     * @brief constructor.
     */
        FArianePolygon( UArianeLayerDrawing* InDrawingLayer
                      , const FName& InName
                      , uint32 InCornerCount
                      , double InRadius
                      , double InStrokeWidth
                      , EArianeAllocationModel InAllocationModel );

    void SetCornerCount( uint32 InCornerCount );
    uint32 GetCornerCount();
    void SetRadius( double InRadius );
    double GetRadius();

    virtual void ResetGeometry() override;
    virtual void ReshapeGeometry() override;

protected:
    /**
     * @brief Copy this ellipse (for copy-paste operations).
     * @return a newly allocated polygon or path that looks the same as this polygon.
     */
    virtual FArianeObject* CopyShape( const FCopyArgs& CopyArgs ) override;

protected:
    UPROPERTY( EditAnywhere )
    uint32 CornerCount;

    UPROPERTY( EditAnywhere )
    double Radius;
};
