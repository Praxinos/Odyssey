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
class FOdysseyVectorVertex;

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
        FOdysseyVectorIntersection( FOdysseyVectorVertex* iVertex
                                  , double iSegmentT );

        FOdysseyVectorVertex* GetVertex();

        double GetSegmentT();

    protected:
        FOdysseyVectorVertex* mVertex;
        double mSegmentT;
};
