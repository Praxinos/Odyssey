#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPrimitive.h"

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
         * @brief Get object type
         * @return VECTORELLIPSETYPE.
         */
        virtual uint32 GetType() override;

    protected:
       /**
         * @brief Copy this ellipse (for copy-paste operations).
         * @return a newly allocated ellipse that looks the same as this ellipse.
         */
        virtual FOdysseyVectorObject* CopyShape() override;

       /**
         * @brief Draw this ellipse.
         * @param iFlags drawing flags from the engine.
         */
        virtual void DrawShape( BLContext* iBLContext, uint64 iFlags ) override;

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
