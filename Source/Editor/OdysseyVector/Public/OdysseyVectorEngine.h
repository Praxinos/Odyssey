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

        // signal flags
        static const uint64 SIGNAL_SCENE_REDRAW       = ( 1 << 0 );
        static const uint64 SIGNAL_SCENE_HIERARCHY    = ( 1 << 1 );
        static const uint64 SIGNAL_OBJECT_TRANSFORMED = ( 1 << 2 );
        static const uint64 SIGNAL_OBJECT_MODIFIED    = ( 1 << 3 );
        static const uint64 SIGNAL_OBJECT_SELECTED    = ( 1 << 4 );
        static const uint64 SIGNAL_INTERACTIVE        = ( 1 << 5 );
        static const uint64 SIGNAL_ALL                = 0xFFFFFFFFFFFFFFFF & ~SIGNAL_INTERACTIVE;

        static FSignalDelegate& OnSignalDelegate();

        std::list<FOdysseyVectorHUD*>& GetHUDList();
        void AddHUD( FOdysseyVectorHUD* iHUDObject );
        void RemoveHUD( FOdysseyVectorHUD* iHUDObject );
        void ClearHUD();
        void ResetHUD();

        /**
         * @brief Clear the mask image
         */
        void ClearMask();

        /**
         * @brief Get the rendering context (Blend2D)
         * @return a pointer to the rendering context
         */
        BLContext* GetBLContext();

        /**
         * @brief Get the color image (the main rendering buffer)
         * @return a pointer to the color image
         */
        BLImage* GetBLImage();

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
        FOdysseyVectorEngine( FOdysseyVectorScene* iScene, double iWidth, double iHeight );

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
         * @brief Fill the mask image with a shape defined by a array of points.
         * @param iPointArray a reference to the array of points.
         */
        ::ULIS::FRectD GenerateFreehandMask( std::vector<::ULIS::FVec2D>& iPointArray );

        /**
         * @brief Get the color at coordinates.
         * @param iX x-axis coordinates.
         * @param iY y-axis coordinates.
         * @param oR pointer to output 8-bit Red channel.
         * @param oG pointer to output 8-bit Green channel.
         * @param oB pointer to output 8-bit Blue channel.
         * @param oA pointer to output 8-bit Alpha channel.
         */
        void GetColorImagePixelValue( uint32 iX, uint32 iY, uint8* oR, uint8* oG, uint8* oB, uint8* oA );

        /**
         * @brief Get the color at coordinates.
         * @param iX x-axis coordinates.
         * @param iY y-axis coordinates.
         * @return the color at coordinates {iX,iY}
         */
        FColor GetColorImagePixelValue( uint32 iX, uint32 iY );

        /**
         * @brief Get the color image dimensions.
         * @param oW width
         * @param oH height
         */
        void GetColorImageSize( uint32* iW, uint32* iH );

        /**
         * @brief Get the color image dimensions.
         * @param oImageRegion a rectangle receiving the image's size
         */
        void GetColorImageSize( ::ULIS::FRectI& oImageRegion );

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
         * @brief Use the color image as the default rendering buffer.
         */
        void UseColorImage(); // TODO : rename UseDefaultImage

        /**
         * @brief Use the mask image as the default rendering buffer.
         */
        void UseMaskImage();


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
         * @brief set an image as the main rendering buffer
         * @param iImage a pointer to the image that will be used as the rendering buffer.
         */
        void UseImage( BLImage* iImage );

        /**
         * @brief render the current HUD.
         */
        void RenderHUD();

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

        /**
         * @brief Fill the mask image with an alpha Circle.
         * @param iX the circle's center on X axis.
         * @param iY the circle's center on Y axis.
         * @param iRadius the circle's radius.
         * @return the bounding box including the circle.
         */
        ::ULIS::FRectD GenerateCircleMask( double iX, double iY, double iRadius );

        /**
         * @brief Fill the mask image with an alpha Rectangle.
         * @param iRect the rectangle
         * @return the bounding box including the rectangle.
         */
        ::ULIS::FRectD GenerateRectangleMask( const ::ULIS::FRectD& iRect );

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

        virtual void UpdateShape( uint32 iUpdateFlags );


    private:
        BLContext* mBLContext;
        BLImage* mBLImage;
        BLImage* mBLMask;
        std::list<FOdysseyVectorHUD*> mHUDList;
        FOdysseyVectorGroup* mSelectionSpace;
        FOdysseyVectorScene* mScene;
        FULISInvalidTileMap mInvalidTileMap;
};
