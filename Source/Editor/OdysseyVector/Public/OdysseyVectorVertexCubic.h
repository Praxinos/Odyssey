#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"
#include "OdysseyVectorHandlePoint.h"

#include "OdysseyVectorVertexCubic.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorVertexCubic : public UOdysseyVectorVertex
{
    public:
        GENERATED_BODY()

    public:
        static UOdysseyVectorVertexCubic* New( double iX, double iY, double iRadius );
        void Init( double iX, double iY, double iRadius );

    private:
        UOdysseyVectorHandlePoint mCtrlPoint;

    protected:

    public:
        ~UOdysseyVectorVertexCubic();
        UOdysseyVectorVertexCubic();

        UOdysseyVectorHandlePoint& GetControlPoint();
        void Set( double iX, double iY );
        void Set( double iX, double iY, bool iBuildSegments );
        void Set( double iX, double iY, double iRadius, bool iBuildSegments );
        ::ULIS::FVec2D GetPerpendicularVector( bool iNormalize );
        void BuildSegments();
        virtual void SetRadius( double iRadius, bool iBuildSegments );
        void SmoothSegments( bool iBuildSegments );
};
