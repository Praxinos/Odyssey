// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianePrimitive.h"
#include "ArianeCoreEnums.h"

#include "ArianeEllipse.generated.h"

struct FArianeVertex;
struct FArianeSegmentCubic;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeEllipse : public FArianePrimitive
{
    GENERATED_BODY()

public:
    static uint32 StaticClass() { return  0xc76ec157; }; // value is crc32 FArianeEllipse
    virtual uint32 GetClass() override { return StaticClass(); };
    virtual bool HasBaseClass( uint32 BaseClassID ) override;

    /**
     * @brief destructor.
     */
    virtual ~FArianeEllipse();
    FArianeEllipse();
    /**
     * @brief constructor.
     */
    FArianeEllipse( UArianeImage* InImage
                  , const FName& InName
                  , double InRadiusX
                  , double InRadiusY
                  , double InStrokeWidth
                  , EArianeAllocationModel InAllocationModel );

    /**
     * @brief Get ellipse's radius on X axis.
     * @return radius on X axis
     */
    double GetRadiusX();

    /**
     * @brief Get ellipse's radius on Y axis.
     * @return radius on Y axis
     */
    double GetRadiusY();

    /**
     * @brief Set ellipse radius on both X and Y axis.
     * @param iRadius the radius.
     */
    void SetRadius( double InRadius );

    /**
     * @brief Set ellipse radius.
     * @param InRadiusX radius on X axis
     * @param InRadiusY radius on Y axis
     */
    void SetRadius( double InRadiusX, double InRadiusY );

    virtual void ResetGeometry() override;
    virtual void ReshapeGeometry() override;

protected:
    /**
     * @brief Copy this ellipse (for copy-paste operations).
     * @return a newly allocated ellipse or path that looks the same as this ellipse.
     */
    virtual FArianeObject* CopyShape( const FCopyArgs& CopyArgs ) override;

protected:
    UPROPERTY( EditAnywhere )
    double RadiusX;

    UPROPERTY( EditAnywhere )
    double RadiusY;
};
