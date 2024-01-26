#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorVertex.h"

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
       FOdysseyVectorIntersection( FOdysseyVectorVertex* iVertex
                                 , double iSegmentT );

       /**
         * @brief Get a vertex belonging to this intersection
         * return a pointer to the vertex
         */
        FOdysseyVectorVertex* GetVertex();

       /**
         * @brief Build the exploration pair needed to find cycles in a paint group.
         *        An exploration pair is made of a entry section, a leaving section
         *        and an intersection (this)
         * return true or false
         */
        void BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray );

        double GetSegmentT();

    protected:
        FOdysseyVectorVertex* mVertex;
        double mSegmentT;
};
