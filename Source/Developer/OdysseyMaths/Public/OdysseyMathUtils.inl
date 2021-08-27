// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

template< class T >
/*static*/
T
FOdysseyMathUtils::CubicBezierPointAtParameter( const T& iP0,
                                                const T& iP1,
                                                const T& iP2,
                                                const T& iP3,
                                                float t )
{
    return iP0 * ( 1 - t ) * ( 1 - t ) * ( 1 - t ) +
           3 * iP1 * t * ( 1 - t ) * ( 1 - t ) +
           3 * iP2 * t * t * ( 1 - t ) +
           iP3 * t * t * t;
}

template< class T >
/*static*/
T
FOdysseyMathUtils::QuadraticBezierPointAtParameter( const T& iP0,
                                                    const T& iP1,
                                                    const T& iP2,
                                                    float t )
{
    return iP0 * ( 1 - t ) * ( 1 - t ) +
           2 * iP1 * t * ( 1 - t ) +
           iP2 * t * t;
}

/*static*/
inline
void
FOdysseyMathUtils::QuadraticBezierSplitAtParameter( FVector2D* ioP0,
                                                    FVector2D* ioP1,
                                                    FVector2D* ioP2,
                                                    float t )
{
    *ioP2 = QuadraticBezierPointAtParameter( *ioP0, *ioP1, *ioP2, t );
    *ioP1 = *ioP0 + ( *ioP1 - *ioP0 ) * t;
}

/*static*/
inline
void
FOdysseyMathUtils::QuadraticBezierInverseSplitAtParameter( FVector2D* ioP0,
                                                           FVector2D* ioP1,
                                                           FVector2D* ioP2,
                                                           float t )
{
    *ioP0 = QuadraticBezierPointAtParameter( *ioP0, *ioP1, *ioP2, t );
    *ioP1 = *ioP1 + ( *ioP2 - *ioP1 ) * t;
}

/*static*/
inline
void
FOdysseyMathUtils::QuadraticBezierSplitMiddle( FVector2D* ioP0,
                                               FVector2D* ioP1,
                                               FVector2D* ioP2 )
{
    *ioP2 = QuadraticBezierMiddlePoint( *ioP0, *ioP1, *ioP2 );
    *ioP1 = ( *ioP0 + *ioP1 ) / 2.f;
}

/*static*/
inline
void
FOdysseyMathUtils::QuadraticBezierInverseSplitMiddle( FVector2D* ioP0,
                                                      FVector2D* ioP1,
                                                      FVector2D* ioP2 )
{
    *ioP0 = QuadraticBezierMiddlePoint( *ioP0, *ioP1, *ioP2 );
    *ioP1 = ( *ioP1 + *ioP2 ) / 2.f;
}

template< class T >
/*static*/
T
FOdysseyMathUtils::QuadraticBezierMiddlePoint( const T& iP0,
                                               const T& iP1,
                                               const T& iP2 )
{
    return iP0 * 0.25f +
           iP1 * 0.5f +
           iP2 * 0.25f;
}

/*static*/
inline
float
FOdysseyMathUtils::QuadraticBezierSampledLengthApproximation( const FVector2D& iP0,
                                                              const FVector2D& iP1,
                                                              const FVector2D& iP2,
                                                              int iSteps )
{
    FVector2D last = iP0;
    float step = 1.f / iSteps;
    float dist = 0.f;
    for( int i = 1; i <= iSteps; ++i )
    {
        float t = i * step;
        FVector2D next = QuadraticBezierPointAtParameter( iP0, iP1, iP2, t );
        dist += FVector2D::Distance( last, next );
        last = next;
    }
    return dist;
}

/*static*/
inline
float
FOdysseyMathUtils::QuadraticBezierRecursiveAdaptativeSampledLengthApproximation( const FVector2D& iP0,
                                                                                 const FVector2D& iP1,
                                                                                 const FVector2D& iP2,
                                                                                 int iAdaptativeLength )
{
    return _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( iP0, iP1, iP2, iAdaptativeLength );
}

/*static*/
inline
float
FOdysseyMathUtils::_QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( const FVector2D& iP0,
                                                                                      const FVector2D& iP1,
                                                                                      const FVector2D& iP2,
                                                                                      int iAdaptativeLength )
{
    FVector2D mid = QuadraticBezierMiddlePoint( iP0, iP1, iP2 );
    float lengthSegmentA = FVector2D::Distance( iP0, mid );
    float lengthSegmentB = FVector2D::Distance( mid, iP2 );
    if( lengthSegmentA > float( iAdaptativeLength ) )
    {
        FVector2D A = iP0;
        FVector2D B = iP1;
        FVector2D C = iP2;
        QuadraticBezierSplitMiddle( &A, &B, &C );
        lengthSegmentA = _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( A, B, C, iAdaptativeLength );
    }

    if( lengthSegmentB > float( iAdaptativeLength ) )
    {
        FVector2D D = iP0;
        FVector2D E = iP1;
        FVector2D F = iP2;
        QuadraticBezierInverseSplitMiddle( &D, &E, &F );
        lengthSegmentB = _QuadraticBezierRecursiveAdaptativeSampledLengthApproximation_Imp( D, E, F, iAdaptativeLength );
    }

    return lengthSegmentA + lengthSegmentB;
}

/*static*/
inline
float
FOdysseyMathUtils::QuadraticBezierLengthAtParameter( const FVector2D& iP0,
                                                     const FVector2D& iP1,
                                                     const FVector2D& iP2,
                                                     float t )
{
    FVector2D A = iP0;
    FVector2D B = iP1;
    FVector2D C = iP2;
    QuadraticBezierSplitAtParameter( &A, &B, &C, t );

    return QuadraticBezierRecursiveAdaptativeSampledLengthApproximation( A, B, C );
}

/*static*/
inline
float
FOdysseyMathUtils::QuadraticBezierLinearParameterAtParameter( const FVector2D& iP0,
                                                              const FVector2D& iP1,
                                                              const FVector2D& iP2,
                                                              float t )
{
    if( t == 1.f )
        return 1.f;
    if( t == 0.5f )
        return 0.5f;
    if( t == 0.f )
        return 0.f;

    float length = QuadraticBezierRecursiveAdaptativeSampledLengthApproximation( iP0, iP1, iP2 );
    float lengthParam = QuadraticBezierLengthAtParameter( iP0, iP1, iP2, t );

    return length / lengthParam;
}

/*static*/
inline
float
FOdysseyMathUtils::QuadraticBezierGenerateLinearLUT( TArray< FOdysseyBezierLutElement >* ioArray,
                                                     const FVector2D& iP0,
                                                     const FVector2D& iP1,
                                                     const FVector2D& iP2,
                                                     int iAdaptativeLength )
{
    ioArray->Empty();
    ioArray->Add( { iP0, 0.f } );
    float length = QuadraticBezierGenerateLinearLUT_Imp( ioArray, iP0, iP1, iP2, iAdaptativeLength );
    ioArray->Add( { iP2, length } );

    return length;
}

/*static*/
inline
float
FOdysseyMathUtils::QuadraticBezierGenerateLinearLUT_Imp( TArray< FOdysseyBezierLutElement >* ioArray,
                                                         const FVector2D& iP0,
                                                         const FVector2D& iP1,
                                                         const FVector2D& iP2,
                                                         int iAdaptativeLength,
                                                         float iOffset )
{
    FVector2D mid = QuadraticBezierMiddlePoint( iP0, iP1, iP2 );
    float lengthSegmentA = FVector2D::Distance( iP0, mid );
    float lengthSegmentB = FVector2D::Distance( mid, iP2 );

    if( lengthSegmentA > float( iAdaptativeLength ) )
    {
        FVector2D A = iP0;
        FVector2D B = iP1;
        FVector2D C = iP2;
        QuadraticBezierSplitMiddle( &A, &B, &C );
        lengthSegmentA = QuadraticBezierGenerateLinearLUT_Imp( ioArray, A, B, C, iAdaptativeLength, iOffset );
    }

    ioArray->Add( { mid, ( lengthSegmentA + iOffset ) } );

    if( lengthSegmentB > float( iAdaptativeLength ) )
    {
        FVector2D D = iP0;
        FVector2D E = iP1;
        FVector2D F = iP2;
        QuadraticBezierInverseSplitMiddle( &D, &E, &F );
        lengthSegmentB = QuadraticBezierGenerateLinearLUT_Imp( ioArray, D, E, F, iAdaptativeLength, iOffset + lengthSegmentA );
    }

    return ( lengthSegmentA + lengthSegmentB );
}

inline
void 
FOdysseyMathUtils::ByteSwap(void* V,int32 Length)
{
    uint8* Ptr = (uint8*)V;
    int32 Top = Length - 1;
    int32 Bottom = 0;
    while(Bottom < Top)
    {
        Swap(Ptr[Top--],Ptr[Bottom++]);
    }
}

/* In the next four functions, we want to map a linear slider value to an exponential function such that:
The slider value is a classic floating number "s" on [0;1]
At the far left, at 0.0, the resulting value is "m" (configurable), for example 1/20 = 0.05 = 5%
At the far right, at 1.0, the resulting value is "M" (configurable), for example 20/1 = 20 = 2000%
At the middle of the slider, at 0.5, the the resulting value is "nV" the Neutral Value, for exemple 1 = 100%
The direct mapping range is 20 - 0.05 = 19.95
The percentage range is 2000% - 5% = 1995%

Let's find the ideal formula to apply to the slider, in order to get the resulting values.
It would be the following : f( s ) = ( s ^ n ) * r + m
where :
s: linear position on the slider
n: exposant ( we'll look for it later )
r: range ( r = M - m )
m: minimum ( decided by the user : here 1/20 )
M: maximum ( decided by the user : here 20/1 )

if "s" equals "0" :
f( 0 )  = m
        = ( 0 ^ n ) * r + m
        = 0 * r + m
        = 0.05 -> 5%

if "s" equals "1" :
f( 1 )  = M
        = ( 1 ^ n ) * r + m
        = 1 * r + m
        = r + m
        = 19.95 + 0.05
        = 20 -> 2000%

This means that : r = M - m

Now, we look for the exponent "n" such that f( 0.5 ) = nV, i.e. being right in the middle of the slider at 0.5 corresponds to the value nV ( for example 1 = 100% ).

f( 0.5 ) = nV
( 0.5 ^ n ) * r + m = nV
( 0.5 ^ n ) * r = nV - m
( 0.5 ^ n ) = ( nV - m ) / r
ln( 0.5 ^ n ) = ln( ( nV - m ) / r )
n * ln( 0.5 ) = ln( ( nV - m ) / r )
n = ln( ( nV - m ) / r ) / ln( 0.5 )

Now we have the range "r" and the exponent "n".

so we have the fonction : f( s ) = ( s ^ ( ln( ( nV - m ) / ( M - m ) ) / ln( 0.5 ) ) ) * ( M - m ) + m
*/

/*static*/
inline
double
FOdysseyMathUtils::ExponentValue( const double iMin, const double iMax, const double iNeutralValue )
{
    return log( ((iNeutralValue - iMin) / (iMax - iMin)) ) / (log(0.5f));
}

/*static*/
inline
double
FOdysseyMathUtils::RangeValue( const double iMin, const double iMax )
{
    return iMax - iMin;
}

/*static*/
inline
double
FOdysseyMathUtils::ExponentialFunction( const double iMin, const double iMax, const double iNeutralValue, const double iValue )
{
    return ::FMath::Clamp(( RangeValue( iMin, iMax ) ) * pow( iValue, ExponentValue( iMin, iMax, iNeutralValue) ) + iMin, iMin, iMax);
}

/*static*/
inline
double
FOdysseyMathUtils::ExponentialFunctionInvert( const double iMin, const double iMax, const double iNeutralValue, const double iValue )
{
    double value = ::FMath::Clamp(iValue, iMin, iMax);
    return exp( log( (value - iMin) / RangeValue( iMin, iMax ) ) / ExponentValue( iMin, iMax, iNeutralValue ) );
}
