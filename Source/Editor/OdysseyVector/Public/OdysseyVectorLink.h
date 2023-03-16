#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class ODYSSEYVECTOR_API FOdysseyVectorLink
{
    protected:
        FOdysseyVectorPoint* mPoint[2];

    public:
       /**
         * @brief Static function to allocate a new link. Note: this is the proper way to allocate a new link as we don't
         * use the constructor to set parameters so that this can be derived from an UOBJECT if needed in future devs. Indeed,
         * UOBJECTs have empty constructors.
         * @param iPoint0 end point at the beginning of this link.
         * @param iPoint1 end point at the start of this link.
         */
        static FOdysseyVectorLink* New( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );

        virtual ~FOdysseyVectorLink();
         FOdysseyVectorLink();
         FOdysseyVectorLink( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );

        void Init( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );

        ::ULIS::FVec2D GetVector( bool iNormalize );
        FOdysseyVectorPoint* GetPoint( int iPointNum );
        virtual ::ULIS::FVec2D GetPointAt( double t );
        double GetStraightDistance();
};
