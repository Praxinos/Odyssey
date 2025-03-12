// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorJoint.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorSection.h"

class FOdysseyVectorSegment;
class FOdysseyVectorPath;
class FOdysseyVectorObject;
class FOdysseyVectorVertex;
class FOdysseyVectorHandleSegment;

struct FExplorationPair
{
    FOdysseyVectorSection* returnSection;
    FOdysseyVectorVertex*  departVertex;
    FOdysseyVectorSection* departSection;
    uint32 departVertexIndex;
    double sectionLength;

    FExplorationPair()
    {
        returnSection = nullptr;
        departVertex = nullptr;
        departSection = nullptr;

        sectionLength = 0.0f;
    };

    FExplorationPair( FOdysseyVectorSection* iReturnSection
                    , FOdysseyVectorVertex*  iDepartVertex
                    , uint32                 iDepartVertexIndex
                    , FOdysseyVectorSection* iDepartSection )
    {
        returnSection     = iReturnSection;
        departVertex      = iDepartVertex;
        departVertexIndex = iDepartVertexIndex;
        departSection     = iDepartSection;
        // used for sorting exploration pairs
        sectionLength = returnSection->GetLength() + departSection->GetLength();
    }
};

struct FExplorationWayPoint
{
    FOdysseyVectorVertex* vertex;
    FOdysseyVectorSection* section;
    double sectionT;

    FExplorationWayPoint( FOdysseyVectorVertex* iVertex
                        , FOdysseyVectorSection* iSection
                        , double iSectionT )
    {
        vertex = iVertex;
        section = iSection;
        sectionT = iSectionT;
    }
};

struct ODYSSEYVECTOR_API FSectionLinkInfo
{
    FOdysseyVectorSection* section;
    uint32 sectionVertexIndex;

    // copy constructor
    FSectionLinkInfo( const FSectionLinkInfo& iCycleSectionInfo )
    {
        this->section            = iCycleSectionInfo.section;
        this->sectionVertexIndex = iCycleSectionInfo.sectionVertexIndex;
    }

    FSectionLinkInfo( FOdysseyVectorSection* iSection, uint32 iSectionVertexIndex )
    {
        section = iSection;
        sectionVertexIndex = iSectionVertexIndex;
    }

   ::ULIS::FVec2D& GetVector()
   {
        return section->GetVector( sectionVertexIndex );
   }
};

class ODYSSEYVECTOR_API FOdysseyVectorVertex : public FOdysseyVectorPoint
{
    private:
        static const uint32 mStaticClass =  0xc591efcf; // value is crc32 FOdysseyVectorVertex

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
       /**
         * @brief function to allocate a new vertex.
         * @param iX coordinates on X axis
         * @param iY coordinates on Y axis
         * @param iRadius radius
         */
        FOdysseyVectorVertex ( double iX, double iY, double iRadius );
        ~FOdysseyVectorVertex();

        static void ArrayToSegmentArray( const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oSegmentArray );

        /**
         * @brief Add a section to the list of section connected to this vertex
         * @param iSection the section to add
         */
        void AddSection( FOdysseyVectorSection* iSection
                       , uint32 iSectionVertexIndex );

        /**
         * @brief Add a segment to the list of segments connected to this vertex
         * @param iSegment the segment
         */
        virtual void AddSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get the average vector of all straight vectors leaving from this vertex for each segment connected to this vertex.
         *        A "straight vector" means the straight vector between segment endpoints.
         * @param iNormalize normalize the result.
         * @return the average straight vector.
         */
        ::ULIS::FVec2D GetAverageStraightVectorOnSegment( bool iNormalize );

        /**
         * @brief Get the average vector of all vectors leaving from this vertex for each segment connected to this vertex.
         * @param iNormalize normalize the result.
         * @return the average vector.
         */
        ::ULIS::FVec2D GetAverageVectorOnSegment( bool iNormalize );

        /**
         * @brief Get the average vector of all vectors leaving from this vertex for each handle connected to this vertex.
         * @param iNormalize normalize the result.
         * @return the average vector.
         */
        ::ULIS::FVec2D GetAverageVectorOnSegmentHandle( bool iNormalize );

        /**
         * @brief Get the bounding volume form by this vertex and its segments.
         * @return the bounding volume form by this vertex and its segments.
         */
        ::ULIS::FRectD GetBoundingBox( bool iWorld );

        /**
         * @brief Get a pointer to the first segment stored in the list. Equals to a call to GetSegmentList().front().
         * @return first last segment stored in the list. If there is only 1 segment, it will return the same
         * result as GetLastSegment().
         */
        FOdysseyVectorSegment* GetFirstSegment();

        /**
         * @brief Get a pointer to the last segment stored in the list. Equals to a call to GetSegmentList().back().
         * @return the last segment stored in the list. If there is only 1 segment, it will return the same
         * result as GetFirstSegment().
         */
        FOdysseyVectorSegment* GetLastSegment();

        /**
         * @brief Get a pointer to a section connected to this vertex AND located on the same segment
         * as the section passed as an argument AND different from that section.
         * @param iSection a pointer to a section.
         * @return a pointer to the other section.
         */
        FOdysseyVectorSection* GetOtherSection( FOdysseyVectorSection* iSection, bool iSameSegment );

        /**
         * @brief Get a pointer to a segment connected to this vertex AND different from that segment.
         * @param iSegment a pointer to a segment.
         * @return a pointer to the other segment.
         */
        FOdysseyVectorSegment* GetOtherSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get a pointer to the path this vertex belongs to.
         * @return a pointer to the path this vertex belongs to.
         */
        FOdysseyVectorPath* GetOwnerAsPath();

        /**
         * @brief Get the point's radius
         * @return the point's radius
         */
        double GetRadius();

        /**
         * @brief Get a pointer to the section connecting this vertex and another vertex passed as argument.
         * @param iSegment
         * @return
         */
        FOdysseyVectorSection* GetSection( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get a pointer to the segment connecting this vertex and another vertex passed as argument.
         * @param iOtherVertex a pointer to the other vertex.
         * @return a pointer to the segment connecting this vertex and iOtherVertex.
         */
        virtual FOdysseyVectorSegment* GetSegment( FOdysseyVectorVertex* iOtherVertex );

        /**
         * @brief Get the number of sections connected to this vertex. Equals to a call to "GetSectionList().size()".
         * @return the number of sections connected to this vertex.
         */
        virtual uint32 GetSectionCount( FOdysseyVectorObject* iOwner );

        /**
         * @brief Get a reference to the list of sections connected to this vertex.
         * @return a reference to the list of sections connected to this vertex.
         */
        std::list<FSectionLinkInfo>& GetSectionLinkInfoList();

        /**
         * @brief Get the number of segments connected to this vertex. Equals to a call to "GetSegmentList().size()".
         * @return the number of segments connected to this vertex.
         */
        uint32 GetSegmentCount();

        /**
         * @brief Get a reference to the list of segments connected to this vertex.
         * @return a reference to the list of segments connected to this vertex.
         */
        std::list<FOdysseyVectorSegment*>& GetSegmentList();

        /**
         * @brief Get the position of the vertex on the segment passed as parameter.
         * @param iSegment the section the vertex lies on.
         * @return 0 or 1.
         */
        virtual uint32 GetIndex( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get the point's ID. This is for the programmer to use e.g as an index of an array.
         * it should no be considered consistent through the whole execution of the program.
         */
        uint32 GetID();

        /**
         * @brief Get the position of the vertex on the section passed as parameter.
         * @param iSection the section the vertex lies on.
         * @return 0 or 1.
         */
        virtual uint32 GetIndex( FOdysseyVectorSection* iSection );

        virtual ::ULIS::FVec2D GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize );

        /**
         * @brief Get the next section to go through. Used by OdysseyGroupPaint for finding cycles.
         *        The next section is determined by its orientation (right or left).
         * @param iLastSection the section we are coming from.
         * @param iVertexIndex vertex index in the last section (0 or 1). This is necessary to handle looping sections
         * @param iOrientation the orientation for the desired next section (1.0f or -1.0f).
         * @return a pointer to the next section to go trough.
         */
        FSectionLinkInfo* GetCycleNextSection( FOdysseyVectorSection* iLastSection
                                             , uint32 iLastSectionVertexIndex
                                             , double iOrientation );

        FSectionLinkInfo* GetCycleNextSection( FSectionLinkInfo* iLastSectionLinkInfo
                                             , double iOrientation );
        FSectionLinkInfo* GetCycleNextSection( FSectionLinkInfo* iLastSectionLinkInfo
                                             , const std::vector<FSectionLinkInfo*> iCandidateSectionArray
                                             , double iOrientation );

        FSectionLinkInfo* GetSectionLinkInfo( FOdysseyVectorSection* iSection
                                            , uint32 iSectionVertexIndex );

        /**
         * @brief Is vertex selected ?
         * @return true if selected, false otherwise
         */
        bool IsSelected();

        /**
         * @brief Is vertex "handle aligned" ?
         * @return true if handles must be aligned, false otherwise
         */
        bool IsHandleAligned();

        /**
         * @brief Tell whether or not the angle between the segments connected to this vertex is smooth (low).
         * @return true or false
         */
        bool IsSmooth();

        /**
         * @brief Tell whether or not the VISITED flags is set.
         * @return true or false
         */
        bool IsVisited();

        bool HasSegment( FOdysseyVectorSegment* iSegment );

        ::ULIS::FVec2D GetNearestSegmentIntersectionCoords();
        double GetNearestSegmentT();
        double GetDistanceToNearestSegment();
        FOdysseyVectorSegment* GetNearestSegment();
        void SetNearestSegment( FOdysseyVectorSegment* iNearestSegment
                              , double iDistanceToNearestSegment
                              , double iNearestSegmentT
                              , const ::ULIS::FVec2D& iNearestSegmentIntersectionCoords );
        void ResetNearestSegment();

        FOdysseyVectorVertex* GetNearestVertex();
        void SetNearestVertex( FOdysseyVectorVertex* iNearestVertex, double iDistanceToNearestVertex );
        void ResetNearestVertex();
        double GetDistanceToNearestVertex();

        /**
         * @brief Build exploration pairs. an exploration pair is composed of an entry section,
         *        a leaving section, and a vertex in between.
         * @param oExplorationPairsArray the output pairs.
         */
        virtual void BuildExplorationPairs( std::vector<FExplorationPair>& oExplorationPairsArray );


        /**
         * @brief Remove a section from the list of section connected to this vertex
         * @param iSection the section to remove
         */
        void RemoveSection( FOdysseyVectorSection* iSection
                          , uint32 iSectionVertexIndex );

        /**
         * @brief Remove a segment from the list of segments connected to this vertex.
         * @param iSegment the segment to remove.
         */
        void RemoveSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Set the vertex as SELECTED.
         * @param iSelected
         */
        void SetSelected( bool iSelected );

        /**
         * @brief Set the vertex as HANDLE_ALIGNED.
         * @param iHandleAligned
         */
        void SetHandleAligned( bool iHandleAligned );
        void SetHandleAligned( bool iHandleAligned, bool iAlignNow );

        /**
         * @brief Set the vertex as VISITED. Internal use only for the GroupPaint class.
         * @param iVisited;
         */
        void SetVisited( bool iVisited );

        void SetRadius( double iRadius );

        static bool GetMinMaxFromList( std::list<FOdysseyVectorVertex*>& iVertexList
                                     , bool iWithHandles
                                     , double& oXMin
                                     , double& oYMin
                                     , double& oXMax
                                     , double& oYMax );

        ::ULIS::FVec2D GetWorldCoords();

        void AlterRadius( FOdysseyVectorVertex* iInitiatorVertex
                        , FOdysseyVectorSegment* iFromSegment
                        , double iDeltaRadius
                        , bool iAlterAllAlong );
        FOdysseyVectorHandleSegment* GetOtherSegmentHandle( FOdysseyVectorSegment* iSegment );
        void SetChained( bool iChained );
        bool IsChained();
        void MakeJoint( FOdysseyVectorSegment* iPrevSegment
                      , FOdysseyVectorSegment* iNextSegment );
        void DrawJoint( BLContext* iBLContext
                      , FOdysseyVectorEngine* iVectorEngine
                      , double iCombinedOpacity
                      , uint64 iDrawingFlags );
        FOdysseyVectorJoint& GetJoint();
        double GetJointLength();
        uint32 GetFlags();
        void GetHandlePosition( ::ULIS::FVec2D iHandlePosition[2] );
        bool HasErasedSectionsOnly();
        void SetOwner( FOdysseyVectorObject* iOwner );
        FOdysseyVectorObject* GetOwner();
        bool HasLengthySection();
        void GetCandidateSections( FOdysseyVectorSection* iLastSection
                                 , uint32 iLastSectionVertexIndex
                                 , std::vector<FOdysseyVectorVertex*>& oPartnerVertexArray
                                 , std::vector<FSectionLinkInfo*>& oCandidateSectionArray );
        void SetLocked( bool iIsLocked );
        bool IsLocked();

        /**
         * @brief Set the point's ID. This is for the programmer to use e.g as an index of an array.
         * it should no be considered consistent through the whole execution of the program.
         * @param iID the desired point's ID
         */
        void SetID( uint32 iID );

        void GetSurroundingSections( std::vector<FOdysseyVectorVertex*>& oPartnerVertexArray
                                   , std::vector<FSectionLinkInfo*>& oSurroundingSectionArray );
        void GetSectionLinkInfo( std::vector<FSectionLinkInfo>& oSectionLinkInfoArray );
        FSectionLinkInfo* GetOtherSectionLinkInfo( FSectionLinkInfo* iLastSectionLinkInfo );
        FOdysseyVectorSection* GetFirstSection();
        FSectionLinkInfo* GetSectionLinkInfo( FOdysseyVectorSegment* iSegment );

        bool IsInvalidated();
        void Invalidate();
        void Update( FOdysseyVectorSegment* iPrevSegment, FOdysseyVectorSegment* iSegment, uint32 iUpdateFlags );

        // a version that does not invalidate the owner
        void SetCoordsSilent( ::ULIS::FVec2D& iCoords );
        uint32 GetErasedSectionCount( FOdysseyVectorObject* iOwner );
        void Print();

    protected:
        /**
         * @brief Set the vertex coordinate in the X-Axis and Y-axis. It invalidates the attached segments.
         * @param iX the desired coordinate in the X-axis.
         * @param iY the desired coordinate in the Y-axis.
         */
        virtual void SetCoords( double iX, double iY ) override;

        /**
         * @brief Mark all connected segments for update.
         */
        void InvalidateSegments();

    protected:
        FOdysseyVectorJoint mJoint;
        //eJointType mJointType;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FSectionLinkInfo> mSectionLinkInfoList;
        std::list<FOdysseyVectorVertex*> mPartnerList;
        FOdysseyVectorObject* mOwner;
        uint32 mFlags;
        double mRadius;
        uint32 mID;

        double mDistanceToNearestSegment;
        double mNearestSegmentT;
        ::ULIS::FVec2D mNearestSegmentIntersectionCoords;
        FOdysseyVectorSegment* mNearestSegment;

        double mDistanceToNearestVertex;
        FOdysseyVectorVertex* mNearestVertex;


    public :
        static const uint32 CHAINED        = ( 1 << 1 );
        static const uint32 VISITED        = ( 1 << 2 );
        static const uint32 SELECTED       = ( 1 << 3 );
        static const uint32 HANDLE_ALIGNED = ( 1 << 4 );
        static const uint32 LOCKED         = ( 1 << 5 );
        static const uint32 INVALIDATED    = ( 1 << 6 );
};
