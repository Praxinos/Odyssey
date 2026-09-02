// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianePath.h"
#include "ArianeCoreEnums.h"

#include "ArianePrimitive.generated.h"

class UArianeImage;

struct ARIANE_API FArianePrimitiveInvalidationFlags : FArianePathInvalidationFlags
{
    private:
        typedef FArianePathInvalidationFlags Super;

    public:
        static const uint32 StaticClass() { return  0x8dae07e0; }; // value is crc32 FArianePrimitiveInvalidationFlags
        virtual uint32 GetClass() override { return StaticClass(); } ;
        virtual bool HasBaseClass( uint32 BaseClass ) const override;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianePrimitive : public FArianePath
{
    GENERATED_BODY()

public:
    enum class EConversionFlags{ Polyline, Bezier };

public:
    static const uint32 StaticClass() { return  0xa30fda3b; }; // value is crc32 FArianePrimitive
    virtual uint32 GetClass() { return StaticClass(); };
    virtual bool HasBaseClass( uint32 iBaseClassID ) override;

    virtual ~FArianePrimitive();
    FArianePrimitive();
    FArianePrimitive( UArianeImage* InImage
                    , const FName& InName
                    , double InStrokeWidth
                    , EArianeAllocationModel InAllocationModel
                    , FArianePrimitiveInvalidationFlags* InInvalidationflags = nullptr );
    void SetStrokeWidth( double InStrokeWidth );
    double GetStrokeWidth();
    FArianePath* Convert( EConversionFlags ConversionFlags );
    virtual void ReshapeGeometry() PURE_VIRTUAL(FArianePrimitive::ReshapeGeometry,);
    virtual void ResetGeometry() PURE_VIRTUAL(FArianePrimitive::ResetGeometry,);
    virtual void PostEditUndo() override;
    virtual void PostLoad() override;

protected:
    UPROPERTY( EditAnywhere )
    double StrokeWidth;

protected:
    TArray<FArianeVertex*> GeneratedVertices;
    TArray<FArianeSegmentCubic*> GeneratedSegments;
};

// define bitwise op
ENUM_CLASS_FLAGS(FArianePrimitive::EConversionFlags)
