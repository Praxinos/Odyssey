#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"

//#include "OdysseyVectorScene.generated.h"

class FOdysseyVectorEngine;

class ODYSSEYVECTOR_API FOdysseyVectorScene : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x60719e64; // value is crc32 FOdysseyVectorScene

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

    private:
        void UpdateShape( uint32 iUpdateFlags );
        FOdysseyVectorObject* CopyShape();
        FOdysseyVectorEngine* mEngine;

    protected:
        std::list<FOdysseyVectorObject*> mSelectedObjectList;
        std::list<FOdysseyVectorObject*> mInvalidatedObjectList;

    public:
        ~FOdysseyVectorScene(){};
        FOdysseyVectorScene(){};
        void Init( std::string iName );
        void Select( FOdysseyVectorObject* iVecObj );
        void Unselect( FOdysseyVectorObject* iVecObj );
        void ClearSelection();
        FOdysseyVectorObject* GetLastSelected();
        std::list<FOdysseyVectorObject*>& GetSelectedObjectList();
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return false; };

        void InvalidateObject( FOdysseyVectorObject* iObject );
        void RemoveSelectedObjects();

        FOdysseyVectorGroup* GroupSelectdObjects();
        uint32 GetType();

        void SetEngine( FOdysseyVectorEngine* iEngine );
        FOdysseyVectorEngine* GetEngine();
};
