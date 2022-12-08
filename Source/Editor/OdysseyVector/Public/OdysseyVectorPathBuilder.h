#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorPathCubic.h"

class FOdysseyVectorPathBuilder : public UOdysseyVectorObject
{
    private:
        double mCumulAngle;
        double mCumulAngleLimit;
        double mLastCubicAngleLimit;
        std::list<FOdysseyVectorPoint*> mSamplePointList;
        std::list<FOdysseyVectorLink*> mSampleLinkList;
        std::list<FOdysseyVectorPoint*> mPointList;
        std::list<FOdysseyVectorLink*> mLinkList;
        FOdysseyVectorSegmentCubic* Sample( FOdysseyVectorPoint* iPoint, double iRadius, bool iEnforce );

        UOdysseyVectorObject* CopyShape();
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius ) { return nullptr; };
        void UpdateShape() {};
        void FitSegment( FOdysseyVectorSegmentCubic& iSegment, std::list<FOdysseyVectorLink*>& iLinkList );
        void ClearUntil( FOdysseyVectorPoint* iPoint );
        double GetTotalSampleLinkLength();
        ::ULIS::FVec2D GetSamplePointAtParameter( double iToTalLinkLength, double iT );
        void Adjust( FOdysseyVectorSegmentCubic& iCubicSegment );

    protected :
        /*std::list<FOdysseyVectorPoint*> mSmoothedPointList;*/
        UOdysseyVectorPathCubic* mCubicPath;
        FOdysseyVectorSegmentCubic* AppendPoint( double iX, double iY, double iRadius, bool iEnforce );
        void Round( FOdysseyVectorSegmentCubic& iCubicSegment );
        void Sharp( FOdysseyVectorSegmentCubic& iCubicSegment, ::ULIS::FVec2D iEntryVector, ::ULIS::FVec2D iExitVector );

    public:
       ~FOdysseyVectorPathBuilder();
        //FOdysseyVectorPathBuilder();
        FOdysseyVectorPathBuilder( UOdysseyVectorPathCubic* iCubicPath );
        FOdysseyVectorSegment* AppendPoint( double iX, double iY, double iRadius );

        bool PickPoint( double iX, double iY, double iRadius, uint64 iSelectionFlags );
        void Unselect(FOdysseyVectorPoint* iPoint);
        FOdysseyVectorSegment* End( double iX, double iY, double iRadius, bool iClose );
        UOdysseyVectorPathCubic* GetCubicPath( );
        FOdysseyVectorLink* GetLastSampleLink();
        FOdysseyVectorPoint* GetLastSamplePoint();

};
