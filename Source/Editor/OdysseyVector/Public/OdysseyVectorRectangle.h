#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

//#include "OdysseyVectorRectangle.generated.h"

class ODYSSEYVECTOR_API FOdysseyVectorRectangle : public FOdysseyVectorPath
{
    private:
        static const uint32 mStaticClass = 0x9ee34077; // value is crc32 FOdysseyVectorRectangle

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        bool HasBaseClass( uint32 iBaseClassID );

    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags );
        FOdysseyVectorObject* CopyShape();
        void UpdateShape( uint32 iUpdateFlags ) {};

    protected :
        double mWidth;
        double mHeight; 
        double mStrokeWidth;

    public:
        ~FOdysseyVectorRectangle();
        FOdysseyVectorRectangle();
        void Init( std::string iName, double iWidth, double iHeight );
        void SetSize( double iWidth, double iHeight );
        double GetWidth();
        double GetHeight();
};
