#include "OdysseyVectorOffsetCurveCubic.h"

FOdysseyVectorOffsetCurveCubic::~FOdysseyVectorOffsetCurveCubic()
{

}

FOdysseyVectorOffsetCurveCubic::FOdysseyVectorOffsetCurveCubic()
    : mLastFragment ( nullptr )
    , mNextFragment ( nullptr )
{
}

void
FOdysseyVectorOffsetCurveCubic::Resize( uint32 iBezierCount )
{
    mLastFragment = nullptr;
    mNextFragment = nullptr;

    mBezierFragmentArray.resize( iBezierCount );
}

std::vector<FOdysseyVectorBezierFragment>&
FOdysseyVectorOffsetCurveCubic::GetBezierFragmentArray()
{ 
    return mBezierFragmentArray;
}

inline ::ULIS::FVec2D
FOdysseyVectorOffsetCurveCubic::GetFragmentPointAt( FOdysseyVectorBezierFragment* iFragment, double iT )
{
    return ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( iFragment->bezier[0]
                                                              , iFragment->bezier[1]
                                                              , iFragment->bezier[2]
                                                              , iFragment->bezier[3]
                                                              , ( iT - iFragment->fromT ) / ( iFragment->toT - iFragment->fromT ) );
}

::ULIS::FVec2D
FOdysseyVectorOffsetCurveCubic::GetPointAt( double iT )
{
    int fragmentCount = mBezierFragmentArray.size();

    // for faster finding, we first check on the last fragment that was met
    if( mLastFragment )
    {
        if( ( iT >= mLastFragment->fromT ) && ( iT <= mLastFragment->toT ) )
        {
            return GetFragmentPointAt( mLastFragment, iT );
        }
    }

    // for faster finding, we also check on the fragment next to the last fragment that was met
    if( mNextFragment )
    {
        if( ( iT >= mNextFragment->fromT ) && ( iT <= mNextFragment->toT ) )
        {
            return GetFragmentPointAt( mNextFragment, iT );
        }
    }

    // then if nothing was found, we check in all fragments
    for( int i = 0; i < fragmentCount; i++ )
    {
        int n = ( i + 1 ) % fragmentCount;

        if( ( iT >= mBezierFragmentArray[i].fromT ) && ( iT <= mBezierFragmentArray[i].toT ) )
        {
            mLastFragment = &mBezierFragmentArray[i];
            mNextFragment = &mBezierFragmentArray[n];

            return ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( mBezierFragmentArray[i].bezier[0]
                                                                      , mBezierFragmentArray[i].bezier[1]
                                                                      , mBezierFragmentArray[i].bezier[2]
                                                                      , mBezierFragmentArray[i].bezier[3]
                                                                      , ( iT - mBezierFragmentArray[i].fromT ) / ( mBezierFragmentArray[i].toT - mBezierFragmentArray[i].fromT ) );
        }
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}
