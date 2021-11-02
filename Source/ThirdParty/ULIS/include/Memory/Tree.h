// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Tree.h
* @author       Clement Berthaud
* @brief        This file provides the definition for a basic Tree.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Core/CallbackCapable.h"
#include "Memory/Array.h"
#include <functional>

ULIS_NAMESPACE_BEGIN
template< class Type > class TRoot;
template< class Type > class TNode;
// Not necessary per say to change _ID always, even when type changes, but to avoid future mistakes, good habit.
template< class Type > using TNodeAddedDelegate     = TLambdaCallback< void, const TRoot< Type >*, const TNode< Type >* >;
template< class Type > using TNodeRemovedDelegate   = TLambdaCallback< void, const TRoot< Type >*, const TNode< Type >*, bool >;
template< class Type > using TParentChangedDelegate = TLambdaCallback< void, const TNode< Type >*, const TRoot< Type >* >;
template< class Type > using TSelfChangedDelegate   = TLambdaCallback< void, const TNode< Type >* >;
template< class Type > using TOnNodeAdded       = TCallbackCapable< TNodeAddedDelegate< Type >, 0 >;
template< class Type > using TOnNodeRemoved     = TCallbackCapable< TNodeRemovedDelegate< Type >, 1 >;
template< class Type > using TOnParentChanged   = TCallbackCapable< TParentChangedDelegate< Type >, 2 >;
template< class Type > using TOnSelfChanged     = TCallbackCapable< TSelfChangedDelegate< Type >, 3 >;

/////////////////////////////////////////////////////
/// @class      TNode
/// @brief      Basic node
template< class Type >
class ULIS_API TNode
    : public TOnParentChanged< Type >
    , public TOnSelfChanged< Type >
{
    typedef TRoot< Type > tParent;
    friend class TRoot< Type >;

public:
    using InnerType = Type;

public:
    virtual ~TNode() {
        ULIS_DEBUG_PRINTF( "TNode Destroyed" )
    }

    TNode(
          const tParent* iParent = nullptr
        , const TOnParentChanged< Type >& iParentChangedDelegate = TOnParentChanged< Type >()
        , const TOnSelfChanged< Type >& iSelfChangedDelegate = TOnSelfChanged< Type >()
    )
        : TOnParentChanged< Type >( iParentChangedDelegate )
        , TOnSelfChanged< Type >( iSelfChangedDelegate )
        , mParent( iParent )
    {
        ULIS_DEBUG_PRINTF( "TNode Created" )
        TOnParentChanged< Type >::Invoke( this, mParent );
    }

    Type& Self() {
        //return  *reinterpret_cast< Type* >( __GetDerivedPtr__() ); // No RTTI
        return  *dynamic_cast< Type* >( this ); // Yes RTTI
    }

    const Type& Self() const {
        //return  *reinterpret_cast< const Type* >( __GetDerivedPtr__() ); // No RTTI
        return  *dynamic_cast< const Type* >( this ); // Yes RTTI
    }

    Type& Def( std::function< void( Type& ) > iFunc ) {
        iFunc( Self() );
        return  Self();
    }

    const tParent* Parent() const {
        return  mParent;
    }

    void SetParent( const tParent* iParent ) {
        mParent = iParent;
        TOnParentChanged< Type >::Invoke( this, mParent );
    }

    const tParent* TopLevelParent() const {
        if( mParent == nullptr )
            return  dynamic_cast< const tParent* >( this );

        const tParent* parent = mParent;
        while( parent->mParent )
            parent = parent->mParent;
        return  parent;
    }

    void NotifyChange() const {
        TOnSelfChanged< Type >::Invoke( this );
        if( mParent )
            mParent->NotifyChange();
    }

private:
    // In case RTTI is disabled this cas enable virtual dynamic casting
    // alternative for derived classes.
    /*
    #define ULIS_DERIVED_FROM_NODE                      \
        virtual void* __GetDerivedPtr__() override {    \
            return this;                                \
        }

    virtual void* __GetDerivedPtr__() = 0 {
        return nullptr;
    }
    */

protected:
    virtual void InitFromParent( const tParent* iParent ) {
    }

private:
    virtual void OnChangeNotifiedInternal() {
    }

private:
    const tParent* mParent;
};

/////////////////////////////////////////////////////
/// @class      TRoot
/// @brief      Abstract Root
template< class Type >
class ULIS_API TRoot
    : public virtual TNode< Type >
    , public TOnNodeAdded< Type >
    , public TOnNodeRemoved< Type >
{
    typedef TNode< Type >   tSuperClass;
    typedef TRoot< Type >   tParent;
    typedef TRoot< Type >   tSelf;
    typedef TNode< Type >   tNode;

public:
    using InnerType = Type;

public:
    virtual ~TRoot() override {
        for( uint64 i = 0; i < mChildren.Size(); ++i ) {
            TOnNodeRemoved< Type >::Invoke( this, mChildren[i], true );
            delete  mChildren[i];
        }
        mChildren.Clear();
        ULIS_DEBUG_PRINTF( "TRoot Destroyed" )
    }

    TRoot(
          tParent* iParent = nullptr
        , const TOnParentChanged< Type >& iParentChangedDelegate = TOnParentChanged< Type >()
        , const TOnSelfChanged< Type >& iSelfChangedDelegate = TOnSelfChanged< Type >()
        , const TOnNodeAdded< Type >& iNodeAddedDelegate = TOnNodeAdded< Type >()
        , const TOnNodeRemoved< Type >& iNodeRemovedDelegate = TOnNodeRemoved< Type >()
    )
        : tSuperClass(
              iParent
            , iParentChangedDelegate
            , iSelfChangedDelegate
        )
        , TOnNodeAdded< Type >( iNodeAddedDelegate )
        , TOnNodeRemoved< Type >( iNodeRemovedDelegate )
    {
        ULIS_DEBUG_PRINTF( "TRoot Created" )
    }

    void
    Reset(
          tParent* iParent = nullptr
        , const TOnParentChanged< Type >& iParentChangedDelegate = TOnParentChanged< Type >()
        , const TOnNodeAdded< Type >& iNodeAddedDelegate = TOnNodeAdded< Type >()
        , const TOnNodeRemoved< Type >& iNodeRemovedDelegate = TOnNodeRemoved< Type >()
    )
    {
        for( uint64 i = 0; i < mChildren.Size(); ++i ) {
            TOnNodeRemoved< Type >::Invoke( this, mChildren[i], false );
            delete  mChildren[i];
        }
        mChildren.Clear();

        tNode::mParent = iParent;
        TOnParentChanged< Type >::SetDelegate( iParentChangedDelegate );
        TOnNodeAdded< Type >::SetDelegate( iNodeAddedDelegate );
        TOnNodeRemoved< Type >::SetDelegate( iNodeRemovedDelegate );
    }

    TArray< tNode* >& Children() {
        return  mChildren;
    }

    const TArray< tNode* >& Children() const {
        return  mChildren;
    }

    Type& operator[]( uint64 iIndex ) {
        return  mChildren[ iIndex ]->Self();
    }

    const Type& operator[]( uint64 iIndex ) const {
        return  mChildren[ iIndex ]->Self();
    }

    void DeleteChild( int iIndex ) {
        ULIS_ASSERT( iIndex < mChildren.Size(), "Bad Index" );
        TOnNodeRemoved< Type >::Invoke( this, mChildren[ iIndex ], false );
        delete mChildren[ iIndex ];
        mChildren.Erase( iIndex, 1 );
    }

    tSelf& AddChild( tNode* iNode, uint64 iIndex = ULIS_UINT64_MAX ) {
        ULIS_ASSERT( iNode, "Bad input" )
        iNode->SetParent( this );
        iNode->InitFromParent( this );
        if( iIndex >= Children().Size() )
            Children().PushBack( iNode );
        else
            Children().Insert( iIndex, iNode );
        TOnNodeAdded< Type >::Invoke( this, iNode );
        return  *this;
    }

private:
    /*
    template< typename ChildType, class ... Args >
    tSelf& AddChild( Args&& ... args ) {
        return  AddChild( new ChildType( std::forward< Args >(args) ... ), ULIS_UINT64_MAX );
    }

    template< typename ChildType, class ... Args >
    tSelf& AddChild( uint64 iIndex, Args&& ... args ) {
        return  AddChild( new ChildType( std::forward< Args >(args) ... ), iIndex );
    }
    */

protected:
    virtual void InitFromParent( const tParent* iParent ) override {
        for( uint64 i = 0; i < mChildren.Size(); ++i ) {
            mChildren[i]->InitFromParent( this );
        }
    }

private:
    TArray< tNode* > mChildren;
};

ULIS_NAMESPACE_END

