// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianePrimitive.h"
#include "ArianeCoreEnums.h"

struct FArianeVertex;
struct FArianeSegmentCubic;

struct ARIANE_API FArianeRectangle : public FArianePrimitive
{
public:
    static uint32 StaticClass() { return   0xce49ed79; }; // value is crc32 FArianeRectangle
    virtual uint32 GetClass() override { return StaticClass(); };
    virtual bool HasBaseClass( uint32 BaseClassID ) override;

    /**
     * @brief destructor.
     */
    virtual ~FArianeRectangle();
    FArianeRectangle();
    /**
     * @brief constructor.
     */
    FArianeRectangle( UArianeLayerDrawing* InDrawingLayer
                    , const FName& InName
                    , double InRadiusX
                    , double InRadiusY
                    , double InStrokeWidth
                    , EArianeAllocationModel InAllocationModel );

    double GetWidth();
    double GetHeight();
    void SetSize( double InWidth, double InHeight );
    virtual void ResetGeometry() override;
    virtual void ReshapeGeometry() override;

protected:
    /**
     * @brief Copy this ellipse (for copy-paste operations).
     * @return a newly allocated rectangle that looks the same as this rectangle.
     */
    virtual FArianeRectangle* CopyShape( const FCopyArgs& CopyArgs ) override;

protected :
    UPROPERTY( EditAnywhere )
    double Width;

    UPROPERTY( EditAnywhere )
    double Height;

protected :
    FArianeVertex* GeneratedVertices[4];
    FArianeSegmentCubic* GeneratedSegments[4];
};
