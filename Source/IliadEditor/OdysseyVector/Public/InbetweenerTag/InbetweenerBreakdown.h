// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "blend2d.h"

#include "InbetweenerTag/InbetweenerChart.h"
#include "InbetweenerTag/InbetweenerGrid.h"

class FOdysseyVectorTagInbetweener;
class FOdysseyVectorGroupPaint;
enum class eInbetweenerGridType : uint8;
class FOdysseyVectorCell;

class ODYSSEYVECTOR_API FInbetweenerBreakdown
{
    public:
        virtual ~FInbetweenerBreakdown();
        FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        void SetTargetDrawingIndex( uint32 iTargetDrawingIndex );
        uint32 GetSourceDrawingIndex();
        uint32 GetTargetDrawingIndex();
        FInbetweenerGrid* GetGrid();
        void SetIndex( uint32 iIndex );
        uint32 GetIndex();
        void SetGrid( eInbetweenerGridType iGridType );
        BLMatrix2D& GetSourceLocalMatrix();
        BLMatrix2D& GetTargetLocalMatrix();
        BLMatrix2D& GetTargetWorldMatrix();
        BLMatrix2D& GetTargetInverseWorldMatrix();
        FOdysseyVectorTagInbetweener* GetInbetweenerTag();
        //FInbetweenerBreakdown* GetMasterBreakdown();
        void SetPrevBreakdown( FInbetweenerBreakdown* iPrevBreakdown );
        void SetNextBreakdown( FInbetweenerBreakdown* iNextBreakdown );

        FInbetweenerBreakdown* GetPrevBreakdown( );
        FInbetweenerBreakdown* GetNextBreakdown( );
        void SetInbetweenerTag( FOdysseyVectorTagInbetweener* iInbetweenerTag );
        void Translate( double iX, double iY );
        void Rotate( double iAngle );
        void Scale( double iX, double iY );
        void GetTargetTransform( double& oTranslationX
                               , double& oTranslationY
                               , double& oRotation
                               , double& oScalingX
                               , double& oScalingY );
        void SetTargetTransform( double iTranslationX
                               , double iTranslationY
                               , double iRotation
                               , double iScalingX
                               , double iScalingY );
        int32 GetTargetCellIndex();
        int32 GetSourceCellIndex();
        void UpdateMatrix();
        ::ULIS::FRectD GetTargetBBox( bool iWorld );
        ::ULIS::FRectD GetSourceBBox( bool iWorld );
        void InterpolateTransform();
        double GetTargetTranslationX();
        double GetTargetTranslationY();
        double GetTargetRotation();
        double GetTargetScalingX();
        double GetTargetScalingY();

        double GetSourceTranslationX();
        double GetSourceTranslationY();
        double GetSourceRotation();
        double GetSourceScalingX();
        double GetSourceScalingY();

        //bool  IsMaster();
        uint32 GetDrawingCount();
        FInbetweenerChart* GetChart();
        void EaseIn( float iEasing, uint32 iFactor );
        void EaseOut( float iEasing, uint32 iFactor );
        void EaseIn( float iEasing, uint32 iFactor, uint32 iFrom, uint32 iTo );
        void EaseOut( float iEasing, uint32 iFactor, uint32 iFrom, uint32 iTo );
        void EaseInAndOut( float iEasing, uint32 iFactor, FInbetweenerChart::Inbetween* iInbetween );
        FOdysseyVectorCell* GetTargetCell();
        FOdysseyVectorCell* GetSourceCell();
        void SetTargetVisibility( bool iTargetVisibility );
        bool IsTargetVisible();

    private:
        double mTargetTranslationX;
        double mTargetTranslationY;
        double mTargetRotation;
        double mTargetScalingX;
        double mTargetScalingY;
        BLMatrix2D mTargetLocalMatrix;
/*
        BLMatrix2D mTargetWorldMatrix;
        BLMatrix2D mTargetInverseWorldMatrix;
*/
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        FInbetweenerBreakdown* mPrevBreakdown;
        FInbetweenerBreakdown* mNextBreakdown;
        FInbetweenerGrid* mGrid;
        uint32 mTargetDrawingIndex;
        uint32 mIndex;
        FInbetweenerChart mChart;
        bool bTargetVisibility;
};
