// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HoldInterpolation.h
* @author       Thomas Schmitt
* @brief        This file provides declaration for the hold interpolation
* @license      Please refer to LICENSE.md
*//*
#pragma once
#include "Core/Core.h"
#include "Animation/Interpolation/AbstractInterpolation.h"

ULIS_NAMESPACE_BEGIN

template< typename T >
class THoldInterpolation : public TAbstractInterpolation< T >
{
protected:
    THoldInterpolation<T>();

    static THoldInterpolation<T>* mInstance = nullptr;

public:
    THoldInterpolation<T>(THoldInterpolation<T> &other) = delete;
    void operator=(const THoldInterpolation<T> &) = delete;
    static THoldInterpolation<T> *GetInstance();
    static void *ReleaseInstance();

public:
    virtual T Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey ) const override;
};

template< typename T >
THoldInterpolation<T>::THoldInterpolation() :
    TAbstractInterpolation<T>()
{

}

template< typename T >
THoldInterpolation<T>* THoldInterpolation<T>::GetInstance()
{
    if( mInstance == nullptr )
        mInstance = new THoldInterpolation<T>();
    return  mInstance;
}

template< typename T >
void * THoldInterpolation<T>::ReleaseInstance()
{
    if( mInstance != nullptr )
    {
        delete mInstance;
        mInstance = nullptr;
    }
}

template< typename T >
T THoldInterpolation<T>::Interpolate( ufloat iFrame, const TKey<T>& iLeftKey, const TKey<T>& iRightKey ) const
{
    return  iLeftKey.mValue;
}

ULIS_NAMESPACE_END

*/
