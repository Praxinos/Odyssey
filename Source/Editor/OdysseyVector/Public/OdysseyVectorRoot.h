#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"

#include "OdysseyVectorRoot.generated.h"

class FOdysseyVectorEngine;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorRoot : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        void UpdateShape();
        UOdysseyVectorObject* CopyShape();
        FOdysseyVectorEngine* mEngine;

    protected:
        std::list<UOdysseyVectorObject*> mSelectedObjectList;
        std::list<UOdysseyVectorObject*> mInvalidatedObjectList;

    public:
        ~UOdysseyVectorRoot(){};
        UOdysseyVectorRoot(){};
        void Init( std::string iName );
        void Select( UOdysseyVectorObject* iVecObj );
        void Unselect( UOdysseyVectorObject* iVecObj );
        void ClearSelection();
        UOdysseyVectorObject* GetLastSelected();
        std::list<UOdysseyVectorObject*>& GetSelectedObjectList();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return nullptr; };
        void Bucket( double iX, double iY, uint32 iFillColor );
        void InvalidateObject( UOdysseyVectorObject* iObject );
        void RemoveSelectedObjects();

        UOdysseyVectorGroup* GroupSelectdObjects();
        uint32 GetType();

        void SetEngine( FOdysseyVectorEngine* iEngine );
        FOdysseyVectorEngine* GetEngine();
};
