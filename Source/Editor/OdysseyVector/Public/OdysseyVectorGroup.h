#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"

class FOdysseyVectorGroup : public FOdysseyVectorObject
{
    private:
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags ) { };
        FOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        FOdysseyVectorObject* CopyShape();
        void UpdateShape();

    protected:

    public:
        ~FOdysseyVectorGroup();
        FOdysseyVectorGroup();
        FOdysseyVectorGroup( std::string iName );
};
