#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

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

USTRUCT()
struct FPathParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Transform")
    eJointType JointType;

    UPROPERTY(EditAnywhere,Category="General")
    bool Filled;
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
        static void SharpSegments( FOdysseyVectorVertex* iVertex, bool iBuildSegments, bool iPreserveHandleLength );
        static void SmoothSegments( FOdysseyVectorVertex* iVertex, bool iBuildSegments, bool iPreserveHandleLength );
        static void SmoothSegments( FOdysseyVectorVertex* iVertex
                                  , ::ULIS::FVec2D iPerpendicularVector
                                  , bool iBuildSegments
                                  , bool iPreserveHandleLength );

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
         * @brief Mark as filled or not. The closed path is filled with the background color.
         * @param iIsFilled true if filled, false otherwise.
         */
        void SetFilled( bool iIsFilled );

       /**
         * @brief Is it a closed path ?
         * @return true if it is closed, false otherwise.
         */
        bool IsLoop();

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

       /**
         * @brief Set the joint type.
         * @param mJointType the joint type.
         */
        void SetJointType( eJointType mJointType );

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

    protected:
        virtual void UpdateShape( uint32 iUpdateFlags ) override;
        virtual void DrawShape( uint64 iFlags ) override;
        virtual bool PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags ) override;
        virtual FOdysseyVectorObject* CopyShape() override;

        void DrawJoint( FOdysseyVectorVertex* iVertex, uint64 iFlags );
        void UpdateBBox();
        void Fill();

    protected :
        std::list<FOdysseyVectorVertex*> mVertexList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSegment*> mInvalidatedSegmentList;
        /*std::list<FOdysseyVectorCycle*> mInvalidatedLoopList;*/
        std::list<FOdysseyVectorVertex*> mSelectedVertexList;
        uint32 mPaintingCode;
        BLPath mBLPath;

    public:
        FPathParam mPathParam;

    public:
        static const uint64 PICK_HANDLE_POINT   = 1;
        static const uint64 PICK_HANDLE_SEGMENT = 1 << 1;
        static const uint64 PICK_POINT          = 1 << 2;



        /*void InvalidateLoop( FOdysseyVectorCycle* iLoop );*/

        void SetPaintingCode( uint32 iPaintingCode );
        uint32 GetPaintingCode();
        BLPath& GetBLPath();
        
        using FOdysseyVectorObject::DrawStructure;

        bool PickPoint( double iWorldX
                      , double iWorldY
                      , double iSelectionRadius
                      , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                      , uint64 iSelectionFlags );

        void Merge( FOdysseyVectorPath* iPath
                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );
        void Merge( FOdysseyVectorPath* iMergedPath
                  , std::vector<FOdysseyVectorVertex*>& iVertexLookup
                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );
        void Mirror( bool iMirrorX, bool iMirrorY );
        void Cut( const ::ULIS::FVec2D& linePoint0
                , const ::ULIS::FVec2D& linePoint1
                , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray
                , std::vector<FOdysseyVectorSegment*>& oOldSegmentArray );
        void SwitchSpace( FOdysseyVectorObject& iObject );
        bool Erase( const ::ULIS::FRectD &iRoi
                  , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                  , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray );
        virtual void PickVertex( std::vector<FOdysseyVectorVertex*>& oPickedVertexArray );
        virtual void DrawStructure( FColor& iStrokeColor, double iStrokeWidth, bool iWorld );
        uint32 GetType();
};
