#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPrimitive.h"

class ODYSSEYVECTOR_API FOdysseyVectorLine : public FOdysseyVectorPrimitive
{
    private:
        static const uint32 mStaticClass =  0xbfb10380; // value is crc32 FOdysseyVectorLine

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

       /**
         * @brief Destructor
         */
        virtual ~FOdysseyVectorLine();

       /**
         * @brief Constructor
         * @param iName shape's name
         * @param iWidth shape's width
         * @param iHeight shape's height
         * @param iStrokeWidth shape's stroke width
         */
        FOdysseyVectorLine( FString iName, double iWidth, double iHeight, double iStrokeWidth );

       /**
         * @brief Set this line's size
         * @param iWidth shape's width
         * @param iHeight shape's height
         */
        void SetSize( double iWidth, double iHeight );

       /**
         * @brief Get this line's width
         * @return this line's width
         */
        double GetWidth();

       /**
         * @brief Get this line's height
         * @return this line's height
         */
        double GetHeight();

    protected:
        virtual void DrawShape( BLContext* iBLContext
                              , const ::ULIS::FRectD& iInvalidationArea
                              , double iAncestorsOpacity
                              , uint64 iFlags ) override;
        virtual FOdysseyVectorObject* CopyShape() override;
        virtual void UpdateShape( uint32 iUpdateFlags ) override;

    protected:
        FOdysseyVectorVertex* mCubicVertex[2];
        FOdysseyVectorSegmentCubic* mCubicSegment;
        double mStrokeWidth;
        double mWidth;
        double mHeight;
};
