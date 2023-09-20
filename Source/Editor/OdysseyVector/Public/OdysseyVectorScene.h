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
        void FlipSelection( bool iWorld, double iXFactor, double iYFactor );

    protected:
        std::list<FOdysseyVectorObject*> mSelectedObjectList;

    public:
        virtual ~FOdysseyVectorScene();
        FOdysseyVectorScene( const FString& iName );
        void FlipSelectionHorizontal( bool iWorld );
        void FlipSelectionVertical( bool iWorld );

        void Init( const FString& iName );
        void Select( FOdysseyVectorObject* iVecObj );
        void Unselect( FOdysseyVectorObject* iVecObj );
        void ClearSelection();
        FOdysseyVectorObject* GetLastSelected();
        std::list<FOdysseyVectorObject*>& GetSelectedObjectList();
        virtual void DrawShape( uint64 iFlags ) override;
        bool PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) { return false; };
        ::ULIS::FVec2D GetWorldPositionFromSelection();
        void InvalidateObject( FOdysseyVectorObject* iObject );
        void RemoveSelectedObjects();

        FOdysseyVectorGroup* GroupSelectedObjects( std::vector<FOdysseyVectorObject*>& oObjectArray
                                                 , std::vector<FOdysseyVectorObject*>& oObjectOldParentArray );
        FOdysseyVectorGroupPaint* MakePaintGroupFromSelectedObjects( std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                                                   , std::vector<FOdysseyVectorObject*>& oCubicPathOldParentArray
                                                                   , std::vector<FOdysseyVectorBucket*>& oRemovedBucketArray );
        uint32 GetType();

        void SetEngine( FOdysseyVectorEngine* iEngine );
        FOdysseyVectorEngine* GetEngine();
};
