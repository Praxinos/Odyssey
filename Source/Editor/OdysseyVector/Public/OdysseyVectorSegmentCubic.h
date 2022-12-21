#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorPoint.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorSegment.h"

#include "OdysseyVectorSegmentCubic.generated.h"

class UOdysseyVectorPathCubic;

typedef struct _FPolygon {
    ::ULIS::FVec2D quadVertex[4];
    ::ULIS::FVec2D lineVertex[2];
    double fromT;
    double toT;
} FPolygon;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorSegmentCubic : public UOdysseyVectorSegment
{
    public:
        GENERATED_BODY()

    public:
        void Init( UOdysseyVectorPathCubic* iPath
                 , UOdysseyVectorVertexCubic* iPoint0
                 , UOdysseyVectorVertexCubic* iPoint1 );

        static UOdysseyVectorSegmentCubic* New( UOdysseyVectorPathCubic* iPath
                                              , UOdysseyVectorVertexCubic* iPoint0
                                              , UOdysseyVectorVertexCubic* iPoint1 );

        void Init( UOdysseyVectorPathCubic* iPath
                 , UOdysseyVectorVertexCubic* iPoint0
                 , double iCtrlPoint0x
                 , double iCtrlPoint0y
                 , double iCtrlPoint1x
                 , double iCtrlPoint1y
                 , UOdysseyVectorVertexCubic* iPoint1 );

        static UOdysseyVectorSegmentCubic* New( UOdysseyVectorPathCubic* iPath
                                              , UOdysseyVectorVertexCubic* iPoint0
                                              , double iCtrlPoint0x
                                              , double iCtrlPoint0y
                                              , double iCtrlPoint1x
                                              , double iCtrlPoint1y
                                              , UOdysseyVectorVertexCubic* iPoint1 );

    protected:
        UOdysseyVectorHandleSegment mCtrlPoint[2];
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
        ~UOdysseyVectorSegmentCubic();
         UOdysseyVectorSegmentCubic();

        UOdysseyVectorHandleSegment& GetControlPoint( int iCtrlPointNum );
        void Draw( ::ULIS::FRectD &iRoi );
        void DrawStructure( ::ULIS::FRectD &iRoi, double iZoomFactor );
        void DrawIntersections ( ::ULIS::FRectD &iRoi, double iZoomFactor );
        ::ULIS::FVec2D GetPreviousVector( bool iNormalize );
        ::ULIS::FVec2D GetNextVector( bool iNormalize );
        ::ULIS::FVec2D GetVectorAtEnd( bool iNormalize );
        ::ULIS::FVec2D GetVectorAtStart( bool iNormalize );
        ::ULIS::FRectD& GetBoundingBox();
        void UpdateBoundingBox();
        bool Pick( double iX, double iY, double iRadius );
        void IncreasePolygonCache(uint32 iSize);
        void ResetPolygonCache();
        uint32 GetPolygonCount(); // TODO: use vector size() method.
        std::vector<FPolygon>& GetPolygonCache();
        void IntersectPath( UOdysseyVectorPathCubic& iPath );
        void Intersect( UOdysseyVectorSegmentCubic& iOther );
        void Update();
        void BuildVariable();
        double GetDistanceSquared();
        ::ULIS::FVec2D GetPointAt(double t);
        bool Cut( ::ULIS::FVec2D& linePoint0, ::ULIS::FVec2D& linePoint1 );
};
