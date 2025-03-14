// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorVertex.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorVertexIntersection;

class ODYSSEYVECTOR_API FOdysseyVectorIntersection
{
    public:
        /**
          * @brief default destructor
          */
        ~FOdysseyVectorIntersection();

        FOdysseyVectorIntersection();

        /**
          * @brief
          */
        FOdysseyVectorIntersection( FOdysseyVectorVertexIntersection* iIntersectionVertex
                                  , double iSegmentT );

        FOdysseyVectorVertexIntersection* GetIntersectionVertex();

        double GetSegmentT();

    protected:
        FOdysseyVectorVertexIntersection* mIntersectionVertex;
        double mSegmentT;
};
