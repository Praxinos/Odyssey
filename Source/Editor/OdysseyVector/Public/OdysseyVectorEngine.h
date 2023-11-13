#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorSegment.h"
#include "OdysseyVectorScene.h"
#include "HUD/OdysseyVectorHUD.h"
#include "ULISInvalidTileMap.h"

//#include "OdysseyVectorScene.generated.h"

typedef struct _FHorizontalLine
{
    int32 x0;
    int32 y0;
    double u0;
    double v0;
    int32 x1;
    int32 y1;
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
        DECLARE_MULTICAST_DELEGATE_TwoParams( FSignalDelegate, FOdysseyVectorScene*, uint64 iDelegateFlags )

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
        static const uint64 SIGNAL_ALL                = 0x0FFFFFFFFFFFFFFF & (~SIGNAL_INTERACTIVE);

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
        FOdysseyVectorEngine( FOdysseyVectorScene* iScene
                            , uint32 iPreferredWidth
                            , uint32 iPreferredHeight );

        /**
         * @brief Erase objects based on the mask image. Currently works with cubic paths only.
         * @param iScene a pointer to the root object
         * @param iRoi the region of interest.
         * @param iSelectedOnly restrict erasure to selected objects only.
         */
        void Erase( FOdysseyVectorScene* iScene
                  , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                  , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                  , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                  , ::ULIS::FRectD &iRoi
                  , bool iSelectedOnly );

        /**
         * @brief Pick an object
         * @param iScene the root object
         * @param iRoi
         * @param iSelectionFlags FOdysseyVectorObject::PICK_MASK_BASED or FOdysseyVectorObject::PICK_MATH_BASED
         * @return an array of pointers to picked objects.
         *  FOdysseyVectorObject::PICK_MASK_BASED: in that case the mask buffer must be filled with 0xFF where picking is wanted.
         *  FOdysseyVectorObject::PICK_MATH_BASED: in that case collisions are mathematically computed.
         */
        void Pick( FOdysseyVectorScene* iScene
                 ,const ::ULIS::FRectD& iRoi
                 , std::vector<FOdysseyVectorObject*>& oPickedObjectArray
                 , uint32 iSelectionFlags );

        /**
         * @brief Pick segments depending on a selection circle passed as parameters.
         * @param iScene the root object
         * @param iX "world" x-axis coordinates for the selection circle.
         * @param iY "world" y-axis coordinates for the selection circle.
         * @param oPickedSegmentArray array of returned segments matching the selection circle.
         * @param oDistance array of returned distance between the segment.
         *  and coordinates iX,iY. Distance is in local coordinates. Can be NULL.
         */
        void PickSegments( FOdysseyVectorScene* iScene
                         , bool iRestrictToSelection
                         , double iX
                         , double iY
                         , double iRadius
                         , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                         , std::vector<double>* oDistance );

        /**
         * @brief Pick segments depending on a selection circle passed as parameters.
         * @param iRestrictToSelection restrict to selected objects (and children objects)
         * @param iScene the root object
         * @param iX "world" x-axis coordinates for the selection circle.
         * @param iY "world" y-axis coordinates for the selection circle.
         * @param oPickedPointArray array of returned points matching the selection circle.
         * @param iPickingFlags can be a combination of the following bit values :
         *  FOdysseyVectorPath::PICK_HANDLE_POINT : pick point handles
         *  FOdysseyVectorPath::PICK_HANDLE_SEGMENT : pick segment handles
         *  FOdysseyVectorPath::PICK_POINT : pick vertices
         */
        void PickPoints( FOdysseyVectorScene* iScene
                       , bool iRestrictToSelection
                       , double iX
                       , double iY
                       , double iRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                       , uint64 iPickingFlags );

        static void RecursivePickCycles( FOdysseyVectorObject* iObj
                                       , double iWorldX
                                       , double iWorldY
                                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

        void PickCycles( FOdysseyVectorScene* iScene
                       , double iWorldX
                       , double iWorldY
                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

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
        void SetScene( FOdysseyVectorScene* iScene );

        /**
         * @brief Get the attached scene
         * @return a pointer to the attached scene
         */
        FOdysseyVectorScene* GetScene( );

        /**
         * @brief Select all objects that lies within the selection space
         */
        void SelectAllInSelectionSpace();

        static void GetVertexSelection( std::list<FOdysseyVectorObject*>& iVectorObjectList
                                      , std::vector<FOdysseyVectorPoint*>& iSelectedPointArray );

        void Render( BLContext* iBLContext, uint64 iDrawingFlags );

        void EraseSections( FOdysseyVectorScene* iScene
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

        void TraceHorizontalLine ( int32 iLineNumber
                                 , double iOpacity
                                 , int8*  iPixelData
                                 , uint32 iImageWidth
                                 , uint32 iImageHeight
                                 , int32  iBitsPerPixel
                                 , const FColor& iColor
                                 , int8*  iBrushPixelData
                                 , uint32 iBrushWidth
                                 , uint32 iBrushHeight
                                 , int32  iBrushBitsPerPixel
                                 , bool   iBrushAlphaOnly );
        void DrawPolygon( ::ULIS::FVec2I* iPoint
                        , double* iU
                        , double* iV
                        , uint32 pointCount
                        , double iOpacity
                        , int8*  iImagePixelData
                        , uint32 iImageWidth
                        , uint32 iImageHeight
                        , int32  iImageBitsPerPixel
                        , const FColor& iColor
                        // temp
                        , int8*  iBrushPixelData
                        , uint32 iBrushWidth
                        , uint32 iBrushHeight
                        , int32  iBrushBitsPerPixel
                        , bool   iBrushAlphaOnly );

        void DrawQuadThread( uint32 iProcessorID
                           , uint32 iProcessorCount
                           , int32  iFirstLine
                           , int32  iLastLine
                           , double iOpacity
                           , int8*  iPixelData
                           , int32  iBitsPerPixel
                           // Temp
                           , int8*  iBrushPixelData
                           , uint32 iBrushWidth
                           , uint32 iBrushHeight
                           , int32  iBrushBitsPerPixel );

        void SetBLMask( BLImage* iBLMask );



        static const uint64 TRAVERSE_STOP                   = ( 1 << 0 );
        static const uint64 TRAVERSE_OBJECT_ACCEPTED        = ( 1 << 1 );
        static const uint64 TRAVERSE_PARENT_ACCEPTED        = ( 1 << 2 );
        static const uint64 TRAVERSE_OBJECT_IGNORE_CHILDREN = ( 1 << 3 );

        bool HasFocus( FOdysseyVectorScene* iScene
                     , FOdysseyVectorObject* iObject
                     , uint64 iTraversalFlags );

        uint64 Traverse( FOdysseyVectorScene* iScene
                       , FOdysseyVectorObject* iObject
                       , uint64 iTraversalFlags
                       , std::function<uint64(FOdysseyVectorObject*,uint64)> iCallback );

       void SetInvalidatedRect( const ::ULIS::FRectD& iRect );
       void SetInvalidatedRect( const ::ULIS::FRectI& iRect );
       ::ULIS::FRectI& GetInvalidatedRect();

        void GetFocusedObjectList( std::list<FOdysseyVectorObject*>& oObjectList );
        void GetFocusedAncestorList( std::list<FOdysseyVectorObject*>& oObjectList );
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


    private:
        BLContextCreateInfo mCreateInfo;
        BLImage* mBLMask;
        std::list<FOdysseyVectorHUD*> mHUDList;
        FOdysseyVectorGroup* mSelectionSpace;
        FOdysseyVectorScene* mScene;
        FULISInvalidTileMap mInvalidTileMap;
        uint32 mPreferredWidth;
        uint32 mPreferredHeight;
        std::vector<FHorizontalLine> mHorizontalLineBuffer;
        uint32 mProcessorCount;
        ::ULIS::FRectI mInvalidatedRect;
        //uint64 mDrawingFlags; // temporary, until we find a way to pass the drawing flags as arg
};
