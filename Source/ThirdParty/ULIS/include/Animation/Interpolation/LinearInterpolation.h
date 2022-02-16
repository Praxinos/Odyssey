// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LinearInterpolation.h
* @author       Thomas Schmitt
* @brief        This file provides declaration for the linear interpolation
* @license      Please refer to LICENSE.md
*//*
#pragma once
#include "Core/Core.h"
#include "Animation/Interpolation/AbstractInterpolation.h"

ULIS_NAMESPACE_BEGIN

template< typename T >
class TLinearInterpolation : public TAbstractInterpolation< T >
{
protected:
    TLinearInterpolation<T>();

    static TLinearInterpolation<T>* mInstance = nullptr;

public:
    TLinearInterpolation<T>(TLinearInterpolation<T> &other) = delete;
    void operator=(const TLinearInterpolation<T> &) = delete;
    static TLinearInterpolation<T> *GetInstance();
    static void *ReleaseInstance();

public:
    virtual T Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey ) const override;
};

template< typename T >
TLinearInterpolation<T>::TLinearInterpolation() :
TAbstractInterpolation<T>()
{

}

template< typename T >
TLinearInterpolation<T>* TLinearInterpolation<T>::GetInstance()
{
    if( mInstance == nullptr )
        mInstance = new TLinearInterpolation<T>();
    return  mInstance;
}

template< typename T >
void * TLinearInterpolation<T>::ReleaseInstance()
{
    if( mInstance != nullptr )
    {
        delete mInstance;
        mInstance = nullptr;
    }
}

template< typename T >
T TLinearInterpolation<T>::Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey) const
{
    ufloat t = (iFrame - iLeftKey.mFrame) / iRightKey.mFrame - iLeftKey.mFrame;

    return  iLeftKey.mValue + t * (iRightKey.mValue - iLeftKey.mValue);
}

ULIS_NAMESPACE_END*/

