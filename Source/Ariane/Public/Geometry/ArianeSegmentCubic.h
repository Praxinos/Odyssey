// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane headers
#include "ArianeSegment.h"
#include "ArianePoint.h"
#include "ArianeHandleSegment.h"

#include "ArianeSegmentCubic.generated.h"

struct FArianeHandleSegment;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeSegmentCubic : public FArianeSegment
{
    GENERATED_BODY()

public:
    struct FSubPoint
    {
        FArianePoint* Point;
        FVector Position;
        double T;
        double Radius;
        int32 Index;

       ~FSubPoint();
        FSubPoint();
        FSubPoint( const FVector& Position
                 , double T
                 , double TRadius
                 , int32 InIndex  );
        FSubPoint( FArianePoint* InPoint
                 , double T
                 , double TRadius
                 , int32 InIndex  );
    };

    struct FSubLine
    {
        FArianePoint* Point[2];
        double T[2];
        double Length;

       ~FSubLine();
        FSubLine( FArianePoint* Point0
                , double T0
                , FArianePoint* Point1
                , double T1 );
    };

public:
    ~FArianeSegmentCubic();
    FArianeSegmentCubic();
    FArianeSegmentCubic( FArianeObject* Owner
                       , FArianeVertex* iVertex0
                       , double Handle0X
                       , double Handle0Y
                       , double Handle0Z
                       , double Handle1X
                       , double Handle1Y
                       , double Handle1Z
                       , FArianeVertex* iVertex1 );

    virtual void Update() override;
    virtual FVector GetPointAt( double T ) override;
    virtual FArianeSegmentCubic* Extract( FArianeObject* NewSegmentOwner
                                        , FArianeVertex* NewSegmentVertex0
                                        , float T0
                                        , FArianeVertex* NewSegmentVertex1
                                        , float T1 ) override;
    virtual FVector GetVectorLeavingFromVertex( FArianeVertex* Vertex, bool bNormalize ) override;
    virtual FVector GetTangentVectorAt( double T, bool bNormalize ) override;

protected:
    void BuildVariable( uint32 MinRecurse
                      , uint32 MaxRecurse );

    static void BuildVariableAdaptive( TArray<FSubPoint>& SubPoints
                                       , int32 FromSubPointIndex
                                       , int32 ToSubPointIndex
                                       , FVector Bezier[4]
                                       , uint32 RecurseDepth
                                       , uint32 MinRecurse
                                       , uint32 MaxRecurse
                                       , uint32& InOutPointCount );

protected:
    UPROPERTY( EditAnywhere )
    FArianeHandleSegment Handle0;

    UPROPERTY( EditAnywhere )
    FArianeHandleSegment Handle1;

protected:
    TArray<FArianePoint> FractionPoints;
};
