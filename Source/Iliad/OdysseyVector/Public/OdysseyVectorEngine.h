// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorGroupPaint.h"
#include "HUD/OdysseyVectorHUD.h"
#include "ULISInvalidTileMap.h"

//#include "OdysseyVectorGroupPaint.generated.h"

// I use namespaces and not ENUM_CLASS_FLAGS because I can't use them as bool without a cast, which is idiotic.
namespace FPolygonDrawingFlags
{
    static const uint64 NONE              = 0;
    static const uint64 STROKE0           = 1ULL << 0;
    static const uint64 STROKE1           = 1ULL << 1;
    static const uint64 STROKE2           = 1ULL << 2;
    static const uint64 STROKE3           = 1ULL << 3;
    static const uint64 STROKEALL         = STROKE0 | STROKE1 | STROKE2 | STROKE3;
    static const uint64 FILL              = 1ULL << 4;
    static const uint64 NOOVERLAP         = 1ULL << 5;
    static const uint64 BRUSHALPHAONLY    = 1ULL << 6;
    static const uint64 BILINEARFILTERING = 1ULL << 7;
};

typedef struct _FHorizontalLine
{
    int32 y;
    int32 x0;
    double u0;
    double v0;
    int32 x1;
    double u1;
    double v1;
    int inited;

    _FHorizontalLine()
    {
        inited = 0;
    }
}
FHorizontalLine;

class ODYSSEYVECTOR_API FOdysseyVectorEngine : public FOdysseyVectorObject
{
    public:
        DECLARE_MULTICAST_DELEGATE_TwoParams( FSignalDelegate, FOdysseyVectorGroupPaint*, uint64 iDelegateFlags )

    private:
        static const uint32 mStaticClass = 0xC4C88C77; // value is crc32 FOdysseyVectorEngine

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID );

        // drawing flags
        static const uint64 DRAWING_WIREFRAME         = ( 1ULL <<  2 );
        static const uint64 DRAWING_IGNORECOLOR       = ( 1ULL <<  3 );

        // signal flags
        static const uint64 SIGNAL_SCENE_REDRAW       = ( 1ULL << 0 );
        static const uint64 SIGNAL_SCENE_HIERARCHY    = ( 1ULL << 1 );
        static const uint64 SIGNAL_OBJECT_TRANSFORMED = ( 1ULL << 2 );
        static const uint64 SIGNAL_OBJECT_MODIFIED    = ( 1ULL << 3 );
        static const uint64 SIGNAL_OBJECT_SELECTED    = ( 1ULL << 4 );
        static const uint64 SIGNAL_INTERACTIVE        = ( 1ULL << 5 );
        static const uint64 SIGNAL_SCENE_CLEAR_ALL    = ( 1ULL << 6 );
        static const uint64 SIGNAL_ALL                = 0x0FFFFFFFFFFFFFFF & (~SIGNAL_INTERACTIVE);
        static const uint64 SIGNAL_SCENE_FORCE_REDRAW = ( SIGNAL_SCENE_REDRAW
                                                        | SIGNAL_SCENE_CLEAR_ALL );

        static const uint64 SIGNAL_USER0_RESERVED = ( 1ULL << 56 );
        static const uint64 SIGNAL_USER1_RESERVED = ( 1ULL << 57 );
        static const uint64 SIGNAL_USER2_RESERVED = ( 1ULL << 58 );
        static const uint64 SIGNAL_USER3_RESERVED = ( 1ULL << 59 );
        static const uint64 SIGNAL_USER4_RESERVED = ( 1ULL << 60 );
        static const uint64 SIGNAL_USER5_RESERVED = ( 1ULL << 61 );
        static const uint64 SIGNAL_USER6_RESERVED = ( 1ULL << 62 );
        static const uint64 SIGNAL_USER7_RESERVED = ( 1ULL << 63 );

        static FSignalDelegate& OnSignalDelegate();

        std::list<FOdysseyVectorHUD*>& GetHUDList();
        void AddHUD( FOdysseyVectorHUD* iHUDObject );
        void RemoveHUD( FOdysseyVectorHUD* iHUDObject );
        void ClearHUD();
        void ResetHUD();

        /**
         * @brief Get the mask image
         * @return a pointer to the mask image
         */
        BLImage* GetBLMask();

        /**
         * @brief Destructor
         */
        ~FOdysseyVectorEngine();

        /**
         * @brief Constructor
         */
        FOdysseyVectorEngine( FOdysseyVectorGroupPaint* iScene
                            , uint32 iPreferredWidth
                            , uint32 iPreferredHeight );

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

        void PickPathPoints( FOdysseyVectorGroupPaint* iScene
                           , double iWorldX
                           , double iWorldY
                           , double iWorldRadius
                           , uint64 iPickingFlags
                           , bool iContinue
                           , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray
                           , std::vector<FOdysseyVectorHandleSegment*>& oPickedHandleArray );

        /**
         * @brief Render the scene to the current buffer
         */
        //void Render();

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
        FOdysseyVectorVertex* Stitch( FOdysseyVectorVertex* iVertexA
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

        /**
         * @brief Returns the invalid Tile map containing invalid rects
         *
         * @return const FULISInvalidTileMap&
         */
        FULISInvalidTileMap& GetInvalidTileMap();

        /**
         * @brief render the current HUD.
         */
        void RenderHUD( BLContext* iBLContext );

        /**
         * @brief Send a signal to methods registered to this delegate.
         * @param iSignalFlags SIGNAL_* flags that can be interpreted by the receiver of the signal.
         */
        void Signal( uint64 iSignalFlags );

        /**
         * @brief Set the attached scene
         * @param iScene the attached scene
         */
        void SetScene( FOdysseyVectorGroupPaint* iScene );

        /**
         * @brief Get the attached scene
         * @return a pointer to the attached scene
         */
        FOdysseyVectorGroupPaint* GetScene( );

        /**
         * @brief Select all objects that lies within the selection space
         */
        void SelectAllInSelectionSpace();

        static void GetVertexSelection( std::list<FOdysseyVectorObject*>& iVectorObjectList
                                      , std::vector<FOdysseyVectorPoint*>& iSelectedPointArray );

        ::ULIS::FRectD Render( BLContext* iBLContext, uint64 iDrawingFlags );

        void EraseSections( FOdysseyVectorGroupPaint* iScene
                          , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                          , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                          , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                          , bool iSelectedOnly );

        uint32 GetPreferredWidth();
        uint32 GetPreferredHeight();

        void TraceLine ( int32 iX0
                       , int32 iY0
                       , double iU0
                       , double iV0
                       , int32 iX1
                       , int32 iY1
                       , double iU1
                       , double iV1
                       , uint32 iImageWidth
                       , uint32 iImageHeight );

        void FillQuad( BLContext* iBLContext
                     , const ::ULIS::FVec2D* iPoint
                     , const double* iU
                     , const double* iV
                     , double iOpacity
                     //
                     , const FColor& iColor
                     //
                     , const int8*  iBrushPixelData
                     , uint32 iBrushWidth
                     , uint32 iBrushHeight
                     , int32  iBrushBitsPerPixel
                     , uint64 iPolygonDrawingFlags );

        void FillTriangle( BLContext* iBLContext
                         , const ::ULIS::FVec2D* iPoint
                         , const double* iU
                         , const double* iV
                         , double iOpacity
                         //
                         , const FColor& iColor
                         //
                         , const int8*  iBrushPixelData
                         , uint32 iBrushWidth
                         , uint32 iBrushHeight
                         , int32  iBrushBitsPerPixel
                         , uint64 iPolygonDrawingFlags );

        void SetBLMask( BLImage* iBLMask );

        BLImageData& GetRenderData();

        void GetSelectedVerticesFromFocusedObjects( std::vector<FOdysseyVectorVertex*>& oVertexArray );


        static const uint64 TRAVERSE_STOP                   = ( 1 << 0 );
        static const uint64 TRAVERSE_OBJECT_ACCEPTED        = ( 1 << 1 );
        static const uint64 TRAVERSE_PARENT_HASFOCUS        = ( 1 << 2 );
        static const uint64 TRAVERSE_OBJECT_IGNORE_CHILDREN = ( 1 << 3 );

        bool ObjectHasFocus( FOdysseyVectorGroupPaint* iScene
                           , FOdysseyVectorObject* iObject
                           , uint64 iTraversalFlags );

        uint64 Traverse( FOdysseyVectorObject* iObject
                       , uint64 iTraversalFlags
                       , std::function<uint64(FOdysseyVectorObject*,uint64)> iCallback );
        void InvalidateRect( const ::ULIS::FRectD& iRect );
        void InvalidateRect( );
        ::ULIS::FRectD GetInvalidatedRect( double iScreenWidth, double iScreenHeight );

        void GetFocusedObjectList( std::list<FOdysseyVectorObject*>& oObjectList );
        void GetFocusedAncestorList( std::list<FOdysseyVectorObject*>& oObjectList );

        FOdysseyVectorObject* GetLastSelectedObject();
        std::list<FOdysseyVectorObject*>& GetSelectedObjectList();
        void UnselectObject( FOdysseyVectorObject* iVecObj );
        void SelectObject( FOdysseyVectorObject* iVecObj );
        void ClearObjectSelection();

        static void FlipObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                               , double iXFactor
                               , double iYFactor );

        static void FlipObjectsHorizontal( const std::list<FOdysseyVectorObject*>& iObjectList );

        static void FlipObjectsVertical( const std::list<FOdysseyVectorObject*>& iObjectList );


        static FOdysseyVectorGroup* GroupObjects( FOdysseyVectorObject* iParent
                                                , const std::list<FOdysseyVectorObject*>& iObjectList
                                                , std::vector<FOdysseyVectorObject*>& oObjectArray
                                                , std::vector<FOdysseyVectorObject*>& oObjectOldParentArray );

        static FOdysseyVectorGroupPaint* MakePaintGroupFromObjects( FOdysseyVectorObject* iParent
                                                                  , const std::list<FOdysseyVectorObject*>& iObjectList
                                                                  , std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                                                  , std::vector<FOdysseyVectorObject*>& oCubicPathOldParentArray
                                                                  , std::vector<FOdysseyVectorBucket*>& oRemovedBucketArray );

        static ::ULIS::FVec2D GetPositionFromObjects( const std::list<FOdysseyVectorObject*>& iObjectList );

        void RemoveObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                          , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray);

        virtual uint32 RemoveChild ( FOdysseyVectorObject* iChild ) override;
        virtual uint32 AddChild( FOdysseyVectorObject* iChild
                               , FOdysseyVectorObject* iInsertAfter ) override;

        void DrawLineAA( int32 x0
                       , int32 y0
                       , int32 x1
                       , int32 y1
                       , const int8* iImagePixelData
                       , uint32 iImageWidth
                       , uint32 iImageHeight
                       , int32  iImageBitsPerPixel
                       , const FColor& iColor );

/*
       uint64 GetDrawingFlags();
       void SetDrawingFlags( uint64 iDrawingFlags );
*/
    protected:
        static void RecursivePick( FOdysseyVectorGroup* iSelectionSpace
                                 , FOdysseyVectorObject* iObj
                                 , std::vector<FOdysseyVectorObject*>& iSelectedObjectArray
                                 , const ::ULIS::FRectD& iRoi
                                 , uint32 iSelectionFlags );

        static void RecursiveErase( FOdysseyVectorObject* iObj
                                  , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                  , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                  , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                  , const ::ULIS::FRectD &iRoi
                                  , bool iSelectedOnly );

        static void RecursiveEraseSections( FOdysseyVectorObject* iObject
                                          , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                          , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                          , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                          , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                          , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                          , bool iSelectedOnly );

        static void GetVertexSelectionRecursive( FOdysseyVectorObject* iObject
                                               , std::vector<FOdysseyVectorPoint*>& iSelectedPointArray );

        void TracePolygon( const  ::ULIS::FVec2I* iPoint
                         , const  double* iU
                         , const  double* iV
                         , uint32 pointCount
                         , double iOpacity
                         , const  FColor& iColor
                         // temp
                         , const  int8* iBrushPixelData
                         , uint32 iBrushWidth
                         , uint32 iBrushHeight
                         , int32  iBrushBitsPerPixel
                         , uint64 iPolygonDrawingFlags );

    protected:
        std::list<FOdysseyVectorObject*> mSelectedObjectList;
        BLContextCreateInfo mCreateInfo;
        BLImage* mBLMask;
        std::list<FOdysseyVectorHUD*> mHUDList;
        FOdysseyVectorGroup* mSelectionSpace;
        FOdysseyVectorGroupPaint* mScene;
        FULISInvalidTileMap mInvalidTileMap;
        uint32 mPreferredWidth;
        uint32 mPreferredHeight;
        std::vector<FHorizontalLine> mHorizontalLineBuffer;
        uint32 mProcessorCount;
        ::ULIS::FRectD mInvalidatedRect;
        BLImageData mRenderData; // for direct drawing via our own drawing routines.
        //uint64 mDrawingFlags; // temporary, until we find a way to pass the drawing flags as arg
};
