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

    public:
        static UOdysseyVectorCircle* New( std::string iName, double iRadiusX, double iRadiusY );
        void Init( std::string iName, double iRadiusX, double iRadiusY );

    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        void UpdateShape();
 
        UOdysseyVectorVertexCubic* mCubicVertex[4];
        UOdysseyVectorSegmentCubic* mCubicSegment[4];

    protected :
        double mRadiusX; 
        double mRadiusY; 

    public:
        ~UOdysseyVectorCircle();
         UOdysseyVectorCircle();
        void SetRadius( double iRadius );
        void SetRadius( double iRadiusX, double iRadiusY );
        double GetRadiusX();
        double GetRadiusY();
        UOdysseyVectorObject* CopyShape();
        UOdysseyVectorPathCubic* Convert();
};
