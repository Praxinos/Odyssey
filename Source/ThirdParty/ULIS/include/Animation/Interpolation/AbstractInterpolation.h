// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Interpolation.h
* @author       Thomas Schmitt
* @brief        This file provides declaration for the basics of interpolation between animation keys
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN

template<typename T>
struct TKey;

enum eInterpolationType
{
    kLinear, 
    kHold,
    kBezier
};

template< typename T >
class TAbstractInterpolation
{
public:
    TAbstractInterpolation();
    virtual ~TAbstractInterpolation();
    virtual TAbstractInterpolation<T>* Clone() const = 0;

public:
    virtual T Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey ) const = 0;
};

template< typename T >
TAbstractInterpolation<T>::TAbstractInterpolation()
{

}

template< typename T >
TAbstractInterpolation<T>::~TAbstractInterpolation()
{

}

ULIS_NAMESPACE_END

