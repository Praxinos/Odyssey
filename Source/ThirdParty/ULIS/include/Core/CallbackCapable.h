// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         CallbackCapable.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TCallbackCapable class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Core/Callback.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TCallbackCapable
/// @brief      The TCallbackCapable class provides a base abstract interface
///             to enable callback capability on PIC classes.
/// @details    Why is TCallbackCapable implemented as a class designed to be
///             inherited from:
///
///             The other solution is to make it a member of the classes that
///             need a delegate, this may seem simpler, but then it requires
///             more code to be able to change the delegate during the object
///             lifetime. This way, the object inherits the SetDelegate method.
///             You can still have more than one delegate, you just need to
///             inherit multiple times from the TCallbackCapable with a
///             different DelegateType. ( Although, most client want to inherit
///             TCallbackCapable privately.
///
///             In case you need multiple delegates of the same type, it is
///             forbidden to inherit directly multiple time from the same base.
///             So that's why the _ID arg is provided, it differentiates the
///             bases artificially and allows to have multiple same delegates
///             for different purposes.
///
///             For simplicity, a macro is provided below, see:
///                 - ULIS_DECLARE_SIMPLE_DELEGATE
///                 - ULIS_DECLARE_SIMPLE_DELEGATE_SPEC
///
///             it provides default template instanciation and export, as well
///             as a typedef such as "FOnValueChanged", which is intented to
///             be the main alias used to acces the TCallbackCapable part of
///             the PIC that uses it. In case of multiple TCallbackCapable,
///             Invoke() calls will be ambiguous and need to be prefixed by
///             the alias:: identifier.
///
///             No automatic wrappers are provided to automate callback calls,
///             because it is not possible to know in advance which delegate
///             must be called, or if it is necessary in all methods ( such as
///             getters ), and it would also be problematic in const methods.
///
///             TCallbackCapable is not supposed to be instanciated, but it's
///             still possible to do so if you want to chose to use it by
///             composition instead of inheritance, and for simplicity the
///             typedefed TCallbackCapable can be used directly as input
///             parameter in client classes and needs to be able to be default-
///             constructed as a default input parameter, which requires it to
///             be instanciable in global scope.
template< class DelegateType, int _ID = 0 >
class TCallbackCapable
{
public:
    using Delegate = DelegateType;

public:
    TCallbackCapable( const DelegateType& iDelegate = DelegateType() )
        : mDelegate( iDelegate )
    {}

    TCallbackCapable( typename DelegateType::tFptr iDelegateFPTR )
        : mDelegate( iDelegateFPTR )
    {}

    template< typename ... Args >
    ULIS_FORCEINLINE void Invoke( Args&& ... args ) const {
        mDelegate.ExecuteIfBound( std::forward< Args >(args)... );
    }

    void SetDelegate( const DelegateType& iDelegate ) {
        mDelegate = iDelegate;
    }

    void SetDelegate( const TCallbackCapable< DelegateType, _ID >& iOther ) {
        mDelegate = iOther.mDelegate;
    }

    void SetDelegate( typename DelegateType::tFptr iDelegateFPTR ) {
        mDelegate = DelegateType( iDelegateFPTR );
    }

    const DelegateType& GetDelegate() const {
        return  mDelegate;
    }

private:
    DelegateType mDelegate;
};

#define ULIS_DECLARE_SIMPLE_DELEGATE( __Name__, __Ret__, ... )                          \
    typedef TCallbackCapable< TLambdaCallback< __Ret__, __VA_ARGS__ > > __Name__;

#define ULIS_DECLARE_SIMPLE_DELEGATE_SPEC( __Name__, __Spec__, __Ret__, ... )                       \
    typedef TCallbackCapable< TLambdaCallback< __Ret__, __VA_ARGS__ >, __Spec__ > __Name__;

ULIS_NAMESPACE_END

