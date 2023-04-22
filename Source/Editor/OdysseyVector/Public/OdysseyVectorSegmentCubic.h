#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorIntersection.h"

class FOdysseyVectorPathCubic;

typedef struct _FPolygon {
    ::ULIS::FVec2D quadVertex[4];
    ::ULIS::FVec2D lineVertex[2];
    double xmin, xmax, ymin, ymax;
    double fromT;
    double toT;
} FPolygon;

class ODYSSEYVECTOR_API FOdysseyVectorSegmentCubic : public FOdysseyVectorSegment
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };

    public:
        virtual ~FOdysseyVectorSegmentCubic();

        FOdysseyVectorSegmentCubic();

       /**
         * @brief function to allocate a new cubic segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         * @return a pointer to the newly created segment
         */
        FOdysseyVectorSegmentCubic ( FOdysseyVectorPathCubic* iPath
                                   , FOdysseyVectorVertex* iVertex0
                                   , FOdysseyVectorVertex* iVertex1 );

       /**
         * @brief function to allocate a new cubic segment
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iCtrlPoint0x
         * @param iCtrlPoint0y
         * @param iCtrlPoint1x
         * @param iCtrlPoint1y
         * @param iVertex1
         * @return a pointer to the newly created segment
         */
        FOdysseyVectorSegmentCubic( FOdysseyVectorPathCubic* iPath
                                  , FOdysseyVectorVertex* iVertex0
                                  , double iCtrlPoint0x
                                  , double iCtrlPoint0y
                                  , double iCtrlPoint1x
                                  , double iCtrlPoint1y
                                  , FOdysseyVectorVertex* iVertex1 );

        void Smooth( double iLimitAngleInRadians );

       /**
         * @brief Init a cubic segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         */
        void Init( FOdysseyVectorPathCubic* iPath
                 , FOdysseyVectorVertex* iVertex0
                 , FOdysseyVectorVertex* iVertex1 );

       /**
         * @brief Init a cubic segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iCtrlPoint0x
         * @param iCtrlPoint0y
         * @param iCtrlPoint1x
         * @param iCtrlPoint1y
         * @param iVertex1
         */
        void Init( FOdysseyVectorPathCubic* iPath
                 , FOdysseyVectorVertex* iVertex0
                 , double iCtrlPoint0x
                 , double iCtrlPoint0y
                 , double iCtrlPoint1x
                 , double iCtrlPoint1y
                 , FOdysseyVectorVertex* iVertex1 );

       /**
         * @brief Draw the cubic segment
         * @param iRoi the region-of-interest
         */
        virtual void Draw( ::ULIS::FRectD &iRoi ) override;

        virtual void DrawStructure( ::ULIS::FRectD &iRoi, bool iWorld ) override;

       /**
         * @brief Get the segment's bounding box.
         * @return a reference to the segment's bounding box.
         */
        virtual ::ULIS::FRectD GetBoundingBox( bool iWorld ) override;

       /**
         * @brief Update cached data for this segment.
         */
        virtual void Update() override;

        ::ULIS::FVec2D GetHandleVector( uint32 iHandleID, bool iNormalize );

        void UpdateBoundingBox();

       /**
         * @brief Test if point at coordinates iX and iY hits the segment. Test is based on cached polygons.
         * @param iX
         * @param iY
         * @param iRadius ignored.
         */
        bool Pick( double iX, double iY, double iRadius );

       /**
         * @brief Increase the polygon cache.
         * @param Increase cache by iSize.
         */
        void IncreasePolygonCache( uint32 iSize );

       /**
         * @brief Reset the polygon cache.
         */
        void ResetPolygonCache();

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
                                  , double& oSmallestDistance ) override;

       /**
         * @brief Intersect this cubic segment with another cubic segment. They MUST have the same coordinate system.
         * @param iTolerance a maximum distance to consider an almost-hit as a hit.
         * @param iIntersectionArray array that receives the created intersections.
         */
        virtual uint32 Intersect( FOdysseyVectorSegment* iOther
                                , double iTolerance
                                , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray ) override;

       /**
         * @brief Builds the variable thickness segment (stores values into polygon cache).
         */
        void BuildVariable();

        ::ULIS::FVec2D GetVectorAtEnd( bool iNormalize );
        ::ULIS::FVec2D GetVectorAtStart( bool iNormalize );

       /**
         * @brief Get a handle (a control point).
         * @param iCtrlPointNum index of the handle (0 or 1).
         * @return a pointer to the requested handle.
         */
        FOdysseyVectorHandleSegment* GetHandle( int iCtrlPointNum );

       /**
         * @brief Get a handle (a control point).
         * @param iAssociatedVertex vertex associated to the handle.
         * @return a pointer to the requested handle.
         */
        FOdysseyVectorHandleSegment* GetHandle( FOdysseyVectorVertex* iAssociatedVertex );

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
         * @brief Get coordinates on the segment at parameter t.
         * @param t between 0.0 and 1.0.
         * @return coordinates at t.
         */
        virtual ::ULIS::FVec2D GetPointAt( double t ) override;

       /**
         * @brief Get a vector tangent to the segment at parameter t.
         * @param t between 0.0 and 1.0.
         * @return vector at t.
         */
        virtual ::ULIS::FVec2D GetTangentAt( double t ) override;

       /**
         * @brief Cut the segment with a straight line segment passed as parameter.
         * If the cut succeeds, it creates new vertices and new segments and stores them to
         * output arrays oNewVertexArray and oNewSegmentArray.
         * @param linePoint0
         * @param linePoint1
         * @param oNewVertexArray
         * @param oNewSegmentArray
         * @return true if there was a cut, false otherwise.
         */
        bool Cut( ::ULIS::FVec2D& linePoint0
                , ::ULIS::FVec2D& linePoint1
                , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray );

       /**
         * @brief Extract a cubic segment from this cubic segment. For T values 0.0 or 1.0, new vertices are not allocated
         * and current ones are reused. Other than that, it creates new vertices and stores them in oNewVertexArray.
         * Radii are interpolated.
         * @param iFromT
         * @param iToT
         * @return a pointer to the newly created segment.
         */
        FOdysseyVectorSegmentCubic* Sample( double iFromT
                                          , double iToT
                                          , std::vector<FOdysseyVectorVertex*>& oNewVertexArray );

    private:
        void BuildVariableAdaptive( double  iFromT
                                  , double  iToT
                                  , double  iStartRadius
                                  , double  iEndRadius
                                  , ::ULIS::FVec2D* iPrevSegmentVector
                                  , ::ULIS::FVec2D* iNextSegmentVector
                                  , int32   iMaxRecurseDepth
                                  , int    *iPolygonID );

        void BuildVariableThickness( double iFromT
                                   , double iToT
                                   , ::ULIS::FVec2D& iFromPoint
                                   , ::ULIS::FVec2D& iToPoint
                                   , ::ULIS::FVec2D* iPrevSegmentVector
                                   , ::ULIS::FVec2D* iNextSegmentVector
                                   , double iStartRadius
                                   , double iEndRadius
                                   , int    iPolygonID );

    protected:
        FOdysseyVectorHandleSegment mCtrlPoint[2];
        std::vector<FPolygon> mPolygonCache;
        BLPath mBLPath;

    private:
        static const uint32 mStaticClass = 0xccff2d66 ; // value is crc32 FOdysseyVectorSegmentCubic
};
