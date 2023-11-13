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

        void Resize( uint32 iBezierCount );
        std::vector<FOdysseyVectorBezierFragment>& GetBezierFragmentArray();
        ::ULIS::FVec2D GetPointAt( double iT );

    private:
        ::ULIS::FVec2D GetFragmentPointAt( FOdysseyVectorBezierFragment* iFragment, double iT );

    protected:
        std::vector<FOdysseyVectorBezierFragment> mBezierFragmentArray;
        FOdysseyVectorBezierFragment* mLastFragment;
        FOdysseyVectorBezierFragment* mNextFragment;
};
