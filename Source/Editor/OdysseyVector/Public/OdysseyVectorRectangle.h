#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

class FOdysseyVectorRectangle : public FOdysseyVectorObject
{
    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        FOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        FOdysseyVectorObject* CopyShape();
        void UpdateShape() {};

    protected :
        double mWidth;
        double mHeight; 

    public:
        ~FOdysseyVectorRectangle();
        FOdysseyVectorRectangle( std::string iName );
        FOdysseyVectorRectangle( std::string iName, double iWidth, double iHeight );
        void SetSize( double iWidth, double iHeight );
        double GetWidth();
        double GetHeight();

};
