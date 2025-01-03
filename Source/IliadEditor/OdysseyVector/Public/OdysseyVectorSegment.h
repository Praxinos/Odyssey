// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorPoint.h"
#include "OdysseyVectorPolygon.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorLink.h"

class FOdysseyVectorObject;
class FOdysseyVectorPath;

typedef struct _FSegmentSubLine
{
    FOdysseyVectorPoint* point[2];
    double t[2];
    double length;

   ~_FSegmentSubLine(){};
    _FSegmentSubLine( FOdysseyVectorPoint* iPoint0
                    , double iT0
                    , FOdysseyVectorPoint* iPoint1
                    , double iT1 )
    : point { iPoint0, iPoint1 }
    , t { iT0, iT1 }
    {
        length = ( iPoint1->GetCoords() - iPoint0->GetCoords() ).Distance();
    }
} FSegmentSubLine;

typedef struct _FOdysseyVectorFraction {
    // TODO : use FOdysseyVectorPolygon4 reduce memory footprint and then convert to
    // FOdysseyVectorPolygon6 using data stored in lineVertex(2] when drawn ?
    FOdysseyVectorPolygon6 polygon; // hexagons to prevent thin gap with joints
    FOdysseyVectorPoint* point[2];
    ::ULIS::FVec2D pointCoordsInParent[2];
    // TODO : compute on the fly to reduce memory footprint ?
    double xMinInParent, xMaxInParent, yMinInParent, yMaxInParent;
    float fromT; // We don't need double, at most there are 128 fractions. so 1.0/128 precision
    float toT; // We don't need double, at most there are 128 fractions. so 1.0/128 precision
    double length;

    _FOdysseyVectorFraction( FOdysseyVectorPoint* iPoint0
                           , double iFromT
                           , FOdysseyVectorPoint* iPoint1
                           , double iToT
                           , double iLength )
    : point { iPoint0, iPoint1 }
    , fromT( iFromT )
    , toT( iToT )
    , length ( iLength )
    {
    }
} FOdysseyVectorFraction;

class ODYSSEYVECTOR_API FOdysseyVectorSegment : public FOdysseyVectorLink
{
    private:
        static const uint32 mStaticClass = 0x45c58ef1; // value is crc32 FOdysseyVectorSegment

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID );

       /**
         * @brief function to allocate a new segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         * @return a pointer to the newly created segment
         */
        FOdysseyVectorSegment( FOdysseyVectorObject* iOwner
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

        uint32 GetIntersectionCount();

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

        std::list<FOdysseyVectorIntersection*>& GetIntersectionList();

       /**
         * @brief Get a pointer to the path this segment belongs to
         * @return a pointer to the path this segment belongs to
         */
        FOdysseyVectorPath* GetOwnerAsPath();
        FOdysseyVectorObject* GetOwner();

       /**
         * @brief Set the path this segment belongs to. This is called by the path itself when the segment is added.
         * @param a pointer to the path this segment belongs to
         */
        void SetOwner( FOdysseyVectorObject* iOwner );

       /**
         * @brief Update cached data for this segment.
         */
        virtual void Update( uint32 iUpdateFlags );

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


        void AddIntersection ( FOdysseyVectorIntersection* iIntersection );

       /**
         * @brief Get the number of polygons in cache.
         * @return the number of polygons in cache.
         */
        uint32 GetFractionCount();

       /**
         * @brief Get the polygons in cache.
         * @return a reference to the array of polygons.
         */
        std::vector<FOdysseyVectorFraction>& GetFractionCache();

       /**
         * @brief Get the segment's bounding box.
         * @return a reference to the segment's bounding box.
         */
        virtual ::ULIS::FRectD GetBoundingBox( bool iWorld );

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

        virtual bool Pick( const ::ULIS::FRectD& iMaskRect, uint8* iPixelData ) = 0;
        virtual bool Pick( double iX, double iY, double iRadius ) = 0;
        ::ULIS::FVec2D GetFractionCacheStartPointInParent();
        ::ULIS::FVec2D GetFractionCacheEndPointInParent();
        virtual double GetLength();

        virtual FOdysseyVectorSegment* Sample( double iFromT
                                             , double iToT
                                             , std::vector<FOdysseyVectorVertex*>& oNewVertexArray ){ return nullptr; };

        ::ULIS::FRectD& GetBBoxInParent();
        void SetBBoxInParent( const ::ULIS::FRectD& iBBoxInParent );
        virtual ::ULIS::FVec2D GetOffsetPoint( uint32 iSide, double iT );
        FOdysseyVectorIntersection* GetClosestIntersection( FOdysseyVectorVertex* iVertex );

        void AddIntersectionSlot();
        void RemoveIntersection ( FOdysseyVectorIntersection* iIntersection );
        uint32 GetIntersectionSlotCount();
        double ProjectConstrained( const ::ULIS::FVec2D& iPoint
                                , ::ULIS::FVec2D& oProjectedPoint );
        void GetUniqueIntersectionVertex( std::vector<FOdysseyVectorVertexIntersection*>& oIntersectionVertexArray );
        virtual void Split( const ::ULIS::FVec2D& iPoint
                          , double iPoinT
                          , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray );
        std::vector<FOdysseyVectorPoint>& GetFractionPointBuffer();
        void   SetTextureU( double iTextureStartU, double iTextureEndU );
        double GetTextureStartU();
        double GetTextureEndU();

    protected:
        void DrawFractionCache( BLContext* iBLContext );

    protected:
        // here we use C-style allocation to avoir unnecessary constructor calls
        // that a std::vector would perform. Moreover, we don't need to iterate
        // among the items, thus we don't need to know the size of it afterwards.
        std::vector<FOdysseyVectorPoint> mFractionPointBuffer;
        std::vector<FOdysseyVectorFraction> mFractionCache;
        std::list<FOdysseyVectorIntersection*> mIntersectionList;
        FOdysseyVectorObject* mOwner;
        ::ULIS::FRectD mBBox;
        ::ULIS::FRectD mBBoxInParent;
        bool mIsInvalidated;
        bool mIsPaintingReady;
        uint32 mID;
        uint32 mPaintingCode; // used by group paint as a boolean without needing to reinitialize its value
        double mLength;
        uint32 mIntersectionSlotCount;
        double mTextureStartU;
        double mTextureEndU;
};
