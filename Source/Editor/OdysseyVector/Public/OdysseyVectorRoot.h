#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"

class FOdysseyVectorRoot : public FOdysseyVectorObject
{
    private:
        FOdysseyVectorObject* RecursiveSelect( FOdysseyVectorObject& iObj, double x, double y, double iRadius );
        void UpdateShape();
        FOdysseyVectorObject* CopyShape();

    protected:
        std::list<FOdysseyVectorObject*> mSelectedObjectList;
        std::list<FOdysseyVectorObject*> mInvalidatedObjectList;

    public:
        ~FOdysseyVectorRoot();
        FOdysseyVectorRoot( std::string iName );
        void Select( double x, double y, double iRadius );
        void Select( FOdysseyVectorObject& iVecObj );
        void ClearSelection();
        FOdysseyVectorObject* GetLastSelected();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        FOdysseyVectorObject* PickShape( double iX, double iY, double iRadius ) { return nullptr; };

        void Bucket( double iX, double iY, uint32 iFillColor );
        void InvalidateObject( FOdysseyVectorObject* iObject );

        FOdysseyVectorGroup* GroupSelectdObjects();
};
