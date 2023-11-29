#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorBrush.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorChain.h"

#include "OdysseyVectorPath.generated.h"

class FOdysseyVectorCycle;

UENUM(BlueprintType)
enum class eJointType : uint8
{
    None   = 0,
    Radial = 1,
    Linear = 2,
    Miter  = 3
};

enum ePointSelectionFlags : uint8
{
    Vertex        = 1,
    SegmentHandle = 2,
    Bucket        = 4,
    Strict        = 8
};

// define bitwise op
ENUM_CLASS_FLAGS(ePointSelectionFlags)

enum class eSegmentAdditionFlags : uint8
{
    None                  =        0  ,
    KeepOriginalSegment   = ( 1 << 0 ),
    RemoveOriginalSegment = ( 1 << 1 ),
    CreateDerivedSegment  = ( 1 << 2 ),
    CreateNewPath         = ( 1 << 3 )
};

// define bitwise op
ENUM_CLASS_FLAGS(eSegmentAdditionFlags)

enum class eVertexAdditionFlags : uint8
{
    None                 =        0  ,
    RemoveOriginalVertex = ( 1 << 0 ),
    CreateDerivedVertex  = ( 1 << 1 ),
    CreateBoundaryVertex = ( 1 << 2 )
};

// define bitwise op
ENUM_CLASS_FLAGS(eVertexAdditionFlags)

USTRUCT()
struct FPathParam
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, Category="Default" )
    eJointType JointType;

    UPROPERTY( EditAnywhere, Category="Default" )
    double MiterLimit;

    //UPROPERTY(EditAnywhere,Category="General")
    bool Filled; // unused for now
};

class ODYSSEYVECTOR_API FOdysseyVectorPath : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x65f13c79; // value is crc32 FOdysseyVectorPath

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        bool HasBaseClass( uint32 iBaseClassID );

        static ::ULIS::FVec2D GetPerpendicularVector( FOdysseyVectorVertex* iVertex, bool iNormalize );
        static void SharpSegments( FOdysseyVectorVertex* iVertex, bool iPreserveHandleLength );
        static void SmoothSegments( FOdysseyVectorVertex* iVertex, bool iPreserveHandleLength );
        static void SmoothSegments( FOdysseyVectorVertex* iVertex
                                  , ::ULIS::FVec2D iPerpendicularVector
                                  , bool iPreserveHandleLength );
        //static
        static void DeleteVertex( FOdysseyVectorPath* iPath
                                , std::vector<FOdysseyVectorVertex*>& iPickedVertexArray
                                , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );

       /**
         * @brief Destructor
         */
        virtual ~FOdysseyVectorPath();

       /**
         * @brief Constructor
         * @param iName path's name
         */
        FOdysseyVectorPath( const FString& iName );

       /**
         * @brief Add a segment to this path.
         * @param iSegment a pointer to the segment to add.
         */
        void AddSegment( FOdysseyVectorSegment* iSegment );

       /**
         * @brief Add a vertex to this path.
         * @param iSegment a pointer to the vertex to add
         */
        void AddVertex( FOdysseyVectorVertex* iVertex );

       /**
         * @brief Cut this path's segments according to a cut line.
         * @param iLinePoint0
         * @param iLinePoint1
         * @param oNewVertexArray  array of pointers to vertices that are added. Useful for undos.
         * @param oNewSegmentArray array of pointers to segments that are added. Useful for undos.
         * @param oOldSegmentArray array of pointers to segments that are removed. Useful for undos.
         */
        void Cut( const ::ULIS::FVec2D& iLinePoint0
                , const ::ULIS::FVec2D& iLinePoint1
                , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray
                , std::vector<FOdysseyVectorSegment*>& oOldSegmentArray );

       /**
         * @brief Draw path's structure (a simple line of fixed width).
         * @param iStrokeColor color of the path to draw.
         * @param iStrokeWidth width of the path to draw.
         * @param iWorld draw in world coordinates system.
         */
        using FOdysseyVectorObject::DrawStructure;
        virtual void DrawStructure(  BLContext* iBLContext, const BLRgba32& iStrokeColor, double iStrokeWidth, bool iWorld );

       /**
         * @brief Erase path according to the mask image.
         * @param iRoi region of interest for faster discarding.
         * @param iSplit
         * @param oAddedPathArray array of pointers to added split paths.
         * @param oAddedVertexArray array of pointers to added vertices.
         * @param oAddedSegmentArray array of pointers to added segments.
         * @param oRemovedVertexArray array of pointers to removed vertices.
         * @param oRemovedSegmentArray array of pointers to removed segments.
         * @return true if the path is empty, false otherwise.
         */
        bool Erase( const ::ULIS::FRectD &iRoi
                  , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                  , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                  , bool iSplit );

       /**
         * @brief Get the joint type.
         * @return the joint type.
         */
        eJointType GetJointType();

       /**
         * @brief Get the first vertex from the list of vertices.
         * @param a pointer to the first vertex of the list, or nullptr if the list is empty.
         */
        FOdysseyVectorVertex* GetFirstVertex();

       /**
         * @brief Get the first segment from the list of segments.
         * @param a pointer to the first segment of the list, or nullptr if the list is empty.
         */
        FOdysseyVectorSegment* GetFirstSegment();

        uint32 GetIntersectionCount();

       /**
         * @brief Get the last vertex from the list of vertices.
         * @param a pointer to the last vertex of the list, or nullptr if the list is empty.
         */
        FOdysseyVectorVertex* GetLastVertex();

       /**
         * @brief Get the last segment from the list of segments.
         * @param a pointer to the last segment of the list, or nullptr if the list is empty.
         */
        FOdysseyVectorSegment* GetLastSegment();

       /**
         * @brief Get the list of segments.
         * @return a reference to the list of segments.
         */
        std::list<FOdysseyVectorSegment*>& GetSegmentList();

       /**
         * @brief Get the list of selected vertices.
         * @return a reference to the list of selected vertices.
         */
        std::list<FOdysseyVectorVertex*>& GetSelectedVertexList();

       /**
         * @brief Get the list of vertices.
         * @return a reference to the list of vertices.
         */
        std::list<FOdysseyVectorVertex*>& GetVertexList();

       /**
         * @brief Tell whether or not this path has intersections. For use by the GrouPaint class only.
         * @return true if it has any intersection, false otherwise.
         */
        bool HasIntersections();

       /**
         * @brief Mark a segment as invalidated. It will also mark the path's hierarchy as invalidated.
         * @param iSegment the segment to invalidate.
         */
        void InvalidateSegment( FOdysseyVectorSegment* iSegment );

       /**
         * @brief Mark all segments as invalidated. It will also mark the path's hierarchy as invalidated.
         */
        void InvalidateAllSegments();

       /**
         * @brief Tell if this path is marked as filled.
         * @return true if marked as filled, false otherwise.
         */
        bool IsFilled();

       /**
         * @brief Is it a closed path ?
         * @return true if it is closed, false otherwise.
         */
        bool IsLoop();

       /**
         * @brief Merge this path with the one passed as parameter. The latter is kept,
         *        its vertices and segments are only copied and converted to this path's
         *        coordinates system. Note: The merged path vertices's ID will be renumbered.
         *        This allows polling the output arrays with an index to know why new vertex
         *        corresponds to which new one. E.g oAddedVertexArray[mergedVertex->GetID()]
         * @param iPath the path to import data from.
         * @oAddedVertexArray array of copied vertices. Useful for undoing e.g.
         * @oAddedSegmentArray array of copied segments. Useful for undoing e.g.
         */
        void Merge( FOdysseyVectorPath* iPath
                  , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray );

       /**
         * @brief Pick vertices
         * @param iWorldX pick point in world coordinates.
         * @param iWorldY pick point in world coordinates.
         * @param iSelectionRadius picking radius.
         * @param oPickedPointArray returned value as an array of pointers.
         * @param picking flags PICK_*.
         * @return true if it is closed, false otherwise.
         */
        bool PickPoint( double iWorldX
                      , double iWorldY
                      , double iSelectionRadius
                      , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray
                      , std::vector<FOdysseyVectorHandleSegment*>& oPickedHandleArray
                      , uint64 iSelectionFlags );

       /**
         * @brief Pick vertices according to the mask image.
         * @param oPickedVertexArray array of pointers to picked vertices.
         */
        void PickVertex( std::vector<FOdysseyVectorVertex*>& oPickedVertexArray );

       /**
         * @brief Remove a segment from this path
         * @param iSegment a pointer to the segment to remove.
         *          Note: the segment is only removed from the list of segments, not freed.
         */
        void RemoveSegment( FOdysseyVectorSegment* iSegment );

       /**
         * @brief Remove a vertex from this path
         * @param iSegment a pointer to the vertex to remove.
         *          Note: the vertex is only removed from the list of vertices, not freed.
         */
        void RemoveVertex( FOdysseyVectorVertex* iVertex );

       /**
         * @brief Select a vertex
         * @param iVertex the vertex to select
         */
        void SelectVertex( FOdysseyVectorVertex* iVertex );

        void SelectAllVertices();

       /**
         * @brief Mark as filled or not. The closed path is filled with the background color.
         * @param iIsFilled true if filled, false otherwise.
         */
        void SetFilled( bool iIsFilled );

       /**
         * @brief Set the joint type.
         * @param mJointType the joint type.
         */
        void SetJointType( eJointType mJointType, bool iInvalidate );

       /**
         * @brief Convert this path to the coordinate system of the object passed as parameter.
                  The path's matrix can then be set to the identity matrix if this path is a 
                  child of the said object.
         */
        void SwitchSpace( FOdysseyVectorObject& iObject );

       /**
         * @brief Get all vertices and sections as arrays. For use by the GroupPaint class
         * @param oVertexArray
         * @param oSectionArray
         */
        void ToVertexAndSectionArray( std::vector<FOdysseyVectorVertex*>& oVertexArray
                                    , std::vector<FOdysseyVectorSection*>& oSectionArray );

       /**
         * @brief Unselect a vertex
         * @param iVertex the vertex to unselect
         */
        void UnselectVertex( FOdysseyVectorVertex* iVertex );

       /**
         * @brief Unselect all vertices.
         */
        void UnselectAllVertices();

        void GetSelectedVertices( std::vector<FOdysseyVectorVertex*>& oVertexArray );

        bool GetBBoxFromSelectedVertices( ::ULIS::FRectD& oBBox, bool iWorld );
        std::list<FOdysseyVectorSegment*>& GetInvalidatedSegmentList();

        virtual void Invalidate() override;
        virtual void Invalidate( uint32 iInvalidationFlags ) override;
        virtual void ApplyTransformations() override;
        virtual void ApplyMatrix( BLMatrix2D& iMatrix ) override;
        void AlterRadius( double iDeltaRadius );
        static ::ULIS::FVec2D GetAverageHandleVector( FOdysseyVectorVertex* iVertex, bool iNormalize );
        // Mask based
        void PickSegments( std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray );
        // Math based
        void PickSegments( double iWorldX
                         , double iWorldY
                         , double iWorldRadius
                         , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                         , std::vector<double>* oDistanceArray );

        void SetBrush( const FOdysseyVectorBrush& iBrush );
        FOdysseyVectorBrush& GetBrush();
        void SetMiterLimit( double iMiterLimit, bool iInvalidate );


        virtual void UpdateShape( uint32 iUpdateFlags ) override;
        virtual void DrawShape( BLContext* iBLContext, double iCombinedOpacity, uint64 iFlags ) override;
        virtual bool PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) override;
        virtual FOdysseyVectorObject* CopyShape() override;

        void RemoveAllVertices();
        void RemoveAllSegments();
        bool HasVertex( FOdysseyVectorVertex* iVertex );
        bool HasSegment( FOdysseyVectorSegment* iSegment );
        virtual void ExportParam( FOdysseyVectorObject* iDestinationObject, bool iInvalidate ) override;

    protected:
        void DrawJoint( BLContext* iBLContext, FOdysseyVectorVertex* iVertex, uint64 iFlags );
        void UpdateBBox();
        void Fill();
        void ExploreChain( FOdysseyVectorChain* iChain );
        void UpdateChain( FOdysseyVectorChain* iChain );
        void FindChains();
        void DrawChain( BLContext* iBLContext
                      , double iCombinedOpacity
                      , FOdysseyVectorChain& iChain
                      , uint64 iDrawingFlags );
        void DrawTexturedSegment( FOdysseyVectorSegment* iSegment
                                , int8*  iScreenPixels
                                , uint32 iScreenWidth
                                , uint32 iScreenHeight
                                , uint32 iScreenBitsPerPixel
                                , int8*  iTexturePixels
                                , uint32 iTextureWidth
                                , uint32 iTextureHeight
                                , uint32 iTextureBitsPerPixel
                                , double iStartU
                                , double iEndU
                                , double iCombinedOpacity
                                , uint64 iDrawingFlags );
        void DrawTexturedJoint( FOdysseyVectorJoint* iJoint
                              , int8*  iScreenPixels
                              , uint32 iScreenWidth
                              , uint32 iScreenHeight
                              , uint32 iScreenBitsPerPixel
                              , int8*  iTexturePixels
                              , uint32 iTextureWidth
                              , uint32 iTextureHeight
                              , uint32 iTextureBitsPerPixel
                              , double iStartU
                              , double iEndU
                              , double iCombinedOpacity
                              , uint64 iDrawingFlags );

        void ParseWayPoints( std::vector<FWayPoint>& iWayPointArray
                           , std::vector<FWayFragment>& iWayFragmentArray
                           , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                           , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                           , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                           , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                           , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                           , bool iSplit );

        eVertexAdditionFlags VertexAdditionPolicy( FWayPoint* iWayPoint, bool iSplit );
        eSegmentAdditionFlags SegmentAdditionPolicy( FWayPoint* iWayPoint0
                                                   , FWayPoint* iWayPoint1
                                                   , bool iSplit );

    protected :
        std::vector<FOdysseyVectorChain> mChainArray;
        std::list<FOdysseyVectorVertex*> mVertexList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSegment*> mInvalidatedSegmentList;
        std::list<FOdysseyVectorVertex*> mSelectedVertexList;
        uint32 mPaintingCode;
        BLPath mBLPath;
        FOdysseyVectorBrush mBrush;

    public:
        FPathParam mPathParam;

    public:
        static const uint64 PICK_HANDLE_VERTEX  = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_VERTEX         = 1 << 2;

        void SetPaintingCode( uint32 iPaintingCode );
        uint32 GetPaintingCode();

        //BLPath& GetBLPath();

        uint32 GetType();
};
