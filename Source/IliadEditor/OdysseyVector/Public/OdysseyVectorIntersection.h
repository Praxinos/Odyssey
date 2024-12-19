// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
