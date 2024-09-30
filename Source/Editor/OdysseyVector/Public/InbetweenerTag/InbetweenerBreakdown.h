#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "blend2d.h"

#include "InbetweenerTag/InbetweenerChart.h"

class FInbetweenerGrid;
class FOdysseyVectorTagInbetweener;
enum class eInbetweenerGridType : uint8;

class ODYSSEYVECTOR_API FInbetweenerBreakdown
{
    public:
        virtual ~FInbetweenerBreakdown();
        FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        void SetSourceDrawingIndex( uint32 iSourceDrawingIndex );
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
        FInbetweenerBreakdown* GetMasterBreakdown();
        void DrawPathsAtTarget( BLContext* iBLContext );
        void DrawPathsAtSource( BLContext* iBLContext );
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
        int32 GetTargetAnimationCellIndex();
        int32 GetSourceAnimationCellIndex();
        void UpdateMatrix();
        ::ULIS::FRectD GetTargetBBox( bool iWorld );
        ::ULIS::FRectD GetSourceBBox( bool iWorld );
        void InterpolateTransform();
        double GetTargetTranslationX();
        double GetTargetTranslationY();
        double GetTargetRotation();
        double GetTargetScalingX();
        double GetTargetScalingY();
        bool  IsMaster();
        uint32 GetDrawingCount();
        FInbetweenerChart* GetChart();

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
        uint32 mSourceDrawingIndex;
        uint32 mTargetDrawingIndex;
        uint32 mIndex;
        FInbetweenerChart mChart;
};
