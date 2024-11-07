// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Searchable.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TSearchable class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Layer/Layer.h"
#include "String/Utils.h"
#include <algorithm>
#include <tuple>
#include <vector>

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TSearchable
/// @brief      Simple Searchable class.
template< class RootType >
class TSearchable
{
    using InnerType = typename RootType::InnerType;

public:
    TSearchable() {
        ULIS_DEBUG_PRINTF( "TSearchable Created" )
    }

    virtual ~TSearchable() = 0;

    InnerType& operator[]( const FString& iStr ) {
        RootType& self = dynamic_cast< RootType& >( *this );
        const uint64 size = self.Children().Size();
        std::vector< std::tuple< int, InnerType* > > matches;
        matches.reserve( size );
        for( uint64 i = 0; i < size; ++i ) {
            InnerType* layer = &self[i];
            const FString& name = layer->Name();
            uint32 typeID = self[i].TypeID();
            int dist = static_cast< int >( LevenshteinDistance( iStr.Data(), iStr.Size(), name.Data(), name.Size() ) );
            matches.push_back( std::tuple< int, InnerType* >( dist, layer ) );
        }
        std::sort( matches.begin(), matches.end() );
        return matches.size() ? *std::get< 1 >( matches[0] ) : self.Self();
    }

    const InnerType& operator[]( const FString& iStr ) const {
        const RootType& self = dynamic_cast< const RootType& >( *this );
        const uint64 size = self.Children().Size();
        std::vector< std::tuple< int, const InnerType* > > matches;
        matches.reserve( size );
        for( uint64 i = 0; i < size; ++i ) {
            const InnerType* layer = &self[i];
            const FString& name = layer->Name();
            uint32 typeID = self[i].TypeID();
            int dist = static_cast< int >( LevenshteinDistance( iStr.Data(), iStr.Size(), name.Data(), name.Size() ) );
            matches.push_back( std::tuple< int, const InnerType* >( dist, layer ) );
        }
        std::sort( matches.begin(), matches.end() );
        return matches.size() ? *std::get< 1 >( matches[0] ) : self.Self();
    }


    template< typename T >
    T& Find( const FString& iName ) {
        return  dynamic_cast< T& >( (*this)[ iName ] );
    }

    template< typename T >
    const T& Find( const FString& iName ) const {
        return  dynamic_cast< const T& >( (*this)[ iName ] );
    }
};

template< class RootType >
TSearchable<RootType>::~TSearchable()
{
    ULIS_DEBUG_PRINTF( "TSearchable Destroyed" )
}

ULIS_NAMESPACE_END

