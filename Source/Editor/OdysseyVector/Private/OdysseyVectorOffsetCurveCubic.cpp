#include "OdysseyVectorOffsetCurveCubic.h"

FOdysseyVectorOffsetCurveCubic::~FOdysseyVectorOffsetCurveCubic()
{

}

FOdysseyVectorOffsetCurveCubic::FOdysseyVectorOffsetCurveCubic()
{
}

void
FOdysseyVectorOffsetCurveCubic::Resize( uint32 iBezierCount )
{
    mBezierFragmentArray.resize( iBezierCount );
}

std::vector<FOdysseyVectorBezierFragment>&
FOdysseyVectorOffsetCurveCubic::GetBezierFragmentArray()
{ 
    return mBezierFragmentArray;
}

::ULIS::FVec2D
FOdysseyVectorOffsetCurveCubic::GetPointAt( double iT )
{
    for( int i = 0; i < mBezierFragmentArray.size(); i++ )
    {
        if( ( iT >= mBezierFragmentArray[i].fromT ) && ( iT <= mBezierFragmentArray[i].toT ) )
        {
            return ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( mBezierFragmentArray[i].bezier[0]
                                                                      , mBezierFragmentArray[i].bezier[1]
                                                                      , mBezierFragmentArray[i].bezier[2]
                                                                      , mBezierFragmentArray[i].bezier[3]
                                                                      , ( iT - mBezierFragmentArray[i].fromT ) / ( mBezierFragmentArray[i].toT - mBezierFragmentArray[i].fromT ) );
        }
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}
