// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>

#include "OdysseyVectorPrimitive.h"

class FOdysseyVectorVertex;
class FOdysseyVectorSegmentCubic;

class ODYSSEYVECTOR_API FOdysseyVectorEllipse : public FOdysseyVectorPrimitive
{
    private:
        static const uint32 mStaticClass = 0x1147fdfe; // value is crc32 FOdysseyVectorEllipse

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

       /**
         * @brief destructor.
         */
        virtual ~FOdysseyVectorEllipse();

       /**
         * @brief constructor.
         */
         FOdysseyVectorEllipse( const FString& iName, double iRadiusX, double iRadiusY, double iStrokeWidth );

       /**
         * @brief Get ellipse's radius on X axis.
         * @return radius on X axis
         */
        double GetRadiusX();

       /**
         * @brief Get ellipse's radius on Y axis.
         * @return radius on Y axis
         */
        double GetRadiusY();

       /**
         * @brief Set ellipse radius on both X and Y axis.
         * @param iRadius the radius.
         */
        void SetRadius( double iRadius );

       /**
         * @brief Set ellipse radius.
         * @param iRadiusX radius on X axis
         * @param iRadiusY radius on Y axis
         */
        void SetRadius( double iRadiusX, double iRadiusY );


    protected:
       /**
         * @brief Copy this ellipse (for copy-paste operations).
         * @return a newly allocated ellipse that looks the same as this ellipse.
         */
        virtual FOdysseyVectorObject* CopyShape( uint64 iCopyFlags ) override;

       /**
         * @brief Draw this ellipse.
         * @param iFlags drawing flags from the engine.
         */
        virtual void DrawShape( BLContext* iBLContext
                              , const ::ULIS::FRectD& iInvalidationArea
                              , double iCombinedOpacity
                              , uint64 iFlags ) override;

       /**
         * @brief Update this ellipse (update cached data).
         * @param iFlags update flags from the engine.
         */
        virtual void UpdateShape( uint32 iUpdateFlags ) override;

    protected :
        FOdysseyVectorVertex* mCubicVertex[4];
        FOdysseyVectorSegmentCubic* mCubicSegment[4];
        double mStrokeWidth;
        double mRadiusX;
        double mRadiusY;
};
