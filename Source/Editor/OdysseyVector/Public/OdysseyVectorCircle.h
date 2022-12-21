#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>
#include "OdysseyVectorVertexCubic.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPathCubic.h"

#include "OdysseyVectorCircle.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorCircle : public UOdysseyVectorPathCubic
{
    public:
        GENERATED_BODY()

    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        void UpdateShape();
 
        UOdysseyVectorVertexCubic* mCubicPoint[4];
        UOdysseyVectorSegmentCubic* mCubicSegment[4];

    protected :
        double mRadiusX; 
        double mRadiusY; 

    public:
        ~UOdysseyVectorCircle();
         UOdysseyVectorCircle();
        void Init( std::string iName );
        void Init( std::string iName, double iRadius );
        void Init( std::string iName, double iRadiusX, double iRadiusY );
        void SetRadius( double iRadius );
        void SetRadius( double iRadiusX, double iRadiusY );
        double GetRadiusX();
        double GetRadiusY();
        UOdysseyVectorObject* CopyShape();
        UOdysseyVectorPathCubic* Convert();
};
