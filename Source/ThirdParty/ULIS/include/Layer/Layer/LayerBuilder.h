// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerBuilder.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TLayerBuilder class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Layer/LayerTypedefs.h"
#include <functional>

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TLayerBuilder
/// @brief      The TLayerBuilder class provides a tool for building layerstacks
///             from code.
template< typename T >
class TLayerBuilder {
public:

    TLayerBuilder< T >( T* iElem )
        : m( iElem )
    {
    }

    template< class ... Args >
    static
    TLayerBuilder
    Assign( T** ioElem, Args&& ... args ) {
        T* ptr = new T( std::forward< Args >( args ) ... );
        *ioElem = ptr;
        return  TLayerBuilder( ptr );
    }

    template< class ... Args >
    static
    TLayerBuilder
    Create( Args&& ... args ) {
        T* ptr = new T( std::forward< Args >( args ) ... );
        return  TLayerBuilder( ptr );
    }

    TLayerBuilder< T >& Def( std::function< void( T* ) > iFunc ) {
        iFunc( m );
        return  *this;
    }

    template< typename U >
    TLayerBuilder< T >& AddChild( TLayerBuilder< U >& iObj ) {
        m->AddChild( iObj.m );
        return  *this;
    }

    template< typename U >
    TLayerBuilder< T >& operator[]( TLayerBuilder< U >& iObj ) {
        m->AddChild( iObj.m );
        return  *this;
    }

    template< typename W, class ... Args >
    TLayerBuilder< T >& operator()( W w, Args&& ... args ) {
        (m->*w)( args ... );
        return  *this;
    }

public:
    T* m;
};

#define ULAssociateStack( _Elem_ )              TLayerBuilder< FLayerStack >( _Elem_ )
#define ULAssignStack( _Elem_, ... )            TLayerBuilder< FLayerStack >::Assign( & _Elem_, __VA_ARGS__ )
#define ULCreateChild( _Class_, ... )           TLayerBuilder< _Class_ >::Create( __VA_ARGS__ )
#define ULAssignChild( _Class_, _Elem_, ... )   TLayerBuilder< _Class_ >::Assign( & _Elem_, __VA_ARGS__ )
#define ULAddLayer( _Elem_ )                    .AddChild( _Elem_ )
#define ULDef( ... )                            .Def( [&]( auto i ){ i-> __VA_ARGS__ ; } )
#define ULPayload( ... )                        ULDef( Payload( __VA_ARGS__ ) )

ULIS_NAMESPACE_END

