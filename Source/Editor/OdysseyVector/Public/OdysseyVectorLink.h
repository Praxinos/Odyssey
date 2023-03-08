#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class ODYSSEYVECTOR_API FOdysseyVectorLink : public UObject
{
    protected:
        FOdysseyVectorPoint* mPoint[2];

    public:
        static FOdysseyVectorLink* New( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );

        ~FOdysseyVectorLink();
         FOdysseyVectorLink();

        void Init( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );

        ::ULIS::FVec2D GetVector( bool iNormalize );
        FOdysseyVectorPoint* GetPoint( int iPointNum );
        virtual double GetDistanceSquared();
        virtual ::ULIS::FVec2D GetPointAt( double t );
        double GetStraightDistance();
};
