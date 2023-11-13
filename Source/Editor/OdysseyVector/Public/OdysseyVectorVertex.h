#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorJoint.h"
#include "OdysseyVectorPoint.h"

class FOdysseyVectorSegment;
class FOdysseyVectorSection;
class FOdysseyVectorPath;
class FOdysseyVectorVertex;
class FOdysseyVectorHandleSegment;

struct FExplorationPair
{
    FOdysseyVectorSection* returnSection;
    FOdysseyVectorVertex*  departVertex;
    FOdysseyVectorSection* departSection;

    FExplorationPair()
    {
        returnSection = nullptr;
        departVertex = nullptr;
        departSection = nullptr;
    };

    FExplorationPair( FOdysseyVectorSection* iReturnSection
                    , FOdysseyVectorVertex*  iDepartVertex
                    , FOdysseyVectorSection* iDepartSection )
    {
        returnSection = iReturnSection;
        departVertex = iDepartVertex;
        departSection = iDepartSection;
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
        FOdysseyVectorVertex ( FOdysseyVectorPath* iPath, double iX, double iY, double iRadius );
        ~FOdysseyVectorVertex();

        static void ArrayToSegmentArray( const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oSegmentArray );

        /**
         * @brief Add a section to the list of section connected to this vertex
         * @param iSection the section to add
         */
        void AddSection( FOdysseyVectorSection* iSection );

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
        FOdysseyVectorPath* GetPath();

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
        virtual uint32 GetSectionCount();

        /**
         * @brief Get a reference to the list of sections connected to this vertex.
         * @return a reference to the list of sections connected to this vertex.
         */
        std::list<FOdysseyVectorSection*>& GetSectionList();

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
         * @brief Get the position of the vertex on the segment passed as parameter, in a range from 0.0 to 1.0.
         * @param iSegment the section the vertex lies on.
         * @return a range from 0.0 to 1.0.
         */
        virtual double GetT( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get the position of the vertex on the section passed as parameter, in a range from 0.0 to 1.0.
         * @param iSection the section the vertex lies on.
         * @return a range from 0.0 to 1.0.
         */
        virtual double GetT( FOdysseyVectorSection* iSection );

        virtual ::ULIS::FVec2D GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize );

        /**
         * @brief Get the next section to go through. Used by OdysseyGroupPaint for finding cycles.
         *        The next section is determined by its orientation (right or left).
         * @param iLastSection the section we are coming from.
         * @param iOrientation the orientation for the desired next section (1.0f or -1.0f).
         * @return a pointer to the next section to go trough.
         */
        virtual FOdysseyVectorSection* GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation );

        /**
         * @brief Mark all connected segments for update.
         */
        void InvalidateSegments();

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

        double GetNearestSegmentT();
        double GetDistanceToNearestSegment();
        FOdysseyVectorSegment* GetNearestSegment();
        void SetNearestSegment( FOdysseyVectorSegment* iNearestSegment
                              , double iDistanceToNearestSegment
                              , double iNearestSegmentT );
        void SetNearestVertex( FOdysseyVectorVertex* iNearestVertex );
        FOdysseyVectorVertex* GetNearestVertex();



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
        void RemoveSection( FOdysseyVectorSection* iSection );

        /**
         * @brief Remove a segment from the list of segments connected to this vertex.
         * @param iSegment the segment to remove.
         */
        void RemoveSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Set the path this vertex belongs to.
         * @param iPath the path this vertex belongs to;
         */
        void SetPath( FOdysseyVectorPath* iPath );

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

        /**
         * @brief Set the vertex as VISITED. Internal use only for the GroupPaint class.
         * @param iVisited;
         */
        void SetVisited( bool iVisited );

        static bool GetMinMaxFromList( std::list<FOdysseyVectorVertex*>& iVertexList
                                     , double& oXMin
                                     , double& oYMin
                                     , double& oXMax
                                     , double& oYMax );

        ::ULIS::FVec2D GetWorldCoords();
        void AlignHandles( FOdysseyVectorHandleSegment* iHandle );
        void AlignHandles();

        void AlterRadius( FOdysseyVectorVertex* iInitiatorVertex
                        , FOdysseyVectorSegment* iFromSegment
                        , double iDeltaRadius
                        , bool iAlterAllAlong );
        FOdysseyVectorHandleSegment* GetOtherSegmentHandle( FOdysseyVectorSegment* iSegment );
        void SetChained( bool iChained );
        bool IsChained();
        void MakeJoint( FOdysseyVectorSegment* iPreviousSegment );
        void DrawJoint( BLContext* iBLContext, uint64 iDrawingFlags );
        FOdysseyVectorJoint& GetJoint();
        double GetJointLength();
        uint32 GetFlags();
        void GetHandlePosition( ::ULIS::FVec2D iHandlePosition[2] );

    protected:
        /**
         * @brief Set the vertex coordinate in the X-Axis and Y-axis. It invalidates the attached segments.
         * @param iX the desired coordinate in the X-axis.
         * @param iY the desired coordinate in the Y-axis.
         */
        virtual void SetCoords( double iX, double iY, double iRadius ) override;

    protected:
        FOdysseyVectorJoint mJoint;
        //eJointType mJointType;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSection*> mSectionList;
        FOdysseyVectorPath* mPath;
        uint32 mFlags;

        double mDistanceToNearestSegment;
        double mNearestSegmentT;
        FOdysseyVectorSegment* mNearestSegment;
        FOdysseyVectorVertex* mNearestVertex;

    public :
        static const uint32 CHAINED        = ( 1 << 1 );
        static const uint32 VISITED        = ( 1 << 2 );
        static const uint32 SELECTED       = ( 1 << 3 );
        static const uint32 HANDLE_ALIGNED = ( 1 << 4 );
};
