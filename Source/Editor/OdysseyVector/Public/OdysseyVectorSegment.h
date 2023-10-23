#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorLink.h"

class FOdysseyVectorObject;
class FOdysseyVectorPath;

typedef struct _FPolygon {
    ::ULIS::FVec2D quadVertex[4];
    double quadU[4];
    double quadV[4];
    ::ULIS::FVec2D lineVertex[2];
    ::ULIS::FVec2D lineVertexInParent[2];
    double xMinInParent, xMaxInParent, yMinInParent, yMaxInParent;
    double fromT;
    double toT;
} FPolygon;

class ODYSSEYVECTOR_API FOdysseyVectorSegment : public FOdysseyVectorLink
{
    private:
        static const uint32 mStaticClass = 0x45c58ef1; // value is crc32 FOdysseyVectorSegment

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

       /**
         * @brief function to allocate a new segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         * @return a pointer to the newly created segment
         */
        FOdysseyVectorSegment( FOdysseyVectorPath* iPath
                             , FOdysseyVectorVertex* iVertex0
                             , FOdysseyVectorVertex* iVertex1 );

        virtual ~FOdysseyVectorSegment();

        void SetID( uint32 iID );

        uint32 GetID();

        void SetPaintingCode( uint32 iPaintingCode );

        uint32 GetPaintingCode();

        void Link();
        void Unlink();

       /**
         * @brief Draw the segment
         * @param iRoi the region-of-interest
         */
        virtual void Draw( BLContext* iBLContext );

        virtual void DrawStructure( BLContext* iBLContext, FOdysseyVectorObject* iParentObject, bool iWorld ){};

        uint32 GetIntersectionVertexCount();

        void GetIntersectionVertices( std::vector<FOdysseyVectorVertex*>& oVertexArray );
        void GetAllVertices( std::vector<FOdysseyVectorVertex*>& oVertexArray );
        virtual ::ULIS::FVec2D GetHandleVector( uint32 iHandleID, bool iNormalize );
        virtual ::ULIS::FVec2D GetHandleVector( FOdysseyVectorVertex* iVertex, bool iNormalize );

       /**
         * @brief Test whether or not this segment is close to the coordinates passed as parameter
         * @param iLocalX X-coordinate (in local system)
         * @param iLocalY Y-coordinate (in local system)
         * @param iDistanceTolerance the maximum distance to the segment
         * @param oSmallestDistance the smallest distance that was tested. Valid only if return value equals true.
         * @return true or false
         */
        virtual bool ProximityTest( double iLocalX
                                  , double iLocalY
                                  , double iDistanceTolerance
                                  , double &oSmallestDistance ){ return false; };

       /**
         * @brief Get the list of intersection vertices
         * @return A reference to the list of intersection vertices
         */
        std::list<FOdysseyVectorVertexIntersection*>& GetIntersectionVertexList();

       /**
         * @brief Get a pointer to the path this segment belongs to
         * @return a pointer to the path this segment belongs to
         */
        FOdysseyVectorPath* GetPath();

       /**
         * @brief Set the path this segment belongs to. This is called by the path itself when the segment is added.
         * @param a pointer to the path this segment belongs to
         */
        void SetPath( FOdysseyVectorPath* iPath );

       /**
         * @brief Update cached data for this segment.
         */
        virtual void Update();

       /**
         * @brief Mark this segment for later update. This invalidates the path as well.
         */
        void Invalidate();

        bool IsInvalidated();

        virtual ::ULIS::FVec2D GetVectorAtEnd( bool iNormalize );
        virtual ::ULIS::FVec2D GetVectorAtStart( bool iNormalize );

       /**
         * @brief Get the vertex that belongs to this segment. Same as static_cast<FOdysseyVectorVertex*>(GetPoint(iNum))
         * @param iVertexID MUST be 0 or 1.
         * @return a pointer to the requested vertex.
         */
        FOdysseyVectorVertex* GetVertex( uint32 iVertexID );

       /**
         * @brief Remove all intersection vertices and all sections (sub-segments).
         */
        void ClearIntersections();

       /**
         * @brief Add an intersection point. This automatically creates the attached sections.
         * @param iIntersectionVertex the intersection vertex
         */
        void AddIntersection ( FOdysseyVectorVertexIntersection* iIntersectionVertex );

       /**
         * @brief Get the number of polygons in cache.
         * @return the number of polygons in cache.
         */
        uint32 GetPolygonCount();

       /**
         * @brief Get the polygons in cache.
         * @return a reference to the array of polygons.
         */
        std::vector<FPolygon>& GetPolygonCache();

       /**
         * @brief Get the segment's bounding box.
         * @return a reference to the segment's bounding box.
         */
        virtual ::ULIS::FRectD GetBoundingBox( bool iWorld ) { return mBBox; };

       /**
         * @brief Get coordinates on the segment at parameter t.
         * @param t between 0.0 and 1.0.
         * @return coordinates at t.
         */
        virtual ::ULIS::FVec2D GetPointAt( double t );

       /**
         * @brief Get a vector tangent to the segment at parameter t.
         * @param t between 0.0 and 1.0.
         * @return vector at t.
         */
        virtual ::ULIS::FVec2D GetTangentAt( double t, bool iNormalize );

        FOdysseyVectorVertex* GetOtherVertex( FOdysseyVectorVertex* iVertex );

        virtual ::ULIS::FVec2D GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iNormalize );
        void BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray );
        virtual FOdysseyVectorHandleSegment* GetHandle( FOdysseyVectorVertex* iVertex );
        virtual FOdysseyVectorHandleSegment* GetHandle( int iCtrlPointNum );

        void SetPaintingReady( bool iIsPaintingReady );
        bool IsPaintingReady();
        virtual bool HasBaseClass( uint32 iBaseClassID );
        virtual bool Pick( const ::ULIS::FRectD& iMaskRect, uint8* iPixelData ) = 0;
        virtual bool Pick( double iX, double iY, double iRadius ) = 0;
        ::ULIS::FVec2D GetPolygonCacheStartPointInParent();
        ::ULIS::FVec2D GetPolygonCacheEndPointInParent();
        virtual double GetLength();

    protected:
        void DrawPolygonCache( BLContext* iBLContext );

    protected:
        std::vector<FPolygon> mPolygonCache;
        std::list<FOdysseyVectorVertexIntersection*> mIntersectionVertexList;
        FOdysseyVectorPath* mPath;
        ::ULIS::FRectD mBBox;
        bool mIsInvalidated;
        bool mIsPaintingReady;
        uint32 mID;
        uint32 mPaintingCode; // used by group paint as a boolean without needing to reinitialize its value
        double mLength;
};
