// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedProperty.h
* @author       Thomas Schmitt
* @brief        This file provides declaration for animating a property in the time
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Animation/AnimatedProperty.h"

ULIS_NAMESPACE_BEGIN

template< typename T >
class TBoundedAnimatedProperty : public TAnimatedProperty<T>
{
public:
    TBoundedAnimatedProperty( T iDefaultValue, T iMinValue, T iMaxValue );
    ~TBoundedAnimatedProperty();

public:
    //We can add keys that have a value outside the bounds of MinValue;MaxValue. We just clamp the value we get at frame iFrame
    virtual T GetValueAtFrame( ufloat iFrame ) const override;

    virtual void AddOrReplaceKey(TKey<T>& iKey) override;

public:
    T GetMinValue() const;
    T GetMaxValue() const;

protected:
    T mMinValue;
    T mMaxValue;
};

template< typename T >
TBoundedAnimatedProperty<T>::TBoundedAnimatedProperty(T iDefaultValue, T iMinValue, T iMaxValue) :
    TAnimatedProperty(iDefaultValue)
{
    if(iMinValue > iMaxValue)
    {
        mMinValue = iMaxValue;
        mMaxValue = iMinValue;
    }
    else
    {
        mMinValue = iMinValue;
        mMaxValue = iMaxValue;
    }
}

template< typename T >
TBoundedAnimatedProperty<T>::~TBoundedAnimatedProperty()
{

}

template< typename T >
T TBoundedAnimatedProperty<T>::GetValueAtFrame( ufloat iFrame ) const
{
    T value = TAnimatedProperty<T>::GetValueAtFrame( iFrame );
    return  FMath::Clamp( value, mMinValue, mMaxValue );
}

template< typename T >
void TBoundedAnimatedProperty<T>::AddOrReplaceKey( TKey<T>& iKey )
{
    iKey.mValue = FMath::Clamp( iKey.mValue, mMinValue, mMaxValue );
    THasKeys<T>::AddOrReplaceKey( iKey );
}

template< typename T >
T TBoundedAnimatedProperty<T>::GetMinValue() const
{
    return  mMinValue;
}

template< typename T >
T TBoundedAnimatedProperty<T>::GetMaxValue() const
{
    return  mMaxValue;
}

ULIS_NAMESPACE_END

