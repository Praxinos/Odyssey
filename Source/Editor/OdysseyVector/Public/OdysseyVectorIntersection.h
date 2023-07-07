#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"

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

       /**
         * @brief Get a vertex belonging to this intersection
         * @param the vertex index
         * return a pointer to the vertex
         */
        FOdysseyVectorVertexIntersection* GetVertex( int iNum );

       /**
         * @brief Checks whether or not a vertex belongs to this intersection
         * @param a pointer to the tested vertex
         * return true or false
         */
        bool HasVertex( FOdysseyVectorVertexIntersection* iVertex );

       /**
         * @brief Build the exploration pair needed to find cycles in a paint group.
         *        An exploration pair is made of a entry section, a leaving section
         *        and an intersection (this)
         * return true or false
         */
        void BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray );

    protected:
        FOdysseyVectorVertexIntersection* mVertex[2];
};
