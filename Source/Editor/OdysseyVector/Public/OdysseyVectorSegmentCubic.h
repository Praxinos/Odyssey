#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorPoint.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexCubic.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorSegment.h"

class UOdysseyVectorPathCubic;

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
        void Init( UOdysseyVectorPathCubic* iPath
                 , FOdysseyVectorVertexCubic* iPoint0
                 , FOdysseyVectorVertexCubic* iPoint1 );

        static FOdysseyVectorSegmentCubic* New( UOdysseyVectorPathCubic* iPath
                                              , FOdysseyVectorVertexCubic* iPoint0
                                              , FOdysseyVectorVertexCubic* iPoint1 );

        void Init( UOdysseyVectorPathCubic* iPath
                 , FOdysseyVectorVertexCubic* iPoint0
                 , double iCtrlPoint0x
                 , double iCtrlPoint0y
                 , double iCtrlPoint1x
                 , double iCtrlPoint1y
                 , FOdysseyVectorVertexCubic* iPoint1 );

        static FOdysseyVectorSegmentCubic* New( UOdysseyVectorPathCubic* iPath
                                              , FOdysseyVectorVertexCubic* iPoint0
                                              , double iCtrlPoint0x
                                              , double iCtrlPoint0y
                                              , double iCtrlPoint1x
                                              , double iCtrlPoint1y
                                              , FOdysseyVectorVertexCubic* iPoint1 );

    protected:
        FOdysseyVectorHandleSegment* mCtrlPoint[2];
        std::vector<FPolygon> mPolygonCache;
        double mDistanceSquared;
        BLPath mBLPath;

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

    public:
        ~FOdysseyVectorSegmentCubic();
         FOdysseyVectorSegmentCubic();

        FOdysseyVectorHandleSegment* GetControlPoint( int iCtrlPointNum );
        void Draw( UOdysseyVectorPathCubic* iPath, ::ULIS::FRectD &iRoi );
        void DrawStructure( UOdysseyVectorPathCubic* iPath, ::ULIS::FRectD &iRoi, double iFactorX, double iFactorY );
        void DrawIntersections ( UOdysseyVectorPathCubic* iPath, ::ULIS::FRectD &iRoi, double iZoomFactor );
        ::ULIS::FVec2D GetPreviousVector( bool iNormalize );
        ::ULIS::FVec2D GetNextVector( bool iNormalize );
        ::ULIS::FVec2D GetVectorAtEnd( bool iNormalize );
        ::ULIS::FVec2D GetVectorAtStart( bool iNormalize );
        ::ULIS::FRectD& GetBoundingBox();
        void UpdateBoundingBox();
        bool Pick( double iX, double iY, double iRadius );
        void IncreasePolygonCache(uint32 iSize);
        void ResetPolygonCache();
        bool ProximityTest( double iLocalX, double iLocalY, double iDistanceTolerance, double& oDistance );
        uint32 GetPolygonCount(); // TODO: use vector size() method.
        std::vector<FPolygon>& GetPolygonCache();
        void IntersectPath( UOdysseyVectorPathCubic& iPath );
        uint32 Intersect( FOdysseyVectorSegmentCubic& iOther, double iTolerance, std::vector<FOdysseyVectorVertexIntersection*>& intersectionVertexArray );
        void Update();
        void BuildVariable();
        double GetDistanceSquared();
        ::ULIS::FVec2D GetPointAt(double t);
        ::ULIS::FVec2D GetTangentAt( double t );
        bool Cut( ::ULIS::FVec2D& linePoint0, ::ULIS::FVec2D& linePoint1 );
        FOdysseyVectorSegmentCubic* Sample( double iFromT
                                          , double iFromRadius
                                          , double iToT
                                          , double itoRadius
                                          , std::vector<FOdysseyVectorVertexCubic*>& newVertexArray );
};
