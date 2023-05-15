#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"

class FOdysseyVectorSegment;
class FOdysseyVectorIntersection;

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
        FOdysseyVectorVertexIntersection( FOdysseyVectorPath* iPath, bool iSelfIntersects, double iX, double iY, double iT );

        /**
         * @brief Get a pointer to the next section to explore in cycle depending on the last visited section.
         * @param iLastSection the last visited section.
         * @param iOrientation ignored.
         * @return a pointer to the next section to explore in cycle.
         */
        virtual FOdysseyVectorSection* GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation ) override;

        virtual double GetT( FOdysseyVectorSegment* iSegment ) override;

        void SetIntersection( FOdysseyVectorIntersection* iIntersection );
        FOdysseyVectorIntersection* GetIntersection();
        FOdysseyVectorVertexIntersection* GetPartner();

        virtual uint32 GetSectionCount() override;
        bool SelfIntersects();
        virtual void BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray ) override;

    private:
        static const uint32 mStaticClass =  0x29459195; // value is crc32 FOdysseyVectorVertexIntersection
        FOdysseyVectorIntersection* mIntersection;
        double mT;
        bool mSelfIntersects;
};
