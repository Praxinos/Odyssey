#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"
//#include "OdysseyVectorCycle.h"

class FOdysseyVectorSegment;
class FOdysseyVectorVertex;
class FOdysseyVectorCycle;

// TODO: inherit from FOdysseyVectorLink ? answer : no, because links should not have
// intersection vertices as endpoints as there is no way to know which segments they are on
class FOdysseyVectorSection
{
    public:
       /**
         * @brief default destructor
         */
        ~FOdysseyVectorSection();
        FOdysseyVectorSection();

       /**
         * @brief constructor
         * @param iSegment the segment it belongs to
         * @param iConversionMatrix
         * @param iVertex0 end point 0
         * @param iVertex1 end point 1
         */
        FOdysseyVectorSection( FOdysseyVectorSegment* iSegment
                             , BLMatrix2D* iConversionMatrix
                             , FOdysseyVectorVertex* iVertex0
                             , FOdysseyVectorVertex* iVertex1 );

        void Init( FOdysseyVectorSegment* iSegment
                 , BLMatrix2D* iConversionMatrix
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
         * @param iStraight
         * @param iNormalize normalize the vector or not
         * return a vector tangent to this section, starting at this vertex. 
         */
        ::ULIS::FVec2D GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iStraight, bool iNormalize );

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

        bool IsLinked();
        void Link();
        void Unlink();
        FOdysseyVectorCycle* GetCycle( uint32 iCycleID );
        void AddCycle( FOdysseyVectorCycle* iCycle );
        FOdysseyVectorCycle* GetOtherCycle( FOdysseyVectorCycle* iCycle );
        ::ULIS::FVec2D* GetBezier();
        uint32 GetFlags();
        uint32 GetCycleCount();
        bool HasCycle( FOdysseyVectorCycle* iCycle );
        ::ULIS::FVec2D GetPointAt( double t );
        ::ULIS::FVec2D GetTangentAt( double t, bool iNormalize );
        ::ULIS::FVec2D& GetVertexCoords( FOdysseyVectorVertex* iVertex );

    protected:
        FOdysseyVectorSegment* mSegment;
        FOdysseyVectorVertex* mVertex[2];
        uint32 mFlags;
        uint32 mCycleCount;
        FOdysseyVectorCycle* mCycle[2]; // there are 2 cycles per section at most. No need for a complicated container.
        ::ULIS::FVec2D mBezier[4];

    private:
        static const uint32 BLOCKVERTEX0 = ( 1 << 0 );
        static const uint32 BLOCKVERTEX1 = ( 1 << 1 );
        static const uint32 LINKED       = ( 1 << 2 );
};
