#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class FOdysseyVectorSegment;
class UOdysseyVectorLoop;
class FOdysseyVectorSection;
class UOdysseyVectorPath;

class ODYSSEYVECTOR_API FOdysseyVectorVertex : public FOdysseyVectorPoint
{
    public:
       /**
         * @brief Static function to allocate a new vertex. Note: this is the proper way to allocate a new vertex as we don't
         * use the constructor to set parameters so that this can be derived from an UOBJECT if needed in future devs. Indeed,
         * UOBJECTs have empty constructors.
         * @param iX coordinates on X axis
         * @param iY coordinates on Y axis
         * @param iRadius radius
         */
        static FOdysseyVectorVertex* New( double iX, double iY, double iRadius );

        ~FOdysseyVectorVertex();
        FOdysseyVectorVertex();

        /**
         * @brief Add a segment to the list of segments connected to this vertex
         * @param iSegment the segment
         */
        virtual void AddSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Add a section to the list of section connected to this vertex
         * @param iSection the section to add
         */
        void AddSection( FOdysseyVectorSection* iSection );

        /**
         * @brief Remove a segment from the list of segments connected to this vertex.
         * @param iSegment the segment to remove.
         */
        void RemoveSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Remove a section from the list of section connected to this vertex
         * @param iSection the section to remove
         */
        void RemoveSection( FOdysseyVectorSection* iSection );

        //::ULIS::FRectD GetRectangle();

        //////// setters

        /**
         * @brief Set the vertex coordinate in the X-axis. It invalidates the attached segments.
         * @param iX the desired coordinate in the X-axis.
         */
        virtual void SetX( double iX );

        /**
         * @brief Set the vertex coordinate in the Y-axis. It invalidates the attached segments.
         * @param iY the desired coordinate in the Y-axis.
         */
        virtual void SetY( double iY );

        /**
         * @brief Set the vertex coordinate in the X-Axis and Y-axis. It invalidates the attached segments.
         * @param iX the desired coordinate in the X-axis.
         * @param iY the desired coordinate in the Y-axis.
         */
        virtual void Set( double iX, double iY );

        /**
         * @brief Set the vertex's absolute radius. It invalidates the attached segments.
         * @param iRadius the desired vertex's radius.
         */
        virtual void SetRadius(double iRadius);

        /**
         * @brief Set the vertex as VISITED. Internal use only for the GroupPaint class.
         * @param iVisited;
         */
        void SetVisited( bool iVisited );

        /**
         * @brief Set the path this vertex belongs to.
         * @param iPath the path this vertex belongs to;
         */
        void SetPath( UOdysseyVectorPath* iPath );

         //////// getters

        /**
         * @brief Get a pointer to the last segment stored in the list. Equals to a call to GetSegmentList().back().
         * @return the last segment stored in the list. If there is only 1 segment, it will return the same
         * result as GetFirstSegment().
         */
        FOdysseyVectorSegment* GetLastSegment();

        /**
         * @brief Get a pointer to the first segment stored in the list. Equals to a call to GetSegmentList().front().
         * @return first last segment stored in the list. If there is only 1 segment, it will return the same
         * result as GetLastSegment().
         */
        FOdysseyVectorSegment* GetFirstSegment();

        /**
         * @brief Get a pointer to the segment connecting this vertex and another vertex passed as argument.
         * @param iOtherVertex a pointer to the other vertex.
         * @return a pointer to the segment connecting this vertex and iOtherVertex.
         */
        virtual FOdysseyVectorSegment* GetSegment( FOdysseyVectorVertex* iOtherVertex );

        /**
         * @brief Get a pointer to the section connecting this vertex and another vertex passed as argument.
         * @param iOtherVertex a pointer to the other vertex.
         * @return a pointer to the section connecting this vertex and iOtherVertex.
         */
        FOdysseyVectorSection* GetSection( FOdysseyVectorVertex* iOtherVertex );

        /**
         * @brief Get a pointer to a section connected to this vertex AND located on the same segment
         * as the section passed as an argument AND different from that section. 
         * @param iSection a pointer to a section.
         * @return a pointer to the other section.
         */
        FOdysseyVectorSection* GetOtherSection( FOdysseyVectorSection* iSection );

        /**
         * @brief Get a pointer to a segment connected to this vertex AND different from that segment. 
         * @param iSegment a pointer to a segment.
         * @return a pointer to the other segment.
         */
        FOdysseyVectorSegment* GetOtherSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get the coordinates of this vertex depending on the segment passed as parameter.
         * while this value would be the same for regular vertices, it may differ for intersection vertices in the case
         * where there is some "intersection tolerance", i.e segments almost intersect.
         * @param iSegment segment the vertex lies on.
         * @return the coordinates of the vertex on that segment.
         */
        virtual ::ULIS::FVec2D GetPosition( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get the number of segments connected to this vertex. Equals to a call to "GetSegmentList().size()".
         * @return the number of segments connected to this vertex.
         */
        uint32 GetSegmentCount();

        /**
         * @brief Get the number of sections connected to this vertex. Equals to a call to "GetSectionList().size()".
         * @return the number of sections connected to this vertex.
         */
        uint32 GetSectionCount();

        /**
         * @brief Get a reference to the list of segments connected to this vertex.
         * @return a reference to the list of segments connected to this vertex.
         */
        std::list<FOdysseyVectorSegment*>& GetSegmentList();

        /**
         * @brief Get a reference to the list of sections connected to this vertex.
         * @return a reference to the list of sections connected to this vertex.
         */
        std::list<FOdysseyVectorSection*>& GetSectionList();

        /**
         * @brief Get the position of the vertex on the segment passed as parameter, in a range from 0.0 to 1.0.
         * @param iSegment the segment the vertex lies on.
         * @return a range from 0.0 to 1.0.
         */
        virtual double GetT( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get a pointer to the path this vertex belongs to.
         * @return a pointer to the path this vertex belongs to.
         */
        UOdysseyVectorPath* GetPath();


        virtual ::ULIS::FVec2D GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize );

        virtual ::ULIS::FVec2D& GetCoordsOnSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Get a pointer to the next section to explore in cycle depending on the last visited section.
         * @param iLastSection the last visited section.
         * @param iOrientation ignored.
         * @return a pointer to the next section to explore in cycle.
         */
        FOdysseyVectorSection* GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation );

        /**
         * @brief Mark all connected segments for update.
         */
        void InvalidateSegments();

        /**
         * @brief Tell whether or not the segment passed as parameter is connected to this vertex.
         * @param iSegment the tested segment.
         * @return true or false
         */
        bool HasSegment( FOdysseyVectorSegment* iSegment );

        /**
         * @brief Tell whether or not the angle between the segments connected to this vertex is smooth (low).
         * @return true or false
         */
        virtual bool IsSmooth() { return false; };

        /**
         * @brief Tell whether or not the VISITED flags is set.
         * @return true or false
         */
        bool IsVisited();

    protected:
        std::list<FOdysseyVectorSegment*> mSegmentList;
        std::list<FOdysseyVectorSection*> mSectionList;
        UOdysseyVectorPath* mPath;
        uint32 mFlags;

    private :
        static const uint32 VISITED = (1 << 2);
};
