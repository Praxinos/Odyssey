#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"
//#include "OdysseyVectorCycle.h"

class FOdysseyVectorIntersection
{
    public:
       /**
         * @brief default destructor
         */
        ~FOdysseyVectorIntersection();

       /**
         * @brief
         */
        FOdysseyVectorIntersection( FOdysseyVectorVertexIntersection* iVertex0
                                  , FOdysseyVectorVertexIntersection* iVertex1 );

       /**
         * @brief Get the vertex at the other end.
         * @param iVertex vertex at this end.
         * return a pointer to the vertex at the other end.
         */
        FOdysseyVectorVertexIntersection* GetOtherVertex( FOdysseyVectorVertexIntersection* iVertex );

        FOdysseyVectorVertexIntersection* GetVertex( int iNum );
        bool HasVertex( FOdysseyVectorVertexIntersection* iVertex );

        void BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray );

    protected:
        FOdysseyVectorVertexIntersection* mVertex[2];
};
