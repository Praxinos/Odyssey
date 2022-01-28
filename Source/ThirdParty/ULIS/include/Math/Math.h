// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Math.h
* @author       Clement Berthaud
* @brief        This file provides the FMath class declaration.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include <cmath>

#define ULIS_MATHS_FUNC ULIS_FORCEINLINE constexpr

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// FMath
namespace FMath
{
    // Do not change this value
    static constexpr float  kEpsilonf = .000001f;
    static constexpr double kPId = 3.1415926535897932;
    static constexpr float  kPIf = 3.14159265f;
    static constexpr float  kGoldenRatio = 1.61803398875f;

    static ULIS_FORCEINLINE int32 FloorToInt( ufloat iValue ) {
        #ifdef ULIS_COMPILED_WITH_SIMD_SUPPORT
            return  _mm_cvt_ss2si( _mm_set_ss( iValue + iValue - 0.5f ) ) >> 1;
        #else
            return  static_cast< int32>( floor( iValue ) );
        #endif
    }

    static ULIS_FORCEINLINE ufloat FloorToFloat( ufloat iValue ) {
        return  static_cast< ufloat >( FloorToInt( iValue ) );
    }

    static ULIS_FORCEINLINE int32 CeilToInt( ufloat iValue ) {
        #ifdef ULIS_COMPILED_WITH_SIMD_SUPPORT
            return -( _mm_cvt_ss2si( _mm_set_ss( -0.5f - ( iValue + iValue ) ) ) >> 1 );
        #else
            return  static_cast< int32>( ceil( iValue ) );
        #endif
    }

    static ULIS_FORCEINLINE ufloat CeilToFloat( ufloat iValue ) {
        return static_cast< ufloat >( CeilToInt( iValue ) );
    }

    static ULIS_FORCEINLINE ufloat RoundToNegativeInfinity( ufloat iValue ) {
        return  FloorToFloat( iValue );
    }

    static ULIS_FORCEINLINE ufloat RoundToPositiveInfinity( ufloat iValue ) {
        return  CeilToFloat( iValue );
    }

    template< typename S, typename T, typename U >
    static ULIS_FORCEINLINE S Pow( T iValue, U iExponent ) {
        return  static_cast< S >( pow( static_cast< S >( iValue ), static_cast< S >( iExponent ) ) );
    }

    template< typename T >
    static ULIS_FORCEINLINE T Min( T iA, T iB ) {
        return  iA < iB ? iA : iB;
    }

    template< typename T >
    static ULIS_FORCEINLINE T Max( T iA, T iB ) {
        return  iA > iB ? iA : iB;
    }

    template< typename T >
    static ULIS_FORCEINLINE T Min3( T iA, T iB, T iC ) {
        return  Min( iA, Min( iB, iC ) );
    }

    template< typename T >
    static ULIS_FORCEINLINE T Min4( T iA, T iB, T iC, T iD ) {
        return  Min( iA, Min3( iB, iC, iD ) );
    }

    template< typename T >
    static ULIS_FORCEINLINE T Max3( T iA, T iB, T iC ) {
        return  Max( iA, Max( iB, iC ) );
    }

    template< typename T >
    static ULIS_FORCEINLINE T Max4( T iA, T iB, T iC, T iD ) {
        return  Max( iA, Max3( iB, iC, iD ) );
    }

    template< typename T >
    static inline T&&  VMin( T&&  val )
    {
        return  std::forward< T >( val );
    }

    template< typename T0, typename T1, typename... Ts >
    static inline auto  VMin( T0&&  val1, T1&&  val2, Ts&&...  vs )
    {
        return  ( val1 < val2 ) ? VMin( val1, std::forward< Ts >( vs )... ) : VMin( val2, std::forward< Ts >( vs )... );
    }

    template< typename T >
    static inline T&&  VMax( T&&  val )
    {
        return  std::forward< T >( val );
    }

    template< typename T0, typename T1, typename... Ts >
    static inline auto  VMax( T0&&  val1, T1&&  val2, Ts&&...  vs )
    {
        return  ( val1 > val2 ) ? VMax( val1, std::forward< Ts >( vs )... ) : VMax( val2, std::forward< Ts >( vs )... );
    }

    template< typename T >
    static ULIS_FORCEINLINE T Clamp( T iValue, T iMin, T iMax ) {
        return  Max( iMin, Min( iValue, iMax ) );
    }

    template< typename T >
    static ULIS_MATHS_FUNC T Abs( T iA ) {
        #pragma warning(push)
        #pragma warning(disable : 4146) // Shut warning unary negative on unsigned type.
        return ( iA < 0 ? -iA : iA );
        #pragma warning(pop)
    }

    template< typename T >
    static ULIS_FORCEINLINE T Sign( T iValue ) {
        return  iValue < static_cast< T >( 0 ) ? static_cast< T >( -1 ) : static_cast< T >( 1 );
    }

    template< typename T >
    static ULIS_FORCEINLINE T PyModulo( T iValue, T iMod ) {
        return  ( iMod + ( iValue % iMod ) ) % iMod;
    }

    template<>
    /*static*/ ULIS_FORCEINLINE float PyModulo< float >( float iValue, float iMod ) {
        return  fmod( iMod + fmod( iValue, iMod ), iMod );
    }

    template< typename T >
    static ULIS_MATHS_FUNC T Mod( T iA, T iB ) {
        return  iA % iB;
    }

    template<>
    /*static*/ ULIS_FORCEINLINE double Mod( double iA, double iB ) {
        return  fmod( iA, iB );
    }

    template<>
    /*static*/ ULIS_FORCEINLINE float Mod( float iA, float iB ) {
        return  fmodf( iA, iB );
    }

    static ULIS_FORCEINLINE double RadToDeg( double iRad )
    {
        return ( iRad * 180 / FMath::kPId );
    }

    static ULIS_FORCEINLINE float RadToDegF(float iRad)
    {
        return (iRad * 180.f / FMath::kPIf);
    }

    static ULIS_FORCEINLINE double DegToRad( double iDeg )
    {
        return ( iDeg * FMath::kPId / 180 );
    }

    static ULIS_FORCEINLINE float DegToRadF( float iDeg )
    {
        return ( iDeg * FMath::kPIf / 180.f );
    }

    static ULIS_MATHS_FUNC int IntegerPartOfNumber( float iNumber )
    {
        return (int)iNumber;
    }

    static ULIS_MATHS_FUNC float FloatingPartOfNumber( float iNumber )
    {
        if ( iNumber > 0 )
            return iNumber - IntegerPartOfNumber( iNumber );
        else
            return - ( iNumber - IntegerPartOfNumber( iNumber ) );
    }

    static ULIS_FORCEINLINE int Round( float iNumber )
    {
        return  (int)( iNumber + 0.5f );
    }

    template< typename T >
    static ULIS_FORCEINLINE T Ceil( T iValue )
    {
        return  ceil( iValue );
    }

    template< typename T >
    static ULIS_FORCEINLINE T Floor( T iValue )
    {
        return  floor( iValue );
    }

    template< typename T >
    static ULIS_FORCEINLINE T RoundAwayFromZero( T iValue )
    {
        return  iValue < 0 ? (T)floor( iValue ) : (T)ceil( iValue );
    }

    template< typename T >
    static ULIS_FORCEINLINE T RoundTowardsZero( T iValue )
    {
        return  iValue < 0 ? (T)ceil( iValue ) : (T)floor( iValue );
    }

    static ULIS_FORCEINLINE bool EpsilonComp( float iA, float iB )
    {
        return  Abs( iA - iB ) <= kEpsilonf;
    }

    template< typename T >
    static ULIS_MATHS_FUNC T Sqrt( T iValue )
    {
        return  static_cast< T >( sqrt( iValue ) );
    }

    template<>
    /*static*/ ULIS_FORCEINLINE double Sqrt< double >( double iValue )
    {
        return  sqrt( iValue );
    }

    template<>
    /*static*/ ULIS_FORCEINLINE float Sqrt< float >( float iValue )
    {
        return  sqrtf( iValue );
    }

    template< typename T >
    static ULIS_FORCEINLINE T InvSqrt( T iValue ) {
        return  static_cast< T >( 1.0 / Sqrt( iValue ) );
    }

    template<>
    /*static*/ ULIS_FORCEINLINE float InvSqrt( float iValue ) {
        float x2 = iValue * 0.5f;
        const float threehalfs = 1.5f;
        float  convf = iValue;
        uint32 convi = 0;
        memcpy( &convi, &convf, sizeof( float ) );
        convi = 0x5f3759df - ( convi >> 1 );
        memcpy( &convf, &convi, sizeof( float ) );
        convf = convf * ( threehalfs - ( x2 * convf * convf ) );
        return convf;
    }

    static ULIS_FORCEINLINE void ByteSwap( void* V, int32 Length )
    {
        uint8* Ptr = (uint8*)V;
        int32 Top = Length - 1;
        int32 Bottom = 0;
        while(Bottom < Top)
        {
            std::swap(Ptr[Top--],Ptr[Bottom++]);
        }
    }

    template< typename T >
    static ULIS_FORCEINLINE double Dist( T x1, T y1, T x2, T y2 )
    {
        return Sqrt( std::pow( x2 - x1, 2 ) + std::pow( y2 - y1, 2 ) );
    }



    // Constexpr
    template< typename T, typename U >
    constexpr std::common_type_t< T, U > ConstexprPow_Imp( T iValue, U iExp )
    {
        return    ( iExp == 0) ? 1
                : ( iExp % 2 == 0 ) ? ConstexprPow_Imp( iValue * iValue, iExp / 2 )
                : iValue * ConstexprPow_Imp( iValue * iValue, ( iExp - 1 ) / 2 );
    }

    template< typename T, typename U >
    constexpr std::common_type_t< T, U > ContexprPow( T iValue, U iExp )
    {
        static_assert( std::is_integral< U >::value, "Only integer exponents !" );
        return    ( iExp == 0) ? 1
                : ( iExp > 0 ) ? ConstexprPow_Imp( iValue, iExp )
                : 1 / ConstexprPow_Imp( iValue, -iExp );
    }

    template< typename T >
    constexpr T ConstexprMin( T iA, T iB ) {
        return  iA < iB ? iA : iB;
    }

    constexpr int64 ConstexprILog2( int64 iValue )
    {
        return iValue < 2 ? 0 : 1 + ConstexprILog2( iValue / 2 );
    }

    constexpr int64 ConstexprINeededBitsForRange( int64 iValue )
    {
        return  ContexprPow( 2, ConstexprILog2( iValue ) ) == iValue ? ConstexprILog2( iValue ) : ConstexprILog2( iValue ) + 1;
    }

} // namespace FMath

ULIS_NAMESPACE_END

