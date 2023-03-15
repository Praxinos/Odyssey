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
        std::vector<FOdysseyVectorPoint*> mSamplePointArray;
        std::vector<FOdysseyVectorPoint*> mPointArray;
        std::vector<FOdysseyVectorPoint> mPointBuffer;

        // Unimplemented. Cubic Path builder cannot be copied. It should be destroyed as soon as the curve is built
        UOdysseyVectorObject* CopyShape();

        // Draw the sample links and points
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );

        // Unimplemented. Cubic Path builder cannot be picked. It should be destroyed as soon as the curve is built
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return false; };

        void UpdateShape( uint32 iUpdateFlags ) {};

        void Reset();

        FOdysseyVectorSegmentCubic* MakeSegment( FOdysseyVectorVertexCubic* iVertex0, FOdysseyVectorVertexCubic* iVertex1 );
        void RecordPoint( FOdysseyVectorPoint* iPoint );
        double RecordSample( FOdysseyVectorPoint* iPoint );

        /**
         * @brief Try to fit the cubic curve as close as possible to the sample links passed as parameter. EXPERIMENTAL
         */
        void FitSegment( FOdysseyVectorSegmentCubic& iSegment, std::list<FOdysseyVectorLink*>& iLinkList );

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

        ::ULIS::FVec2D GetFirstVectorFromSamples();
        ::ULIS::FVec2D GetLastVectorFromSamples();

    protected :
        UOdysseyVectorPathCubic* mCubicPath;

    public:
       ~UOdysseyVectorPathBuilder();
        UOdysseyVectorPathBuilder();

        /**
         * @brief attach a cubic path to shape.
         *
         * @param iCubicPath the cubic path
         * @param iStitchVertex start from this vertex (must be an end-point)
         */
        void Attach( UOdysseyVectorPathCubic* iCubicPath );

        /**
         * @brief Get the attached cubic path
         *
         * @return the attached cubic path
         */
        UOdysseyVectorPathCubic* GetCubicPath( );

        FOdysseyVectorSegmentCubic* RecordVertex( FOdysseyVectorVertexCubic* iRecordedVertex
                                                , FOdysseyVectorVertexCubic* iPreviousVertex
                                                , double iX
                                                , double iY
                                                , double iRadius );
};
