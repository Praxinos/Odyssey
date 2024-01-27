#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"

class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorIntersection
{
    public:
       /**
         * @brief default destructor
         */
        ~FOdysseyVectorIntersection();

       /**
         * @brief
         */
       FOdysseyVectorIntersection( double iSegmentT );


        uint32 GetIntersectionVertexID();
        void SetIntersectionVertexID( uint32 iIntersectionVertexID );

        double GetSegmentT();

    protected:
        uint32 mIntersectionVertexID;
        double mSegmentT;
};
