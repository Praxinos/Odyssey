// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Units.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for Units tools.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Math.h"

ULIS_NAMESPACE_BEGIN
namespace detail {
//constexpr const char* kwUnits[][9] = {
//      { "", "kilo", "mega", "giga", "tera", "peta", "exa", "zetta", "yotta" }
//    , { "", "kibi", "mebi", "gibi", "tebi", "pebi", "exbi", "zebi", "yobi" }
//    , { "", "k", "M", "G", "T", "P", "E", "Z", "Y" }
//    , { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi", "Yi" }
//};
} // namespace detail

/////////////////////////////////////////////////////
/// @enum       eMetricSystemStandard
/// @brief      Metric System Standard: either SI ( systeme international ) or
///             IEC ( International Electrotechnical Commission ).
enum class eMetricSystemStandard {
      kSI   // Systeme International )
    , kIEC  // International Electrotechnical Commission
};

/////////////////////////////////////////////////////
/// @class      TBinaryInformationUnit
/// @brief      The TBinaryInformationUnit class allows the use of binary memory
///             units using any metric system standard defined in eMetricSystemStandard
template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
struct TBinaryInformationUnit
{
    /////////////////////////////////////////////////////
    /// @class      TMetricSystemStandardSpecs
    /// @brief      The TMetricSystemStandardSpecs class allows to specialize
    ///             and differentiate constant multiplier for various metrics systems
    ///             standards as defined in eMetricSystemStandard.
    template< eMetricSystemStandard >
    struct TMetricSystemStandardSpecs {
    };

    template<>
    struct TMetricSystemStandardSpecs< eMetricSystemStandard::kSI > {
        static constexpr double multiplier = 1000.0;
    };

    template<>
    struct TMetricSystemStandardSpecs< eMetricSystemStandard::kIEC > {
        static constexpr double multiplier = 1024.0;
    };

    /////////////////////////////////////////////////////
    /// @class      TUnitConversionFactor
    /// @brief      The TUnitConversionFactor class allows to get the conversion factor
    ///             beetween any two binary information units.
    template< eMetricSystemStandard FROM_STD, uint8 FROM_POW, uint8 FROM_SEQ, eMetricSystemStandard TO_STD, uint8 TO_POW, uint8 TO_SEQ >
    struct TUnitConversionFactor {
        static constexpr double value = 1
            * FMath::ContexprPow< double, int16 >( TMetricSystemStandardSpecs< FROM_STD >::multiplier, FROM_POW )
            / FMath::ContexprPow< double, int16 >( TMetricSystemStandardSpecs< TO_STD >::multiplier, TO_POW )
            * static_cast< double >( FROM_SEQ ) / TO_SEQ;
    };

    /////////////////////////////////////////////////////
    /// C-Tors
    template< typename T >
    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( T iValue )
        : value( static_cast< double >( iValue ) )
    {}

    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iOther )
        : value( iOther.value )
    {}

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther )
        : value(
            + iOther.Value()
            * TUnitConversionFactor< OTHER_STD, OTHER_POW, OTHER_SEQ, SELF_STD, SELF_POW, SELF_SEQ >::value
        )
    {}

    double Value() const {
        return  value;
    }

    operator double() const {
        return  value;
    }

    operator uint64() const {
        return  static_cast< uint64 >( FMath::Ceil( value ) );
    }

    operator uint32() const {
        ULIS_WARNING( true, "Warning: cast to 32 bits may overflow" );
        return  static_cast< uint32 >( FMath::Ceil( value ) );
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    bool operator==( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        return  this->value == TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).value;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    bool operator!=( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        !( operator==( iOther ) );
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    bool operator<( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        return  this->value < TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).value;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    bool operator<=( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        return  this->value <= TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).value;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    bool operator>( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        return  this->value > TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).value;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    bool operator>=( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        return  this->value >= TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).value;
    }

    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator-() const {
        return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( -value );
    }

    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator+=( double iValue ) {
        value += static_cast< double >( iValue );
        return  *this;
    }

    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator-=( double iValue ) {
        value -= static_cast< double >( iValue );
        return  *this;
    }

    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator*=( double iValue ) {
        value *= static_cast< double >( iValue );
        return  *this;
    }

    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator/=( double iValue ) {
        value /= static_cast< double >( iValue );
        return  *this;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator+=( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        value += TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).Value();
        return  *this;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator-=( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        value -= TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).Value();
        return  *this;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator*=( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        value *= TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).Value();
        return  *this;
    }

    template< eMetricSystemStandard OTHER_STD, uint8 OTHER_POW, uint8 OTHER_SEQ >
    TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& operator/=( const TBinaryInformationUnit< OTHER_STD, OTHER_POW, OTHER_SEQ >& iOther ) {
        value /= TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >( iOther ).Value();
        return  *this;
    }

private:
    double value;
};

// Binary Operators with scalar on left hand side
template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator+( double iValue, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iValue + iUnit.Value()
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator-( double iValue, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iValue - iUnit.Value()
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator*( double iValue, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iValue * iUnit.Value()
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator/( double iValue, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iValue / iUnit.Value()
    );
}

// Binary Operators with scalar on right hand side
template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator+( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit, double iValue ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iUnit.Value() + iValue
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator-( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit, double iValue ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iUnit.Value() - iValue
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator*( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit, double iValue ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iUnit.Value() * iValue
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator/( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iUnit, double iValue ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iUnit.Value() / iValue
    );
}

// Binary Operators with self type
template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator+( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iA, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iB ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iA.Value() + iB.Value()
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator-( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iA, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iB ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iA.Value() - iB.Value()
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator*( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iA, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iB ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iA.Value() * iB.Value()
    );
}

template< eMetricSystemStandard SELF_STD, uint8 SELF_POW, uint8 SELF_SEQ >
TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ > operator/( const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iA, const TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >& iB ) {
    return  TBinaryInformationUnit< SELF_STD, SELF_POW, SELF_SEQ >(
          iA.Value() / iB.Value()
    );
}

/////////////////////////////////////////////////////
/// Typedefs
// Note:
// Unlike other prefixes, kilo is the only abbreviated lower case prefix for SI.
// Unlike SI, IEC keeps uppercase kilo prefix.
// SI Typedef Metric - bit
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 0, 1 > bit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 1, 1 > kilobit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 2, 1 > megabit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 3, 1 > gigabit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 4, 1 > terabit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 5, 1 > petabit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 6, 1 > exabit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 7, 1 > zettabit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 8, 1 > yottabit_t;
typedef kilobit_t       kbit_t;
typedef megabit_t       Mbit_t;
typedef gigabit_t       Gbit_t;
typedef terabit_t       Tbit_t;
typedef petabit_t       Pbit_t;
typedef exabit_t        Ebit_t;
typedef zettabit_t      Zbit_t;
typedef yottabit_t      Ybit_t;
// SI Typedef Metric - byte
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 0, 8 > byte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 1, 8 > kilobyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 2, 8 > megabyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 3, 8 > gigabyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 4, 8 > terabyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 5, 8 > petabyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 6, 8 > exabyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 7, 8 > zettabyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kSI, 8, 8 > yottabyte_t;
typedef byte_t          B_t;
typedef kilobyte_t      kB_t;
typedef megabyte_t      MB_t;
typedef gigabyte_t      GB_t;
typedef terabyte_t      TB_t;
typedef petabyte_t      PB_t;
typedef exabyte_t       EB_t;
typedef zettabyte_t     ZB_t;
typedef yottabyte_t     YB_t;
typedef byte_t          octet_t;
typedef kilobyte_t      kilooctet_t;
typedef megabyte_t      megaoctet_t;
typedef gigabyte_t      gigaoctet_t;
typedef terabyte_t      teraoctet_t;
typedef petabyte_t      petaoctet_t;
typedef exabyte_t       exaoctet_t;
typedef zettabyte_t     zettaoctet_t;
typedef yottabyte_t     yottaoctet_t;
typedef octet_t         o_t;
typedef kilooctet_t     ko_t;
typedef megaoctet_t     Mo_t;
typedef gigaoctet_t     Go_t;
typedef teraoctet_t     To_t;
typedef petaoctet_t     Po_t;
typedef exaoctet_t      Eo_t;
typedef zettaoctet_t    Zo_t;
typedef yottaoctet_t    Yo_t;
// IEC Typedef Metric - bit
//typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 0, 8 > bit_t; // No need to redefine, same as SI
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 1, 1 > kibibit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 2, 1 > mebibit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 3, 1 > gibibit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 4, 1 > tebibit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 5, 1 > pebibit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 6, 1 > exbibit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 7, 1 > zebibit_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 8, 1 > yobibit_t;
typedef kibibit_t       Kibit_t;
typedef mebibit_t       Mibit_t;
typedef gibibit_t       Gibit_t;
typedef tebibit_t       Tibit_t;
typedef pebibit_t       Pibit_t;
typedef exbibit_t       Eibit_t;
typedef zebibit_t       Zibit_t;
typedef yobibit_t       Yibit_t;
// IEC Typedef Metric - byte
//typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 0, 8 > byte_t; // No need to redefine, same as SI
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 1, 8 > kibibyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 2, 8 > mebibyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 3, 8 > gibibyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 4, 8 > tebibyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 5, 8 > pebibyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 6, 8 > exbibyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 7, 8 > zebibyte_t;
typedef TBinaryInformationUnit< eMetricSystemStandard::kIEC, 8, 8 > yobibyte_t;
typedef kibibyte_t      KiB_t;
typedef mebibyte_t      MiB_t;
typedef gibibyte_t      GiB_t;
typedef tebibyte_t      TiB_t;
typedef pebibyte_t      PiB_t;
typedef exbibyte_t      EiB_t;
typedef zebibyte_t      ZiB_t;
typedef yobibyte_t      YiB_t;
typedef kibibyte_t      kibioctet_t;
typedef mebibyte_t      mebioctet_t;
typedef gibibyte_t      gibioctet_t;
typedef tebibyte_t      tebioctet_t;
typedef pebibyte_t      pebioctet_t;
typedef exbibyte_t      exbioctet_t;
typedef zebibyte_t      zebioctet_t;
typedef yobibyte_t      yobioctet_t;
typedef kibioctet_t     Kio_t;
typedef mebioctet_t     Mio_t;
typedef gibioctet_t     Gio_t;
typedef tebioctet_t     Tio_t;
typedef pebioctet_t     Pio_t;
typedef exbioctet_t     Eio_t;
typedef zebioctet_t     Zio_t;
typedef yobioctet_t     Yio_t;

namespace units_literals {
/////////////////////////////////////////////////////
/// Used defined literals
// double
// User defined literals - SI - bit - double
ULIS_API bit_t operator "" _bit( long double i );
ULIS_API kbit_t operator "" _kbit( long double i );
ULIS_API Mbit_t operator "" _Mbit( long double i );
ULIS_API Gbit_t operator "" _Gbit( long double i );
ULIS_API Tbit_t operator "" _Tbit( long double i );
ULIS_API Pbit_t operator "" _Pbit( long double i );
ULIS_API Ebit_t operator "" _Ebit( long double i );
ULIS_API Zbit_t operator "" _Zbit( long double i );
ULIS_API Ybit_t operator "" _Ybit( long double i );
// User defined literals - SI - byte - double
ULIS_API B_t operator "" _B( long double i );
ULIS_API kB_t operator "" _kB( long double i );
ULIS_API MB_t operator "" _MB( long double i );
ULIS_API GB_t operator "" _GB( long double i );
ULIS_API TB_t operator "" _TB( long double i );
ULIS_API PB_t operator "" _PB( long double i );
ULIS_API EB_t operator "" _EB( long double i );
ULIS_API ZB_t operator "" _ZB( long double i );
ULIS_API YB_t operator "" _YB( long double i );
// User defined literals - SI - octet - double
ULIS_API o_t operator "" _o( long double i );
ULIS_API ko_t operator "" _ko( long double i );
ULIS_API Mo_t operator "" _Mo( long double i );
ULIS_API Go_t operator "" _Go( long double i );
ULIS_API To_t operator "" _To( long double i );
ULIS_API Po_t operator "" _Po( long double i );
ULIS_API Eo_t operator "" _Eo( long double i );
ULIS_API Zo_t operator "" _Zo( long double i );
ULIS_API Yo_t operator "" _Yo( long double i );
// User defined literals - IEC - bit - double
ULIS_API Kibit_t operator "" _Kibit( long double i );
ULIS_API Mibit_t operator "" _Mibit( long double i );
ULIS_API Gibit_t operator "" _Gibit( long double i );
ULIS_API Tibit_t operator "" _Tibit( long double i );
ULIS_API Pibit_t operator "" _Pibit( long double i );
ULIS_API Eibit_t operator "" _Eibit( long double i );
ULIS_API Zibit_t operator "" _Zibit( long double i );
ULIS_API Yibit_t operator "" _Yibit( long double i );
// User defined literals - IEC - byte - double
ULIS_API KiB_t operator "" _KiB( long double i );
ULIS_API MiB_t operator "" _MiB( long double i );
ULIS_API GiB_t operator "" _GiB( long double i );
ULIS_API TiB_t operator "" _TiB( long double i );
ULIS_API PiB_t operator "" _PiB( long double i );
ULIS_API EiB_t operator "" _EiB( long double i );
ULIS_API ZiB_t operator "" _ZiB( long double i );
ULIS_API YiB_t operator "" _YiB( long double i );
// User defined literals - IEC - octet - double
ULIS_API Kio_t operator "" _Kio( long double i );
ULIS_API Mio_t operator "" _Mio( long double i );
ULIS_API Gio_t operator "" _Gio( long double i );
ULIS_API Tio_t operator "" _Tio( long double i );
ULIS_API Pio_t operator "" _Pio( long double i );
ULIS_API Eio_t operator "" _Eio( long double i );
ULIS_API Zio_t operator "" _Zio( long double i );
ULIS_API Yio_t operator "" _Yio( long double i );
// uint64_literal
// User defined literals - SI - bit - uint64_literal
ULIS_API bit_t operator "" _bit( uint64_literal i );
ULIS_API kbit_t operator "" _kbit( uint64_literal i );
ULIS_API Mbit_t operator "" _Mbit( uint64_literal i );
ULIS_API Gbit_t operator "" _Gbit( uint64_literal i );
ULIS_API Tbit_t operator "" _Tbit( uint64_literal i );
ULIS_API Pbit_t operator "" _Pbit( uint64_literal i );
ULIS_API Ebit_t operator "" _Ebit( uint64_literal i );
ULIS_API Zbit_t operator "" _Zbit( uint64_literal i );
ULIS_API Ybit_t operator "" _Ybit( uint64_literal i );
// User defined literals - SI - byte - uint64_literal
ULIS_API B_t operator "" _B( uint64_literal i );
ULIS_API kB_t operator "" _kB( uint64_literal i );
ULIS_API MB_t operator "" _MB( uint64_literal i );
ULIS_API GB_t operator "" _GB( uint64_literal i );
ULIS_API TB_t operator "" _TB( uint64_literal i );
ULIS_API PB_t operator "" _PB( uint64_literal i );
ULIS_API EB_t operator "" _EB( uint64_literal i );
ULIS_API ZB_t operator "" _ZB( uint64_literal i );
ULIS_API YB_t operator "" _YB( uint64_literal i );
// User defined literals - SI - octet - uint64_literal
ULIS_API o_t operator "" _o( uint64_literal i );
ULIS_API ko_t operator "" _ko( uint64_literal i );
ULIS_API Mo_t operator "" _Mo( uint64_literal i );
ULIS_API Go_t operator "" _Go( uint64_literal i );
ULIS_API To_t operator "" _To( uint64_literal i );
ULIS_API Po_t operator "" _Po( uint64_literal i );
ULIS_API Eo_t operator "" _Eo( uint64_literal i );
ULIS_API Zo_t operator "" _Zo( uint64_literal i );
ULIS_API Yo_t operator "" _Yo( uint64_literal i );
// User defined literals - IEC - bit - uint64_literal
ULIS_API Kibit_t operator "" _Kibit( uint64_literal i );
ULIS_API Mibit_t operator "" _Mibit( uint64_literal i );
ULIS_API Gibit_t operator "" _Gibit( uint64_literal i );
ULIS_API Tibit_t operator "" _Tibit( uint64_literal i );
ULIS_API Pibit_t operator "" _Pibit( uint64_literal i );
ULIS_API Eibit_t operator "" _Eibit( uint64_literal i );
ULIS_API Zibit_t operator "" _Zibit( uint64_literal i );
ULIS_API Yibit_t operator "" _Yibit( uint64_literal i );
// User defined literals - IEC - byte - uint64_literal
ULIS_API KiB_t operator "" _KiB( uint64_literal i );
ULIS_API MiB_t operator "" _MiB( uint64_literal i );
ULIS_API GiB_t operator "" _GiB( uint64_literal i );
ULIS_API TiB_t operator "" _TiB( uint64_literal i );
ULIS_API PiB_t operator "" _PiB( uint64_literal i );
ULIS_API EiB_t operator "" _EiB( uint64_literal i );
ULIS_API ZiB_t operator "" _ZiB( uint64_literal i );
ULIS_API YiB_t operator "" _YiB( uint64_literal i );
// User defined literals - IEC - octet - uint64_literal
ULIS_API Kio_t operator "" _Kio( uint64_literal i );
ULIS_API Mio_t operator "" _Mio( uint64_literal i );
ULIS_API Gio_t operator "" _Gio( uint64_literal i );
ULIS_API Tio_t operator "" _Tio( uint64_literal i );
ULIS_API Pio_t operator "" _Pio( uint64_literal i );
ULIS_API Eio_t operator "" _Eio( uint64_literal i );
ULIS_API Zio_t operator "" _Zio( uint64_literal i );
ULIS_API Yio_t operator "" _Yio( uint64_literal i );

} // namespace units

// Alloc units
typedef uint8 tByte;                        ///< A byte or octet value, 8bits
typedef uint8* tAlloc;                      ///< A base pointer for a heap allocation
typedef uint8* tMetaBase;                   ///< A base pointer for a meta base in an Arena ( see docs for specific arena about their meta base )
typedef uint8** tClient;                    ///< A pointer to a client, that is a pointer to a heap allocation susceptible to move after defrag.

ULIS_NAMESPACE_END

