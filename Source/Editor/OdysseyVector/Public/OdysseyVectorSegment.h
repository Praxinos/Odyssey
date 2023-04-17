#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorLink.h"

class FOdysseyVectorPath;

class ODYSSEYVECTOR_API FOdysseyVectorSegment : public FOdysseyVectorLink
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

       /**
         * @brief Static function to allocate a new segment. Note: this is the proper way to allocate a new segment as we don't
         * use the constructor to set parameters so that this can be derived from an UOBJECT if needed in future devs. Indeed,
         * UOBJECTs have empty constructors.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         * @return a pointer to the newly created segment
         */
        static FOdysseyVectorSegment* New( FOdysseyVectorPath* iPath
                                         , FOdysseyVectorVertex* iVertex0
                                         , FOdysseyVectorVertex* iVertex1 );

        virtual ~FOdysseyVectorSegment();
        FOdysseyVectorSegment();

       /**
         * @brief Init a segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         */
        void Init( FOdysseyVectorPath* iPath
                 , FOdysseyVectorVertex* iVertex0
                 , FOdysseyVectorVertex* iVertex1 );

       /**
         * @brief Draw the segment
         * @param iRoi the region-of-interest
         */
        virtual void Draw( ::ULIS::FRectD &iRoi );

        virtual void DrawStructure( ::ULIS::FRectD &iRoi, bool iWorld ){};

       /**
         * @brief Test whether or not this segment is close to the coordinates passed as parameter
         * @param iLocalX X-coordinate (in local system)
         * @param iLocalY Y-coordinate (in local system)
         * @param iDistanceTolerance the maximum distance to the segment
         * @param oSmallestDistance the smallest distance that was tested. Valid only if return value equals true.
         * @return true or false
         */
        virtual bool ProximityTest( double iLocalX
                                  , double iLocalY
                                  , double iDistanceTolerance
                                  , double &oSmallestDistance ){ return false; };

       /**
         * @brief Get the list of intersection vertices
         * @return A reference to the list of intersection vertices
         */
        std::list<FOdysseyVectorVertexIntersection*>& GetIntersectionVertexList();

       /**
         * @brief Check whether or not the intersection vertex passed as parameter belongs to this segment
         * @return true or false
         */
        bool HasIntersectionVertex( FOdysseyVectorVertexIntersection& mIntersectionVertex );

       /**
         * @brief Get a pointer to the path this segment belongs to
         * @return a pointer to the path this segment belongs to
         */
        FOdysseyVectorPath* GetPath();

       /**
         * @brief Set the path this segment belongs to. This is called by the path itself when the segment is added.
         * @param a pointer to the path this segment belongs to
         */
        void SetPath( FOdysseyVectorPath* iPath );

       /**
         * @brief Update cached data for this segment.
         */
        virtual void Update();

       /**
         * @brief Mark this segment for later update. This invalidates the path as well.
         */
        void Invalidate();

        bool IsInvalidated();

        virtual ::ULIS::FVec2D GetVectorAtEnd( bool iNormalize );
        virtual ::ULIS::FVec2D GetVectorAtStart( bool iNormalize );

       /**
         * @brief Get the vertex that belongs to this segment. Same as static_cast<FOdysseyVectorVertex*>(GetPoint(iNum))
         * @param iVertexID MUST be 0 or 1.
         * @return a pointer to the requested vertex.
         */
        FOdysseyVectorVertex* GetVertex( uint32 iVertexID );

       /**
         * @brief Remove all intersection vertices and all sections (sub-segments).
         */
        void ClearIntersections();

       /**
         * @brief Get the section that matches parameter t.
         * @param t must be between 0.0 and 1.0.
         * @return a pointer to the requested section.
         */
        FOdysseyVectorSection* GetSection ( double t );

       /**
         * @brief Get the list of sections on this segment.
         * @return a reference to the list of sections.
         */
        std::list<FOdysseyVectorSection*>& GetSectionList();

       /**
         * @brief Add an intersection point. This automatically creates the attached sections.
         * @param iIntersectionVertex the intersection vertex
         */
        void AddIntersection ( FOdysseyVectorVertexIntersection* iIntersectionVertex );

       /**
         * @brief Delete a section. Also frees the section.
         * @param iSection the section to delete.
         */
        void DeleteSection ( FOdysseyVectorSection* iSection );

       /**
         * @brief Add a section.
         * @param iSection the section to add.
         */
        void AddSection ( FOdysseyVectorSection* iSection );

       /**
         * @brief Get the segment's bounding box.
         * @return a reference to the segment's bounding box.
         */
        virtual ::ULIS::FRectD GetBoundingBox( bool iWorld ) { return mBBox; };

       /**
         * @brief Get coordinates on the segment at parameter t.
         * @param t between 0.0 and 1.0.
         * @return coordinates at t.
         */
        virtual ::ULIS::FVec2D GetPointAt( double t );

       /**
         * @brief Get a vector tangent to the segment at parameter t.
         * @param t between 0.0 and 1.0.
         * @return vector at t.
         */
        virtual ::ULIS::FVec2D GetTangentAt( double t );

        FOdysseyVectorVertex*
        GetOtherVertex( FOdysseyVectorVertex* iVertex );

    protected:
        std::list<FOdysseyVectorVertexIntersection*> mIntersectionVertexList;
        std::list<FOdysseyVectorSection*> mSectionList;
        FOdysseyVectorPath* mPath;
        ::ULIS::FRectD mBBox;
        bool mIsInvalidated;

    private:
        static const uint32 mStaticClass = 0x45c58ef1; // value is crc32 FOdysseyVectorSegment
};
