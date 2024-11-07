// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerUtils.h
* @author       Clement Berthaud
* @brief        This file provides utilities for layers.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
/*
ILayer& FindLayerByFuzzyNameInContainer( const FString& iStr, TRoot< ILayer >& iRoot );
const ILayer& FindLayerByFuzzyNameInContainer( const FString& iStr, const TRoot< ILayer >& iRoot );
IAnimatedLayer& FindLayerByFuzzyNameInContainer( const FString& iStr, IAnimatedLayerRoot& iRoot );
const IAnimatedLayer& FindLayerByFuzzyNameInContainer( const FString& iStr, const IAnimatedLayerRoot& iRoot );
*/

// PIC Helper macros
// PIC stands for Pseudo-Interface-Composition [class]
// Example Usage:
// ULIS_DECLARE_PIC_SIMPLE( IHasLock, bool, false, IsLocked, SetLocked ) // ( Header.h )
// ULIS_DEFINE_PIC_SIMPLE( IHasLock, bool, IsLocked, SetLocked ) // ( Source.cpp )

#define ULIS_DECLARE_PIC_SIMPLE( __Class__, __Type__, __Default__, __Getter__, __Setter__ ) \
    ULIS_DECLARE_SIMPLE_DELEGATE( FOn_ ## __Type__ ## _Changed, void, __Type__ )            \
    class ULIS_API __Class__                                                                \
        : public FOn_ ## __Type__ ## _Changed                                               \
    {                                                                                       \
        protected:                                                                          \
            __Class__ (                                                                     \
                  __Type__ i = __Default__                                                  \
                , const FOn_ ## __Type__ ## _Changed & d = FOn_ ## __Type__ ## _Changed ()  \
            );                                                                              \
        public:                                                                             \
            __Type__ __Getter__ () const;                                                   \
            void __Setter__ ( __Type__ i );                                                 \
        private:                                                                            \
            __Type__ m;                                                                     \
    };

#define ULIS_DEFINE_PIC_SIMPLE( __Class__, __Type__, __Getter__, __Setter__ )       \
    __Class__ :: __Class__ (                                                        \
          __Type__ i                                                                \
        , const FOn_ ## __Type__ ## _Changed & d                                    \
    )                                                                               \
        : FOn_ ## __Type__ ## _Changed( d )                                         \
        , m( i ) {}                                                                 \
    __Type__ __Class__ :: __Getter__ () const { return  m; }                        \
    void __Class__ :: __Setter__ ( __Type__ i ) { m = i; Invoke( m ); }

ULIS_NAMESPACE_END

