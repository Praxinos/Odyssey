#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPrimitive.h"

class ODYSSEYVECTOR_API FOdysseyVectorRectangle : public FOdysseyVectorPrimitive
{
    private:
        static const uint32 mStaticClass = 0x9ee34077; // value is crc32 FOdysseyVectorRectangle

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        bool HasBaseClass( uint32 iBaseClassID );

       /**
         * @brief destructor.
         */
        virtual ~FOdysseyVectorRectangle();

       /**
         * @brief constructor.
         */
        FOdysseyVectorRectangle( FString iName, double iWidth, double iHeight, double iStrokeWidth );

       /**
         * @brief Set rectangle size.
         * @param iWidth the width.
         * @param iHeight the height.
         */
        void SetSize( double iWidth, double iHeight );

       /**
         * @brief Get rectangle width.
         * @return the width.
         */
        double GetWidth();

       /**
         * @brief Get rectangle height.
         * @return the height.
         */
        double GetHeight();

    private:
        virtual void DrawShape( BLContext* iBLContext, double iHierarchyOpacity, uint64 iFlags ) override;
        virtual FOdysseyVectorObject* CopyShape() override;
        virtual void UpdateShape( uint32 iUpdateFlags ) override;

    protected :
        FOdysseyVectorVertex* mCubicVertex[4];
        FOdysseyVectorSegmentCubic* mCubicSegment[4];

    public:
        double mStrokeWidth;
        double mWidth;
        double mHeight;
};
