// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Queue.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the TQueue class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/ForwardList.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TQueue
/// @brief      The TQueue class provides a simple dynamic queue based on a
///             forward list.
/// @details    The need of a custom TQueue class rose from the fact
///             that std:: classes cannot be exported easily accross dll
///             boundaries when ULIS is compiled as a shared library.
template< typename T >
class TQueue
    : private TForwardList< T >
{
    typedef TForwardList< T >   tSuperClass;
    typedef TQueue< T >         tSelf;

public:
    /*! Destroy the queue and cleanup memory. */
    ~TQueue< T >()
    {}

    /*! Default constructor ( empty ). */
    TQueue< T >()
        : tSuperClass()
    {}

    /*! Copy constructor, explicitly removed. */
    TQueue< T >( const TQueue< T >& iOther ) = delete;

    /*! Copy Assignment Operator, explicitly removed. */
    TQueue< T >& operator=( const TQueue< T >& iOther ) = delete;

    /*! Move Assignment Operator, explicitly removed. */
    TQueue< T >& operator=( TQueue< T >&& iOther ) = delete;

    /*!
        Move constructor.
        The first object is left in a valid but empty state.
        The second object steals the state.
    */
    TQueue< T >( TQueue< T >&& iOther )
        : tSuperClass( std::forward< tSelf >( iOther ) )
    {}

    /*!
        Access component at front.
        The behaviour is undefined if the queue is empty.
    */
    T& Front() {
        return  tSuperClass::Front();
    }

    /*!
        Access const component at front.
        The behaviour is undefined if the queue is empty.
    */
    const T& Front() const {
        return  tSuperClass::Front();
    }

    /*!
        Access component at back.
        The behaviour is undefined if the queue is empty.
    */
    T& Back() {
        return  tSuperClass::Back();
    }

    /*!
        Access const component at back.
        The behaviour is undefined if the queue is empty.
    */
    const T& Back() const {
        return  tSuperClass::Back();
    }

    /*! Returns wether the queue is empty or not. */
    bool IsEmpty() const {
        return  tSuperClass::IsEmpty();
    }

    /*! Returns the queue size. */
    uint64 Size() const {
        return  tSuperClass::Size();
    }

    /*!
        Clear the queue, deallocating everything.
    */
    void Clear() {
        tSuperClass::Clear();
    }

    /*!
        Push, insert a new element at the end of the queue.
    */
    void Push( const T& iValue ) {
        tSuperClass::PushBack( iValue );
    }

    /*!
        Push, insert a new element at the end of the queue.
    */
    void Push( T&& iValue ) {
        tSuperClass::PushBack( std::forward< T >( iValue ) );
    }

    /*!
        Emplace, emplace a new element at end of the queue.
    */
    template< class... Args >
    void Emplace( Args&& ... args ) {
        tSuperClass::EmplaceBack( std::forward< Args >(args)... );
    }

    /*!
        Pop, pop the front element.
        The behaviour is undefined is the queue is empty.
    */
    void Pop() {
        tSuperClass::PopFront();
    }
};

ULIS_NAMESPACE_END

