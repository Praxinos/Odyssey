#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"
#include "OdysseyVectorHandlePoint.h"

class FOdysseyVectorSegmentCubic;

class ODYSSEYVECTOR_API FOdysseyVectorVertexCubic : public FOdysseyVectorVertex
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        uint32 GetClass() { return mStaticClass; };

       /**
         * @brief Static function to allocate a new cubic vertex. Note: this is the proper way to allocate a new cubic vertex
         * as we don't use the constructor to set parameters so that this can be derived from an UOBJECT if needed in future
         * devs. Indeed, UOBJECTs have empty constructors.
         * @param iX coordinates on X axis
         * @param iY coordinates on Y axis
         * @param iRadius radius
         */
        static FOdysseyVectorVertexCubic* New( double iX, double iY, double iRadius );

        /**
         * @brief Destructor.
         */
        ~FOdysseyVectorVertexCubic();

        /**
         * @brief Constructor.
         */
        FOdysseyVectorVertexCubic();

        /**
         * @brief Inits the cubic vertex with coordinates and radius
         * @param iX coordinates on X axis
         * @param iY coordinates on Y axis
         * @param iRadius vertex's radius.
         */
        void Init( double iX, double iY, double iRadius );

        /**
         * @brief Get a pointer to the handle (used to set the vertex's radius).
         */
        FOdysseyVectorHandlePoint* GetHandle();

        /**
         * @brief Get a vector perpendicular to the average tangents to the segments at this vertex.
         * @param iNormalize normalize the vector, true or false
         * @return a vector perpendicular to the average tangents to the segments at this vertex.
         */
        ::ULIS::FVec2D GetPerpendicularVector( bool iNormalize );


        ::ULIS::FVec2D GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize );

        /**
         * @brief Build the cached data for this segment. Usually called via a call to path->Update()
         */
        void BuildSegments();

        /**
         * @brief Build the cached data for this segment. Usually called via a call to path->Update()
         * @param iRadius vertex's radius.
         */
        void SmoothSegments( bool iBuildSegments, bool iPreserveHandleLength );

        /**
         * @brief Tell whether or not the angle between the segments connected to this vertex is smooth (low).
         * @return true or false
         */
        virtual bool IsSmooth() override;

        //        virtual void SetRadius( double iRadius, bool iBuildSegments );
        //        void Set( double iX, double iY );
        //        void Set( double iX, double iY, bool iBuildSegments );
        //        void Set( double iX, double iY, double iRadius, bool iBuildSegments );

    protected:
        FOdysseyVectorHandlePoint* mCtrlPoint;

    private :
        static const uint32 mStaticClass = 0x142a5c0; // value is crc32 FOdysseyVectorVertexCubic
};
