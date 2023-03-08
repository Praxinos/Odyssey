#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

#include "OdysseyVectorLink.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorLink : public UObject
{
    public:
        GENERATED_BODY()

    public:
        static UOdysseyVectorLink* New( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );
        void Init( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );

    protected:
        FOdysseyVectorPoint* mPoint[2];

    public:
        ~UOdysseyVectorLink();
         UOdysseyVectorLink();
        ::ULIS::FVec2D GetVector( bool iNormalize );
        FOdysseyVectorPoint* GetPoint( int iPointNum );
        virtual double GetDistanceSquared();
        virtual ::ULIS::FVec2D GetPointAt( double t );
        double GetStraightDistance();
};
