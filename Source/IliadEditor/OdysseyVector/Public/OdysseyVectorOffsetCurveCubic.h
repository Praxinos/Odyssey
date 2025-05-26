// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <ULIS>
#include <blend2d.h>

typedef struct _FOdysseyVectorBezierFragment
{
    ::ULIS::FVec2D bezier[4];
    ::ULIS::FVec2D straightVector;
    double straightLength;
    double fromT;
    double toT;

    _FOdysseyVectorBezierFragment()
    {
    }

    _FOdysseyVectorBezierFragment( ::ULIS::FVec2D iBezier[4], double iFromT, double iToT )
    {
        memcpy( bezier, iBezier, sizeof( bezier ) );

        fromT = iFromT;
        toT = iToT;
    }
} FOdysseyVectorBezierFragment;

class ODYSSEYVECTOR_API FOdysseyVectorOffsetCurveCubic
{
    public:
        ~FOdysseyVectorOffsetCurveCubic();
        FOdysseyVectorOffsetCurveCubic();

        /**
         * @brief Alloc memory for iBezierCount bezier fragment
         * @param iBezierCount the number of bezier fragments to allocate.
         */
        void Resize( uint32 iBezierCount );

        /**
         * @brief Get a reference to the bezier fragment array
         * @return a reference to the bezier fragment array
         */
        std::vector<FOdysseyVectorBezierFragment>& GetBezierFragmentArray();

        /**
         * @brief Get a point located at iT. iT is in the range 0.0f-1.0f and is absolute to
         *        the parent bezier curve. This function converts iT into the fragment's range.
         * @param iT
         * @return a position
         */
        ::ULIS::FVec2D GetPointAt( double iT );

    private:
        ::ULIS::FVec2D GetFragmentPointAt( FOdysseyVectorBezierFragment* iFragment, double iT );

    protected:
        std::vector<FOdysseyVectorBezierFragment> mBezierFragmentArray;
        FOdysseyVectorBezierFragment* mLastFragment;
        FOdysseyVectorBezierFragment* mNextFragment;
};
