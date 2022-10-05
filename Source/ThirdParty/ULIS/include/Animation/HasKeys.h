// IDDN.FR.001.250001.006.S.P.2019.000.00000
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
#include "Math/Geometry/Vector.h"
#include "Animation/Interpolation/AbstractInterpolation.h"
#include "Animation/Interpolation/LinearInterpolation.h"
#include "Animation/Interpolation/HoldInterpolation.h"
#include "Animation/Interpolation/BezierInterpolation.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN

template< typename T >
struct TKey
{
    TKey( ufloat iFrame, T iValue, TAbstractInterpolation<T>* iInterpolation );
    ~TKey();

    TKey<T>& operator=(const TKey<T>& iOther) { mFrame = iOther.mFrame; mValue = iOther.mValue; mInterpolationType = iOther.mInterpolationType; mInterpolation = iOther.GetInterpolation()->Clone(); return *this; }
    TKey<T>(const TKey<T>& iOther) { mFrame = iOther.mFrame; mValue = iOther.mValue; mInterpolationType = iOther.mInterpolationType; mInterpolation = iOther.GetInterpolation()->Clone(); }

    TAbstractInterpolation<T>* GetInterpolation() const;
    void SetInterpolationFromType( eInterpolationType iType );
    eInterpolationType GetInterpolationType() const;

    ufloat mFrame;
    T mValue;

private:
    eInterpolationType mInterpolationType;
    TAbstractInterpolation<T>* mInterpolation;
};

template< typename T >
TKey<T>::TKey( ufloat iFrame, T iValue, TAbstractInterpolation<T>* iInterpolation ):
    mFrame(iFrame),
    mValue(iValue),
    mInterpolation(iInterpolation)
{
    if( dynamic_cast<TLinearInterpolation<T>*>( mInterpolation ) != nullptr )
        mInterpolationType = eInterpolationType::kLinear;
    else if (dynamic_cast<THoldInterpolation<T>*>(mInterpolation) != nullptr)
        mInterpolationType = eInterpolationType::kHold;
    else
        mInterpolationType = eInterpolationType::kBezier;
}

template< typename T >
TKey<T>::~TKey()
{
    if( mInterpolation )
        delete mInterpolation;
}

template< typename T >
void TKey<T>::SetInterpolationFromType( eInterpolationType iType )
{
    delete mInterpolation;
    switch(iType)
    {
        case eInterpolationType::kLinear:
            mInterpolation = new TLinearInterpolation<T>();
        break;
        case eInterpolationType::kHold:
            mInterpolation = new THoldInterpolation<T>();
        break;
        case eInterpolationType::kBezier:
            mInterpolation = new TBezierInterpolation<T>();
        break;
    }
    mInterpolationType = iType;
}

template< typename T >
TAbstractInterpolation<T>* TKey<T>::GetInterpolation() const
{
    return mInterpolation;
}

template< typename T >
eInterpolationType TKey<T>::GetInterpolationType() const
{
    return mInterpolationType;
}

/** Returns the whole array of keys(AFTER the addition), the index at which we added the key, and a reference to the key we added */
template< class Type > using TKeyAddedDelegate = TLambdaCallback< void, ::ULIS::TArray<TKey<Type>>&, uint64, TKey<Type>& >;

/** Returns the whole array of keys(BEFORE the deletion), the index at which we deleted the key, and a reference to the key we deleted */
template< class Type > using TKeyRemovedDelegate = TLambdaCallback< void, ::ULIS::TArray<TKey<Type>>&, uint64, TKey<Type>& >;

/** Returns a copy of the key before the change, and a reference to the changed key */
template< class Type > using TKeyChangedDelegate = TLambdaCallback< void, TKey<Type>, TKey<Type>& >;

template< class Type > using TOnKeyAdded = TCallbackCapable< TKeyAddedDelegate< Type >, 0 >;
template< class Type > using TOnKeyRemoved = TCallbackCapable< TKeyRemovedDelegate< Type >, 1 >;
template< class Type > using TOnKeyChanged = TCallbackCapable< TKeyChangedDelegate< Type > >;

template< typename T >
class THasKeys:
    public TOnKeyAdded<T>,
    public TOnKeyRemoved<T>,
    public TOnKeyChanged<T>
{
public:
    THasKeys();
    ~THasKeys();

public:

    /** Adds a key in Keys Array, replaces existing key if there was already one at the same frame*/
    virtual void AddOrReplaceKey( TKey<T>& iKey );

    /** Removes a key at a certain frame. Return true if there was one at the frame passed in parameter */
    virtual bool RemoveKeyAtFrame( ufloat iFrame );

    /** Const getter, useful to check or display the keys, but no modification is allowed */
    const ::ULIS::TArray<TKey<T>>& GetKeys() const;

private:
    ::ULIS::TArray<TKey<T>> mKeys;
};

template< typename T >
THasKeys<T>::THasKeys()
{
}

template< typename T >
THasKeys<T>::~THasKeys()
{

}

template< typename T >
void THasKeys<T>::AddOrReplaceKey(TKey<T>& iKey)
{
    if(mKeys.Size() == 0)
    {
        mKeys.PushBack(iKey);
        TOnKeyAdded<T>::Invoke( mKeys, 0, mKeys[0] );
        return;
    }

    if(mKeys[0].mFrame == iKey.mFrame)
    {
        TKey<T> beforeChangeKey = mKeys[0];
        mKeys[0] = iKey;
        TOnKeyChanged<T>::Invoke( beforeChangeKey, mKeys[0]);
        return;
    }
    else if(mKeys[mKeys.Size() - 1].mFrame == iKey.mFrame)
    {
        TKey<T> beforeChangeKey = mKeys[mKeys.Size() - 1];
        mKeys[mKeys.Size() - 1] = iKey;
        TOnKeyChanged<T>::Invoke( beforeChangeKey, mKeys[mKeys.Size() - 1] );
        return;
    }

    if(mKeys[0].mFrame > iKey.mFrame)
    {
        mKeys.Insert(0, iKey);
        TOnKeyAdded<T>::Invoke( mKeys, 0, mKeys.Front() );
        return;
    }
    else if(mKeys[mKeys.Size() - 1].mFrame < iKey.mFrame)
    {
        mKeys.PushBack(iKey);
        TOnKeyAdded<T>::Invoke( mKeys, mKeys.Size() - 1, mKeys.Back() );
        return;
    }

    //Dichotomy to search for the index at which we want to insert our key
    int leftKeyIndex = 0;
    int rightKeyIndex = mKeys.Size() - 1;

    while(rightKeyIndex - leftKeyIndex > 1)
    {
        int searchIndex = (leftKeyIndex + rightKeyIndex) / 2;
        
        if(mKeys[searchIndex].mFrame == iKey.mFrame)
        {
            TKey<T> beforeChangeKey = mKeys[searchIndex];
            mKeys[searchIndex] = iKey;
            TOnKeyChanged<T>::Invoke( beforeChangeKey, mKeys[searchIndex]);
            return;
        }

        if(mKeys[searchIndex].mFrame > iKey.mFrame)
        {
            rightKeyIndex = searchIndex;
        }
        else
        {
            leftKeyIndex = searchIndex;
        }
    }

    mKeys.Insert(rightKeyIndex, iKey);
    TOnKeyAdded<T>::Invoke( mKeys, rightKeyIndex, mKeys[rightKeyIndex] );
}

template< typename T >
bool THasKeys<T>::RemoveKeyAtFrame( ufloat iFrame )
{
    if(mKeys.Size() == 0)
        return  false;

    if(mKeys[0].mFrame == iFrame)
    {
        TOnKeyRemoved<T>::Invoke( mKeys, 0, mKeys[0] );
        mKeys.Erase(0);
        return  true;
    }
    else if(mKeys[mKeys.Size() - 1].mFrame == iFrame)
    {
        TOnKeyRemoved<T>::Invoke( mKeys, mKeys.Size() - 1, mKeys[mKeys.Size() - 1] );
        mKeys.PopBack();
        return  true;
    }

    //Dichotomy to search for the index at which we want to remove our key
    int leftKeyIndex = 0;
    int rightKeyIndex = mKeys.Size() - 1;

    while(rightKeyIndex - leftKeyIndex > 1)
    {
        int searchIndex = (leftKeyIndex + rightKeyIndex) / 2;

        if(mKeys[searchIndex].mFrame == iFrame)
        {
            TOnKeyRemoved<T>::Invoke( mKeys, searchIndex, mKeys[searchIndex] );
            mKeys.Erase(searchIndex);
            return  true;
        }

        if(mKeys[searchIndex].mFrame > iFrame)
        {
            rightKeyIndex = searchIndex;
        }
        else
        {
            leftKeyIndex = searchIndex;
        }
    }

    return  false;
}

template< typename T >
const TArray<TKey<T>>& THasKeys<T>::GetKeys() const
{
    return  mKeys;
}

ULIS_NAMESPACE_END

