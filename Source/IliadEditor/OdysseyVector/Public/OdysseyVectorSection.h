// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <list>
#include "OdysseyVectorPoint.h"
//#include "OdysseyVectorCycle.h"

class FOdysseyVectorSegment;
class FOdysseyVectorVertex;
class FOdysseyVectorCycle;
class FOdysseyVectorObject;

// TODO: inherit from FOdysseyVectorLink ? answer : no, because links should not have
// intersection vertices as endpoints as there is no way to know which segments they are on
class ODYSSEYVECTOR_API FOdysseyVectorSection
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
        FOdysseyVectorSection( FOdysseyVectorObject* iOwner
                             , FOdysseyVectorSegment* iSegment
                             , FOdysseyVectorVertex* iVertex0
                             , FOdysseyVectorVertex* iVertex1
                             , double iSectionT0
                             , double iSectionT1
                             , bool iStitchShortSections
                             , std::vector<FOdysseyVectorSection*>* oShortSectionArray );


        static void ListToArray( const std::list<FOdysseyVectorSection*>& iSectionList
                               , std::vector<FOdysseyVectorSection*>& oSectionArray );

        void Init( FOdysseyVectorObject* iOwner
                 , FOdysseyVectorSegment* iSegment
                 , FOdysseyVectorVertex* iVertex0
                 , FOdysseyVectorVertex* iVertex1
                 , double iSectionT0
                 , double iSectionT1
                 , bool iStitchShortSections
                 , std::vector<FOdysseyVectorSection*>* oShortSectionArray );

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
         * @brief Block the section for traversal from the vertex passed as parameter. Used by the GroupPaint class.
         * @param iVertex
         */
        void UnBlock( uint32 iVertexIndex );

       /**
         * @brief Unblock the section for traversal from the vertex passed as parameter. Used by the GroupPaint class.
         * @param iVertex
         */
        void Block( uint32 iVertexIndex );

       /**
         * @brief Check the blocking status of this section from the vertex passed as parameter. Used by the GroupPaint class.
         * @param iVertex
         * @return true or false
         */
        bool IsBlocked( uint32 iVertexIndex );

        ::ULIS::FVec2D& GetVector( uint32 iVertexIndex );
        bool IsLinked();
        void Link();
        void Unlink( bool iRestore  );
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
        void SetErased( bool iErased );
        bool IsErased();
        double GetLength();
        bool IsValid();
        double GetT( uint32 iIndex );
        FOdysseyVectorObject* GetOwner();
        void Merge( uint32 iPartnerID );
        void Stitch();

       /**
         * @brief Get a vector tangent to this section, starting at this vertex.
         * @param iVertexIndex index the vertex (0 or 1)
         * @param iStraight
         * @param iNormalize normalize the vector or not
         * return a vector tangent to this section, starting at this vertex.
         */
        ::ULIS::FVec2D GetVectorFromVertex( uint32 iVertexIndex
                                          , bool iStraight
                                          , bool iNormalize );

        double GetSegmentT( uint32 iIndex );
        void Print();
        void LinkWithoutStitching();
        void UnlinkWithoutStitching();
        bool IsGap();

    protected:
        FOdysseyVectorObject* mOwner;
        FOdysseyVectorSegment* mSegment;
        FOdysseyVectorVertex* mVertex[2];
        // used by the eraser tool in "section mode"
        FOdysseyVectorVertex* mOriginalVertex[2];
        uint32 mFlags;
        uint32 mCycleCount;
        FOdysseyVectorCycle* mCycle[2]; // there are 2 cycles per section at most. No need for a complicated container.
        ::ULIS::FVec2D mBezier[4];
        double mLength;
        // vectors at endpoint;
        ::ULIS::FVec2D mVector[2];
        double mSegmentT[2];

    private:
        static const uint32 BLOCKVERTEX0 = ( 1 << 0 );
        static const uint32 BLOCKVERTEX1 = ( 1 << 1 );
        static const uint32 LINKED       = ( 1 << 2 );
        static const uint32 ERASED       = ( 1 << 3 );
        static const uint32 GAP          = ( 1 << 4 );
};
