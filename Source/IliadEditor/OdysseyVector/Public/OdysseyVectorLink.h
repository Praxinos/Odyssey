// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class ODYSSEYVECTOR_API FOdysseyVectorLink
{
    private:
        static const uint32 mStaticClass =  0x58092e87; // value is crc32 FOdysseyVectorLink

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        //virtual bool HasBaseClass( uint32 iBaseClassID ) override;

       /**
         * @brief Destructor
         */
        virtual ~FOdysseyVectorLink();

       /**
         * @brief Constructor
         * @param iPoint0 end point at the beginning of this link.
         * @param iPoint1 end point at the start of this link.
         */
         FOdysseyVectorLink( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 );

       /**
         * @brief Get a vector going from the point passed as parameter to the other point.
         * @param The point the vector starts from.
         * @param iNormalize normalize the vector. True or false.
         * @return A vector.
         */
        ::ULIS::FVec2D GetVector( FOdysseyVectorPoint* iPoint0, bool iNormalize );

       /**
         * @brief Get a vector going from point0 to point1.
         * @param iNormalize normalize the vector. True or false.
         * @return A vector.
         */
        ::ULIS::FVec2D GetVector( bool iNormalize );

       /**
         * @brief Get one of the endpoints.
         * @param iPointNum the index of the endpoint (0 or 1).
         * @return A pointer to the endpoint.
         */
        FOdysseyVectorPoint* GetPoint( int iPointNum );

       /**
         * @brief Set endpoint 0.
         * @param a pointer to the endpoint.
         */
        void SetPoint0( FOdysseyVectorPoint* iPoint );

       /**
         * @brief Set endpoint 1.
         * @param a pointer to the endpoint.
         */
        void SetPoint1( FOdysseyVectorPoint* iPoint );

       /**
         * @brief Get the interpolated coordinates of a point at parameter t.
         * @param iT the parameter between 0.0 and 1.0.
         * @return The coordinates of a point at parameter t.
         */
        virtual ::ULIS::FVec2D GetPointAt( double iT );

       /**
         * @brief Get the distance between endpoints.
         * @return the distance between endpoints.
         */
        double GetStraightDistance();

    protected:
        FOdysseyVectorPoint* mPoint[2];
};
