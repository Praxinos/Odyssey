#pragma once

#include "CoreMinimal.h"

#include <ULIS>

#include "InbetweenerGrid.h"


class ODYSSEYVECTOR_API FInbetweenerGridARAP : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridARAP(){};
        FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        void Regularize();

        /**
         * @brief Update the grid when the owner object is updated.
         * @param iUpdateFlags update flags receieved by the owner object
         * @param iTagInvalidationFlags inbetweener tag invalidation flags
         */
        virtual void Update( uint32 iUpdateFlags
                           , uint64 iTagInvalidationFlags ) override;

        /**
         * @brief Get the rigidity for ARAP deformation
         * @return the rigidity
         */
        uint32 GetRigidity();

        /**
         * @brief Set the rigidity for ARAP deformation
         * @param iRigidity
         */
        void SetRigidity( uint32 iRigidity );

        /**
         * @brief Map paths to the grid according to the needs of the grid
         * @param iPathBuffer the paths to map
         */
        virtual void MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer ) override;

        friend class FOdysseyVectorTagInbetweener;

    // ARAP deformation (do not confuse with ARAP interpolation)
    protected:
        void DiscardEmptyQuads( std::vector<FInterpolatedPath>& iPathBuffer );
        uint32 Regularize( eInbetweenerPointPositionType iSourcePositionType
                         , eInbetweenerPointPositionType iDestPositionType
                         , int maxIterations
                         , bool allGrid
                         , bool convergenceStop );
        double RegularizeQuads( eInbetweenerPointPositionType iPositionType );
        void RegularizeQuad( FInbetweenerQuad* iQuad
                           , eInbetweenerPointPositionType iPositionType );
        void IntersectNeededQuads( const ::ULIS::FRectD& iSourceBBox
                                 , double iXMin
                                 , double iYMin
                                 , double iXMax
                                 , double iYMax );

    protected:
        uint32 mRigidity;
};
