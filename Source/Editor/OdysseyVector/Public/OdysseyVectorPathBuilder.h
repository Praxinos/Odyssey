#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorPathCubic.h"

//#include "OdysseyVectorPathBuilder.generated.h"

class FOdysseyVectorPointSample : public FOdysseyVectorPoint
{
    public:
        ~FOdysseyVectorPointSample();
        FOdysseyVectorPointSample(){};
        FOdysseyVectorPointSample( double iX, double iY, double iRadius );
        FOdysseyVectorPointSample( std::vector<FOdysseyVectorPoint>& iPointBuffer ); // average values of points in array

        void SetSharp( bool iIsSharp );
        bool IsSharp();

    private:
        bool mIsSharp;
};

class FOdysseyVectorLinkSample: public FOdysseyVectorLink
{
    public:
        ~FOdysseyVectorLinkSample();
        FOdysseyVectorLinkSample(){};
        void Init( FOdysseyVectorPointSample* iSamplePoint0, FOdysseyVectorPointSample* iSamplePoint1 );

    private:
        // Stores a copy of the sample points passed as parameters in the constructor.
        // The reason behind this is the fact that the sample points are stored in a
        // buffer (std::vector), and the buffer can be reallocated then the pointers 
        // become invalid. So, it's preferable to work on a copy, we are sure that the
        // data are always valid.
        FOdysseyVectorPointSample mSamplePoint[2];
};

class ODYSSEYVECTOR_API FOdysseyVectorPathBuilder : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x6a7eba27; // value is crc32 FOdysseyVectorPathBuilder

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        bool HasBaseClass( uint32 iBaseClassID );

    private:
        double mCumulAngle;
        double mCumulAngleLimit;
        double mAngleLimit;
        uint32 mPointID;

        std::vector<FOdysseyVectorPoint> mPointBuffer;
        std::vector<FOdysseyVectorPointSample> mSampleBuffer;
        std::vector<FOdysseyVectorLinkSample> mLinkBuffer;
/*
        std::vector<FOdysseyVectorPointSample*> mSampleArray;
        std::vector<FOdysseyVectorPoint*> mPointArray;
*/
        std::vector<FOdysseyVectorVertex*> mVertexArray;

        FOdysseyVectorPathCubic* mCubicPath;
        FOdysseyVectorSegmentCubic* mCubicSegment;

        // Unimplemented. Cubic Path builder cannot be copied. It should be destroyed as soon as the curve is built
        FOdysseyVectorObject* CopyShape();

        // Draw the sample links and points
        void DrawShape( uint64 iFlags );

        // Unimplemented. Cubic Path builder cannot be picked. It should be destroyed as soon as the curve is built
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return false; };

        void UpdateShape( uint32 iUpdateFlags ) {};

        double GetSampleAngle();

        uint32 RecordVertex();
        uint32 RecordSample( double iX, double iY, double iRadius, uint32 iID );
        uint32 RecordPoint( double iX, double iY, double iRadius, uint32 iID );

        void ClearPointsUntil( uint32 iID );
        void ClearSamplesUntil( uint32 iID );

        /**
         * @brief Try to fit the cubic curve as close as possible to the sample links passed as parameter. EXPERIMENTAL
         */
        void FitSegment( FOdysseyVectorSegmentCubic& iSegment, std::vector<FOdysseyVectorLink>& iLinkArray );

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

        void AdjustHandle( FOdysseyVectorSegmentCubic* iCubicSegment, uint32 iHandleID, double iCheckAt, int iDepth );

        FOdysseyVectorLink* GetLastSampleLink();

        FOdysseyVectorPoint* GetLastSamplePoint();

        ::ULIS::FVec2D GetFirstVectorFromSamples();
        ::ULIS::FVec2D GetLastVectorFromSamples();

    public:
        static const uint32 NEWVERTEX  = ( 1 << 0 );
        static const uint32 NEWSAMPLE  = ( 1 << 1 );
        static const uint32 NEWSEGMENT = ( 1 << 2 );

        virtual ~FOdysseyVectorPathBuilder();
        FOdysseyVectorPathBuilder();

        /**
         * @brief attach a cubic path to shape.
         *
         * @param iCubicPath the cubic path
         * @param iStitchVertex start from this vertex (must be an end-point)
         */
        void Attach( FOdysseyVectorPathCubic* iCubicPath );

        /**
         * @brief Get the attached cubic path
         *
         * @return the attached cubic path
         */
        FOdysseyVectorPathCubic* GetCubicPath( );

        void RecordStart( FOdysseyVectorVertex *iVertex );
        FOdysseyVectorVertex* RecordIntermediate( double iX
                                                     , double iY
                                                     , double iRadius
                                                     , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                                                     , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray );
        FOdysseyVectorSegmentCubic* RecordEnd( FOdysseyVectorVertex *iVertex );
        uint32 GetPointCount();
};
