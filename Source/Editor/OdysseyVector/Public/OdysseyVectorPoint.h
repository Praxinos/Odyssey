#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorPoint.generated.h"

class UOdysseyVectorSegment;
class UOdysseyVectorLoop;
class FOdysseyVectorSection;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorPoint : public UObject
{
    public:
        GENERATED_BODY()

    public:
        static UOdysseyVectorPoint* New( double iX, double iY, double iRadius );
        void Init( double iX, double iY, double iRadius );

    private:
        uint32 mID;

    protected:
        ::ULIS::FVec2D mCoords;
        double mRadius;

    public:
        /*static  const uint32 POINT_TYPE_REGULAR        = 0;
        static  const uint32 POINT_TYPE_VERTEX         = 1;
        static  const uint32 POINT_TYPE_INTERSECTION   = 2;
        static  const uint32 POINT_TYPE_HANDLE_POINT   = 3;
        static  const uint32 POINT_TYPE_HANDLE_SEGMENT = 4;*/

        ~UOdysseyVectorPoint();
        UOdysseyVectorPoint();
        virtual ::ULIS::FVec2D& GetCoords();
        double GetX();
        double GetY();
        virtual void SetX( double iX );
        virtual void SetY( double iY );
        virtual void Set( double iX, double iY );
        virtual void SetRadius( double iRadius );
        double GetRadius();
};
