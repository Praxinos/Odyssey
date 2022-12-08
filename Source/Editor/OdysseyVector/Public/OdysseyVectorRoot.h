#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"

#include "OdysseyVectorRoot.generated.h"

UCLASS()
class UOdysseyVectorRoot : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        UOdysseyVectorObject* RecursiveSelect( UOdysseyVectorObject& iObj, double x, double y, double iRadius );
        void UpdateShape();
        UOdysseyVectorObject* CopyShape();

    protected:
        std::list<UOdysseyVectorObject*> mSelectedObjectList;
        std::list<UOdysseyVectorObject*> mInvalidatedObjectList;

    public:
        ~UOdysseyVectorRoot(){};
        UOdysseyVectorRoot(){};
        void Init( std::string iName );
        void Select( double x, double y, double iRadius );
        void Select( UOdysseyVectorObject& iVecObj );
        void ClearSelection();
        UOdysseyVectorObject* GetLastSelected();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius ) { return nullptr; };

        void Bucket( double iX, double iY, uint32 iFillColor );
        void InvalidateObject( UOdysseyVectorObject* iObject );

        UOdysseyVectorGroup* GroupSelectdObjects();
};
