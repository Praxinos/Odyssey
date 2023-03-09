#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorLoop.h"

// TODO: inherit from FOdysseyVectorLink ?
class FOdysseyVectorSection
{
    public:
       /**
         * @brief default destructor
         */
        ~FOdysseyVectorSection();

       /**
         * @brief constructor
         * @param iSegment the segment it belongs to
         * @param iVertex0 end point 0
         * @param iVertex1 end point 1
         */
        FOdysseyVectorSection( FOdysseyVectorSegment* iSegment
                             , FOdysseyVectorVertex* iVertex0
                             , FOdysseyVectorVertex* iVertex1 );

       /**
         * @brief Get the segment it lies on.
         * return a pointer to the segment it lies on.
         */
        FOdysseyVectorSegment* GetSegment();

       /**
         * @brief Get the vertex at the other end.
         * @param iVertex vertex at this end.
         * return a pointer to the vertex at the other end.
         */
        FOdysseyVectorVertex* GetOtherVertex( FOdysseyVectorVertex* iVertex );

       /**
         * @brief Get the vertex at one end.
         * @param iNum the end index (0 or 1).
         * return a pointer to the vertex at this end.
         */
        FOdysseyVectorVertex* GetVertex( int iNum );

       /**
         * @brief Get a vector tangent to this section, starting at this vertex.
         * @param iVertex the vertex
         * @param iNormalize normalize the vector or not
         * return a vector tangent to this section, starting at this vertex. 
         */
        ::ULIS::FVec2D GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iNormalize );

       /**
         * @brief Increment the number of cycles connected to this section. 
         */
        void IncrementCycleCount();

       /**
         * @brief Block the section for traversal from the vertex passed as parameter. Used by the GroupPaint class.
         * @param iVertex
         */
        void UnBlock( FOdysseyVectorVertex* iVertex );

       /**
         * @brief Unblock the section for traversal from the vertex passed as parameter. Used by the GroupPaint class.
         * @param iVertex
         */
        void Block( FOdysseyVectorVertex* iVertex );

       /**
         * @brief Check the blocking status of this section from the vertex passed as parameter. Used by the GroupPaint class.
         * @param iVertex
         * @return true or false
         */
        bool IsBlocked( FOdysseyVectorVertex* iVertex );

    protected:
        FOdysseyVectorSegment* mSegment;
        FOdysseyVectorVertex* mVertex[2];
        uint32 mFlags;
        uint32 mCycleCount;

    private:
        static const uint32 BLOCKVERTEX0 = ( 1 << 0 );
        static const uint32 BLOCKVERTEX1 = ( 1 << 1 );

};
