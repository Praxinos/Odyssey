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
        static UOdysseyVectorLink* New( UOdysseyVectorPoint* iPoint0, UOdysseyVectorPoint* iPoint1 );
        void Init( UOdysseyVectorPoint* iPoint0, UOdysseyVectorPoint* iPoint1 );

    protected:
        UOdysseyVectorPoint* mPoint[2];

    public:
        ~UOdysseyVectorLink();
         UOdysseyVectorLink();
        ::ULIS::FVec2D GetVector( bool iNormalize );
        UOdysseyVectorPoint* GetPoint( int iPointNum );
        virtual double GetDistanceSquared();
        virtual ::ULIS::FVec2D GetPointAt( double t );
        double GetStraightDistance();
};
