// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorVertex;

class ODYSSEYVECTOR_API FOdysseyVectorPoint
{
    private:
        static const uint32 mStaticClass = 0xead829a; // value is crc32 FOdysseyVectorPoint

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        static void ArrayToVertexArray( std::vector<FOdysseyVectorPoint*>& iPointArray
                                      , std::vector<FOdysseyVectorVertex*>& oVertexArray );

        /**
         * @brief Destructor.
         */
        virtual ~FOdysseyVectorPoint();

        /**
         * @brief Default constructor. all members are zeroed.
         */
        FOdysseyVectorPoint();

        /**
         * @brief Constructor
         * @param iX coordinate on X axis.
         * @param iY coordinate on Y axis.
         * @param iRadius radius.
         */
        FOdysseyVectorPoint( double iX, double iY );

        ::ULIS::FVec2D& GetCoords();

        /**
         * @brief Get the point's coordinate on X axis
         * @return the point's coordinates on X axis
         */
        double GetX();

        /**
         * @brief Get the point's coordinate on Y axis
         * @return the point's coordinate on Y axis
         */
        double GetY();

        /**
         * @brief Set the point's coordinate on X & Y axis
         * @param iCoords the desired coordinates of type ::ULIS::FVec2D
         */
        void Set( const ::ULIS::FVec2D& iCoords );

        /**
         * @brief Set the point's coordinate on X & Y axis
         * @param iX the desired coordinate on X axis
         * @param iY the desired coordinate on Y axis
         */
        void Set( double iX, double iY );

        /**
         * @brief Set the point's coordinate on X axis
         * @param iX the desired coordinate on X axis
         */
        void SetX( double iX );

        /**
         * @brief Set the point's coordinate on Y axis
         * @param iY the desired coordinate on Y axis
         */
        void SetY( double iY );

    protected:
        /**
         * @brief Set the point's coordinates. This is the function that all other
                  positionning functions are based on and should be derived if any shape
                  invalidation is needed after positionning.
         * @param iX the desired coordinate on X axis.
         * @param iY the desired coordinate on Y axis.
         */
        virtual void SetCoords( double iX, double iY );

    protected:
        // DO NOT add other member variables. this class must remain small in footprint
        ::ULIS::FVec2D mCoords;
};
