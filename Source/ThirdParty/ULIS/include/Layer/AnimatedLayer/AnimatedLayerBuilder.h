// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedLayerBuilder.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TAnimatedLayerBuilder class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/AnimatedLayer/AnimatedLayerTypedefs.h"
#include <functional>

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TAnimatedLayerBuilder
/// @brief      The TAnimatedLayerBuilder class provides a tool for building AnimatedLayerStacks
///             from code.
template< typename T >
class TAnimatedLayerBuilder {
public:

    TAnimatedLayerBuilder< T >( T* iElem )
        : m( iElem )
    {
    }

    template< class ... Args >
    static
    TAnimatedLayerBuilder
    Assign( T** ioElem, Args&& ... args ) {
        T* ptr = new T( std::forward< Args >( args ) ... );
        *ioElem = ptr;
        return  TAnimatedLayerBuilder( ptr );
    }

    template< class ... Args >
    static
    TAnimatedLayerBuilder
    Create( Args&& ... args ) {
        T* ptr = new T( std::forward< Args >( args ) ... );
        return  TAnimatedLayerBuilder( ptr );
    }

    TAnimatedLayerBuilder< T >& Def( std::function< void( T* ) > iFunc ) {
        iFunc( m );
        return  *this;
    }

    template< typename U >
    TAnimatedLayerBuilder< T >& AddChild( TAnimatedLayerBuilder< U >& iObj ) {
        m->AddChild( iObj.m );
        return  *this;
    }

    template< typename U >
    TAnimatedLayerBuilder< T >& operator[]( TAnimatedLayerBuilder< U >& iObj ) {
        m->AddChild( iObj.m );
        return  *this;
    }

    template< typename W, class ... Args >
    TAnimatedLayerBuilder< T >& operator()( W w, Args&& ... args ) {
        (m->*w)( args ... );
        return  *this;
    }

public:
    T* m;
};

#define ULAAssociateStack( _Elem_ )              TAnimatedLayerBuilder< FAnimatedLayerStack >( _Elem_ )
#define ULAAssignStack( _Elem_, ... )            TAnimatedLayerBuilder< FAnimatedLayerStack >::Assign( & _Elem_, __VA_ARGS__ )
#define ULACreateChild( _Class_, ... )           TAnimatedLayerBuilder< _Class_ >::Create( __VA_ARGS__ )
#define ULAAssignChild( _Class_, _Elem_, ... )   TAnimatedLayerBuilder< _Class_ >::Assign( & _Elem_, __VA_ARGS__ )
#define ULAAddLayer( _Elem_ )                    .AddChild( _Elem_ )
#define ULADef( ... )                            .Def( [&]( auto i ){ i-> __VA_ARGS__ ; } )
#define ULAPayload( ... )                        ULDef( Payload( __VA_ARGS__ ) )

ULIS_NAMESPACE_END

