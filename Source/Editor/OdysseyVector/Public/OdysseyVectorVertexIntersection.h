#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"

struct FIntersection
{
    double t;
};

struct FExplorationPair
{
    FOdysseyVectorSection* returnSection;
    FOdysseyVectorSection* departSection;

    FExplorationPair()
    {
        returnSection = nullptr;
        departSection = nullptr;
    };

    FExplorationPair( FOdysseyVectorSection* iReturnSection, FOdysseyVectorSection* iDepartSection )
    {
        returnSection = iReturnSection;
        departSection = iDepartSection;
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorVertexIntersection : public FOdysseyVectorVertex
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };

        /**
         * @brief Destructor.
         */
        ~FOdysseyVectorVertexIntersection();

        /**
         * @brief Constructor.
         */
        FOdysseyVectorVertexIntersection( double iX, double iY, bool iSelfIntersect );

        double GetT( FOdysseyVectorSection* iSection );

        void MapSection( FOdysseyVectorSection* iSection, double t );

        void BuildExplorationPairs();
        std::vector<FExplorationPair>& GetExplorationPairs();

        /**
         * @brief Get a pointer to the next section to explore in cycle depending on the last visited section.
         * @param iLastSection the last visited section.
         * @param iOrientation ignored.
         * @return a pointer to the next section to explore in cycle.
         */
        FOdysseyVectorSection* GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation );

    protected:
        uint64 mIntersectionID;
        // map for intersection positions
        std::multimap<FOdysseyVectorSection*, FIntersection> mTMap; // non unique mapping (segment can intersect itself)
        std::vector<FExplorationPair> mExplorationPairs;
        bool mSelfIntersect;

    private:
        static const uint32 mStaticClass =  0x29459195; // value is crc32 FOdysseyVectorVertexIntersection
};
