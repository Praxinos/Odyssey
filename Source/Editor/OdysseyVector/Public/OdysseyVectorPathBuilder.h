#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorPathCubic.h"

#include "OdysseyVectorPathBuilder.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorPathBuilder : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        double mCumulAngle;
        double mCumulAngleLimit;
        double mLastCubicAngleLimit;
        std::list<FOdysseyVectorPoint*> mSamplePointList;
        std::list<FOdysseyVectorLink*> mSampleLinkList;
        std::list<FOdysseyVectorPoint*> mPointList;
        std::list<FOdysseyVectorLink*> mLinkList;

        /**
         * @brief Record a sample point located at the same position as the point passed as parameter
         *
         * @param iPoint the point from which to copy the coordinates
         * @param iRadius point radius
         * @param iEnforce forces the creation of a cubic segment ending at this point's location.
         * @return the cubic segment created from the sample point and the previous sample point.
         */
        FOdysseyVectorSegmentCubic* Sample( FOdysseyVectorPoint* iPoint, double iRadius, bool iEnforce );

        // Unimplemented. Cubic Path builder cannot be copied. It should be destroyed as soon as the curve is built
        UOdysseyVectorObject* CopyShape();

        // Draw the sample links and points
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );

        // Unimplemented. Cubic Path builder cannot be picked. It should be destroyed as soon as the curve is built
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return false; };

        void UpdateShape( uint32 iUpdateFlags ) {};

        /**
         * @brief Try to fit the cubic curve as close as possible to the sample links passed as parameter. EXPERIMENTAL
         */
        void FitSegment( FOdysseyVectorSegmentCubic& iSegment, std::list<FOdysseyVectorLink*>& iLinkList );

        /**
         * @brief Clear all sample points until the point passed as parameter
         * The sample points are the one used to fit the cubic curve
         */
        void ClearUntil( FOdysseyVectorPoint* iPoint );

        /**
         * @brief Clear all sample points until the point passed as parameter
         * The sample points are the one used to fit the cubic curve
         */
        double GetTotalSampleLinkLength();

        /**
         * @brief Get coordinates at T from existing links. T being between 0 and 1.
         * 
         * @param iToTalLinkLength the total length of the links
         * @param iT a value between 0 and 1 to compute the requested coordinates.
         */
        ::ULIS::FVec2D GetSamplePointAtParameter( double iToTalLinkLength, double iT );

        /**
         * @brief Try to fit the cubic curve as close as possible.
         */
        void Adjust( FOdysseyVectorSegmentCubic& iCubicSegment );

        FOdysseyVectorLink* GetLastSampleLink();

        FOdysseyVectorPoint* GetLastSamplePoint();

    protected :
        UOdysseyVectorPathCubic* mCubicPath;
        FOdysseyVectorVertexCubic* mStitchVertex;

        /**
         * @brief append a point at coordinates iX, iY with radius iRadius
         *
         * @param iX x coordinates
         * @param iY y coordinates
         * @param iRadius point radius
         * @param iEnforce forces the creation of a segment
         *
         * @return a cubic segment (if any)
         */
        FOdysseyVectorSegmentCubic* AppendPoint( double iX, double iY, double iRadius, bool iEnforce );

        /**
         * @brief shape the segment according to an entry vector and an exit vector
         *
         * @param iEntryVector entry vector (at point 0)
         * @param iExitVector exit vector (at point 1)
         */
        void Sharp( FOdysseyVectorSegmentCubic& iCubicSegment, ::ULIS::FVec2D iEntryVector, ::ULIS::FVec2D iExitVector );

    public:
       ~UOdysseyVectorPathBuilder();
        UOdysseyVectorPathBuilder();

        /**
         * @brief attach a cubic path to shape.
         *
         * @param iCubicPath the cubic path
         * @param iStitchVertex start from this vertex (must be an end-point)
         */
        void Attach( UOdysseyVectorPathCubic* iCubicPath, FOdysseyVectorVertexCubic* iStitchVertex );

        /**
         * @brief convenience function for AppendPoint( double iX, double iY, double iRadius, bool iEnforce );
         */
        FOdysseyVectorSegment* AppendPoint( double iX, double iY, double iRadius );

        /**
         * @brief record the last sample point and close the cubic path if needed
         *
         * @param iX x coordinates
         * @param iY y coordinates
         * @param iRadius point radius
         * @param iClose close the path (link to the first point)
         */
        FOdysseyVectorSegment* End( double iX, double iY, double iRadius, bool iClose );

        /**
         * @brief Get the attached cubic path
         *
         * @return the attached cubic path
         */
        UOdysseyVectorPathCubic* GetCubicPath( );
};
