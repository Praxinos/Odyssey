// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedProperty.h
* @author       Thomas Schmitt
* @brief        This file provides declaration for animating a property
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "DefaultValue.h"
#include "Animation/HasKeys.h"
#include "Animation/Interpolation/AbstractInterpolation.h"

ULIS_NAMESPACE_BEGIN

template< typename T >
class TAnimatedProperty : public THasKeys<T>
{
public:
    TAnimatedProperty( T iDefaultValue );
    TAnimatedProperty( std::function< T( ufloat ) > iDefaultExpression );
    ~TAnimatedProperty();

public:
    virtual T GetValueAtFrame( ufloat iFrame ) const;

protected:
    TDefaultValue<T> mTrueDefaultValue;
};

template< typename T >
TAnimatedProperty<T>::TAnimatedProperty(T iDefaultValue) :
    mTrueDefaultValue( TDefaultValue<T>::FromValue( iDefaultValue ) )
{
}

template< typename T >
TAnimatedProperty<T>::TAnimatedProperty( std::function< T( ufloat ) > iDefaultExpression ) :
    mTrueDefaultValue( TDefaultValue<T>::FromExpression( iDefaultExpression ))
{
}

template< typename T >
TAnimatedProperty<T>::~TAnimatedProperty()
{

}

template< typename T >
T TAnimatedProperty<T>::GetValueAtFrame( ufloat iFrame ) const
{
    if(this->GetKeys().Size() == 0)
        return  mTrueDefaultValue(0.f);

    if( this->GetKeys()[0].mFrame >= iFrame )
        return  this->GetKeys()[0].mValue;
    else if( this->GetKeys()[this->GetKeys().Size() - 1].mFrame <= iFrame )
        return  this->GetKeys()[this->GetKeys().Size() - 1].mValue;

    int leftKeyIndex = 0;
    int rightKeyIndex = this->GetKeys().Size() - 1;

    //Dichotomy to search for the successive keys to interpolate between them
    while(rightKeyIndex - leftKeyIndex > 1)
    {
        int searchIndex = (leftKeyIndex + rightKeyIndex) / 2;
        if(this->GetKeys()[searchIndex].mFrame > iFrame)
        {
            rightKeyIndex = searchIndex;
        }
        else
        {
            leftKeyIndex = searchIndex;
        }
    }

    return  this->GetKeys()[leftKeyIndex].GetInterpolation()->Interpolate( iFrame, this->GetKeys()[leftKeyIndex], this->GetKeys()[rightKeyIndex] );
}


ULIS_NAMESPACE_END

