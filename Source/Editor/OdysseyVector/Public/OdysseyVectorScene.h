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

class ODYSSEYVECTOR_API FOdysseyVectorScene : public FOdysseyVectorGroupPaint
{
    public:

//        DECLARE_MULTICAST_DELEGATE_OneParam(FRefreshDetailsView,FOdysseyVectorScene*)
//        FRefreshDetailsView mDetailsView;

    private:
        static const uint32 mStaticClass = 0x60719e64; // value is crc32 FOdysseyVectorScene

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

    private:
        FOdysseyVectorObject* CopyShape();
        FOdysseyVectorEngine* mEngine;


        void FlipObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                        , double iXFactor
                        , double iYFactor );

    protected:
        std::list<FOdysseyVectorObject*> mSelectedObjectList;

    public:
        virtual ~FOdysseyVectorScene();
        FOdysseyVectorScene( const FString& iName );
        void FlipObjectsHorizontal( const std::list<FOdysseyVectorObject*>& iObjectList );
        void FlipObjectsVertical( const std::list<FOdysseyVectorObject*>& iObjectList );
        ::ULIS::FVec2D GetPositionFromObjects( const std::list<FOdysseyVectorObject*>& iObjectList );

        void Init( const FString& iName );
        void Select( FOdysseyVectorObject* iVecObj );
        void Unselect( FOdysseyVectorObject* iVecObj );
        void ClearSelection();
        FOdysseyVectorObject* GetLastSelected();
        std::list<FOdysseyVectorObject*>& GetSelectedObjectList();
        virtual void DrawShape( BLContext* iBLContext, double iHierarchyOpacity, uint64 iFlags ) override;
        bool PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return false; };
        ::ULIS::FVec2D GetWorldPositionFromSelection();
        void InvalidateObject( FOdysseyVectorObject* iObject );
        void RemoveObjects( const std::list<FOdysseyVectorObject*>& iObjectList );

        FOdysseyVectorGroup* GroupObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                                         , std::vector<FOdysseyVectorObject*>& oObjectArray
                                         , std::vector<FOdysseyVectorObject*>& oObjectOldParentArray );

        FOdysseyVectorGroupPaint* MakePaintGroupFromObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                                                           , std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                                           , std::vector<FOdysseyVectorObject*>& oCubicPathOldParentArray
                                                           , std::vector<FOdysseyVectorBucket*>& oRemovedBucketArray );
        uint32 GetType();

        void SetEngine( FOdysseyVectorEngine* iEngine );
        FOdysseyVectorEngine* GetEngine();
};
