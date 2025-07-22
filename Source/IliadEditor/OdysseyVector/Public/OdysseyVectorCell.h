// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <ULIS>
#include "OdysseyVectorObject.h"

#include "OdysseyVectorCell.generated.h"

class FOdysseyVectorEngine;
class FOdysseyVectorGroup;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorLayer;
class IOdysseyVectorLayer;
class IOdysseyVectorCell;
class FOdysseyVectorTagInbetweener;
class IOdysseyVectorHUD;
class FOdysseyVectorHandleSegment;
class FOdysseyVectorVertex;
class FOdysseyVectorSegment;
class FOdysseyVectorPath;

// Recommended for the unreal reflection system + Garbage collection
// (however it seems to work fine with IOdysseyVectorCell declared only)
UINTERFACE(MinimalAPI, Blueprintable)
class UOdysseyVectorCell : public UInterface
{
    GENERATED_BODY()
};

class IOdysseyVectorCell
{
    GENERATED_BODY()

    public:
        virtual int32 GetIndex() = 0;
        virtual uint32 GetLength() = 0;
        virtual uint32 GetFrame() = 0;

        //ERIC PATCH: allows the tool to retrieve the block and keep it in memory while editing
        //See LoadVector() call in UOdysseyPainterEditorVectorBaseTool
        virtual TSharedPtr<::ULIS::FBlock> GetBlock() const = 0;
};

class ODYSSEYVECTOR_API FOdysseyVectorCell : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x78514e2d; // value is crc32 FOdysseyVectorCell

    public:
        DECLARE_MULTICAST_DELEGATE_TwoParams( FRequestRedrawDelegate, FOdysseyVectorGroupPaint*, uint64 iDelegateFlags )

        // Invalidation flags
        static const uint64 REDRAW_DEFAULT        = ( 1ULL << 0 );
        static const uint64 REDRAW_INTERACTIVE    = ( 1ULL << 1 );
        static const uint64 REDRAW_CLEAR_ALL      = ( 1ULL << 2 );

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

        FRequestRedrawDelegate& OnRequestRedrawDelegate();

        /**
         * @brief destructor
         */
        virtual ~FOdysseyVectorCell(){};

        /**
         * @brief constructor
         */
        FOdysseyVectorCell( IOdysseyVectorCell* iCellInterface
                          , FOdysseyVectorGroupPaint* iScene );

        void SetScene( FOdysseyVectorGroupPaint* iScene );
        FOdysseyVectorGroupPaint* GetScene();
        virtual void Invalidate( uint64 iInvalidationFlags ) override;
        FOdysseyVectorLayer* GetLayer();
        IOdysseyVectorCell* GetCellInterface();
        void GetFocusedObjectList( std::list<FOdysseyVectorObject*>& oObjectList );
        void GetFocusedAncestorList( std::list<FOdysseyVectorObject*>& oObjectList );

        FOdysseyVectorObject* GetLastSelectedObject();
        std::list<FOdysseyVectorObject*>& GetSelectedObjectList();
        void UnselectObject( FOdysseyVectorObject* iVecObj );
        void SelectObject( FOdysseyVectorObject* iVecObj );
        void ClearObjectSelection();
        bool ObjectHasFocus( FOdysseyVectorObject* iObject
                           , uint64 iTraversalFlags );
        void GetFocusedInbetweenerTagList( std::list<FOdysseyVectorTagInbetweener*>& oFocusedInbetweenerTagList );
        void GetSelectedInbetweenerTagList( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList );
        void InvalidateRect( const ::ULIS::FRectD& iRect );
        void InvalidateRect();
        void GetSelectedVerticesFromFocusedObjects( std::vector<FOdysseyVectorVertex*>& oVertexArray );
        void PickPathPoints( FOdysseyVectorGroupPaint* iScene
                           , double iWorldX
                           , double iWorldY
                           , double iWorldRadius
                           , uint64 iPickingFlags
                           , bool iStopAtFirstSuccess
                           , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray
                           , std::vector<FOdysseyVectorHandleSegment*>& oPickedHandleArray );

        void RemoveObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                          , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray );

        ::ULIS::FRectD GetInvalidatedRect();

        /**
         * @brief Select all objects that lies within the selection space
         */
        void SelectAllInSelectionSpace();

        /**
         * @brief Pick an object
         * @param iScene the root object
         * @param iRoi
         * @param iSelectionFlags FOdysseyVectorObject::PICK_MASK_BASED or FOdysseyVectorObject::PICK_MATH_BASED
         * @return an array of pointers to picked objects.
         *  FOdysseyVectorObject::PICK_MASK_BASED: in that case the mask buffer must be filled with 0xFF where picking is wanted.
         *  FOdysseyVectorObject::PICK_MATH_BASED: in that case collisions are mathematically computed.
         */
        void Pick( FOdysseyVectorGroupPaint* iScene
                 ,const ::ULIS::FRectD& iRoi
                 , std::vector<FOdysseyVectorObject*>& oPickedObjectArray
                 , uint32 iSelectionFlags );

        /**
         * @brief Attach to separated segments. They MUST belong to the same path. Use FOdysseyVectorPath::Merge() if necessary.
            Note: iVertexA and iVertexB  will be removed from the path.
         * @param iScene the root object
         * @param iVertexA
         * @param iVertexB
         * @param oAddedSegmentArray array of pointer to newly created segments.
         * @param oRemovedSegmentArray array of pointer to removed segments.
         * @return nullptr if iVertexA and iVertexA belong to different paths and have more than 1 segment already, the joint vertex otherwise.
         */
        static FOdysseyVectorVertex* Stitch( FOdysseyVectorVertex* iVertexA
                                           , FOdysseyVectorVertex* iVertexB
                                           , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                           , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                           , bool iSmooth );

        /**
         * @brief Set the selection space, i.e the group we pick objects from.
         *   Default is null, meaning the scene is the selection space.
         * @param iSelectionSpace a pointer to the selection space. Use NULL to define the scene as the selection space.
         */
        void SetSelectionSpace( FOdysseyVectorGroup* iSelectionSpace );

        /**
         * @brief Set the selection space, i.e the group we pick objects from.
         *   Default is null, meaning the scene is the selection space.
         * @param iSelectionSpace a pointer to the selection space. Use NULL to define the scene as the selection space.
         */
        FOdysseyVectorGroup* GetSelectionSpace();

        void SetBLMask( BLImage* iBLMask );

        /**
         * @brief Get the mask image
         * @return a pointer to the mask image
         */
        BLImage* GetBLMask();

        int32 GetIndex();
        uint32 GetLength();
        uint32 GetFrame();
        bool PendingRedraw();
        void SetPendingRedraw( bool iPendingRedraw );

    protected:
        virtual void Update( uint32 iUpdateFlags ) override;
        static void RecursivePick( FOdysseyVectorGroup* iSelectionSpace
                                 , FOdysseyVectorObject* iObj
                                 , std::vector<FOdysseyVectorObject*>& iSelectedObjectArray
                                 , const ::ULIS::FRectD& iRoi
                                 , uint32 iSelectionFlags );

    protected:
        FRequestRedrawDelegate mOnRequestRedrawDelegate;
        IOdysseyVectorCell* mCellInterface;
        FOdysseyVectorGroupPaint* mScene;
        FOdysseyVectorGroup* mSelectionSpace;
        std::list<FOdysseyVectorObject*> mSelectedObjectList;
        ::ULIS::FRectD mInvalidatedRect;
        BLImage* mBLMask;
        bool bPendingRedraw;
};
