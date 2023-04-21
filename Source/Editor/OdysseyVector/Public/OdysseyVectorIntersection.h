#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"
//#include "OdysseyVectorCycle.h"

struct FExplorationPair
{
    FOdysseyVectorSection* returnSection;
    FOdysseyVectorSection* departSection;
    FOdysseyVectorVertexIntersection* intersectionVertex;

    FExplorationPair()
    {
        returnSection = nullptr;
        departSection = nullptr;
        intersectionVertex = nullptr;
    };

    FExplorationPair( FOdysseyVectorSection* iReturnSection
                    , FOdysseyVectorVertexIntersection* iIntersectionVertex
                    , FOdysseyVectorSection* iDepartSection )
    {
        returnSection = iReturnSection;
        departSection = iDepartSection;
        intersectionVertex = iIntersectionVertex;
    }
};


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

        void BuildExplorationPairs();
        std::vector<FExplorationPair>& GetExplorationPairs();

    protected:
        std::vector<FExplorationPair> mExplorationPairs;
        FOdysseyVectorVertexIntersection* mVertex[2];
};
