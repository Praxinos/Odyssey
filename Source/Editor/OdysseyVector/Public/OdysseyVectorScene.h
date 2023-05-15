#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"

//#include "OdysseyVectorScene.generated.h"

class FOdysseyVectorEngine;

class ODYSSEYVECTOR_API FOdysseyVectorScene : public FOdysseyVectorGroup
{
    public:
        DECLARE_MULTICAST_DELEGATE_TwoParams(FUpdateDelegate, FOdysseyVectorScene*, uint32 iUpdateFlags )
        FUpdateDelegate mOnUpdateDelegate;

    public:
        FUpdateDelegate& OnUpdateDelegate();

//        DECLARE_MULTICAST_DELEGATE_OneParam(FRefreshDetailsView,FOdysseyVectorScene*)
//        FRefreshDetailsView mDetailsView;

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

    public:
        virtual ~FOdysseyVectorScene();
        FOdysseyVectorScene();
        virtual void Update( uint32 iUpdateFlags ) override;
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

        FOdysseyVectorGroup* GroupSelectedObjects( std::vector<FOdysseyVectorObject*>& oObjectArray
                                                 , std::vector<FOdysseyVectorObject*>& oObjectOldParentArray );
        FOdysseyVectorGroupPaint* MakePaintGroupFromSelectedObjects( std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                                                   , std::vector<FOdysseyVectorObject*>& oCubicPathOldParentArray
                                                                   , std::vector<FOdysseyVectorObject*>& oRemovedPaintGroupArray );
        uint32 GetType();

        void SetEngine( FOdysseyVectorEngine* iEngine );
        FOdysseyVectorEngine* GetEngine();
};
