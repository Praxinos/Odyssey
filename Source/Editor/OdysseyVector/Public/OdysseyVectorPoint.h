#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorSegment;
class UOdysseyVectorCycle;
class FOdysseyVectorSection;

class ODYSSEYVECTOR_API FOdysseyVectorPoint
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

       /**
         * @brief Static function to allocate a new point. Note: this is the proper way to allocate a new point as we don't
         * use the constructor to set parameters so that this can be derived from an UOBJECT if needed in future devs. Indeed,
         * UOBJECTs have empty constructors.
         * @param iX coordinates on X axis
         * @param iY coordinates on Y axis
         * @param iRadius radius
         */
        static FOdysseyVectorPoint* New( double iX, double iY, double iRadius );

        virtual ~FOdysseyVectorPoint();
        FOdysseyVectorPoint();

        /**
         * @brief Inits the point with coordinates and radius
         * @param iX coordinates on X axis
         * @param iY coordinates on Y axis
         * @param iRadius radius
         */
        void Init( double iX, double iY, double iRadius );

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
         * @brief Get the point's radius
         * @return the point's radius
         */
        double GetRadius();

        /**
         * @brief Set the point's absolute coordinate on X axis
         * @param iX the desired coordinate on X axis
         */
        virtual void SetX( double iX );

        /**
         * @brief Set the point's absolute coordinate on Y axis
         * @param iY the desired coordinate on Y axis
         */
        virtual void SetY( double iY );

        /**
         * @brief Set the point's absolute coordinates on both X and Y axis
         * @param iX the desired coordinate on X axis
         * @param iY the desired coordinate on Y axis
         */
        virtual void Set( double iX, double iY );

        /**
         * @brief Set the point's absolute radius
         * @param iRadius the desired point's radius
         */
        virtual void SetRadius( double iRadius );

        /**
         * @brief Set the point's ID. This is for the programmer to use e.g as an index of an array.
         * it should no be considered consistent through the whole execution of the program.
         * @param iID the desired point's ID
         */
        void SetID( uint32 iID );

        /**
         * @brief Get the point's ID. This is for the programmer to use e.g as an index of an array.
         * it should no be considered consistent through the whole execution of the program.
         */
        uint32 GetID();

    protected:
        uint32 mID;
        ::ULIS::FVec2D mCoords;
        double mRadius;

    private:
        static const uint32 mStaticClass = 0xead829a; // value is crc32 FOdysseyVectorPoint
};
