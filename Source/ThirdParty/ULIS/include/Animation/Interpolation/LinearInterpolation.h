// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LinearInterpolation.h
* @author       Thomas Schmitt
* @brief        This file provides declaration for the linear interpolation
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Animation/Interpolation/AbstractInterpolation.h"

ULIS_NAMESPACE_BEGIN

template< typename T >
class TLinearInterpolation : public TAbstractInterpolation< T >
{
public:
    TLinearInterpolation<T>();
    virtual TLinearInterpolation<T>* Clone() const;

public:
    virtual T Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey ) const override;
};

template< typename T >
TLinearInterpolation<T>::TLinearInterpolation() :
TAbstractInterpolation<T>()
{

}

template< typename T >
TLinearInterpolation<T>* TLinearInterpolation<T>::Clone() const
{
    return new TLinearInterpolation<T>();
}

template< typename T >
T TLinearInterpolation<T>::Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey) const
{
    ufloat t = (iFrame - iLeftKey.mFrame) / iRightKey.mFrame - iLeftKey.mFrame;

    return  iLeftKey.mValue + t * (iRightKey.mValue - iLeftKey.mValue);
}

ULIS_NAMESPACE_END

