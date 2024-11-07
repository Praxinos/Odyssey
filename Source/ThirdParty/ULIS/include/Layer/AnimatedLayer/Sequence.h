// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Sequence.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for a basic animation Sequence.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/AnimatedLayer/Cel.h"
#include "Animation/AnimatedProperty.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TSequence
/// @brief      Basic Animation Sequence of Cels
template< class T, class CelTypeFactory >
class TSequence
    : public CelTypeFactory
{

public:
    virtual ~TSequence() {
        Reset();
    }

    template< typename ... Args >
    TSequence( Args&& ... args )
        : CelTypeFactory( std::forward< Args >(args)... )
        , mInstances()
        , mTimeRemap( []( ufloat iFrame ){ return  iFrame; } )
    {}

public:
    void
    Reset()
    {
        for( uint64 i = 0; i < mInstances.Size(); ++i )
            delete  mInstances[i];
        mInstances.Clear();
    }

    TArray< TCel< T >* >& Instances() {
        return  mInstances;
    }

    const TArray< TCel< T >* >& Instances() const {
        return  mInstances;
    }

    void PushNewCel() {
        mInstances.PushBack( CelTypeFactory::MakeNew() );
    }

    void PopCel() {
        if( mInstances.Size() == 0 )
            return;

        delete  mInstances.Back();
        mInstances.PopBack();
    }

    void InsertNewCelAtFrame( uint64 iFrame ) {
        mInstances.Insert( IndexAtFrame( iFrame ), CelTypeFactory::MakeNew() );
    }

    void InsertBlankCelAtFrame( uint64 iFrame ) {
        mInstances.Insert( IndexAtFrame( iFrame ), CelTypeFactory::MakeBlank() );
    }
    void InsertSharedResourceCelAtFrame( uint64 iFrame, TCel< T >* iRefCel ) {
        mInstances.Insert( IndexAtFrame( iFrame ), CelTypeFactory::MakeSharedFrom( iRefCel ) );
    }

    void RemoveCelAtFrame( uint64 iFrame ) {
        const uint64 index = IndexAtFrame( iFrame );
        delete mInstances[index];
        mInstances.Erase( index );
    }

    TCel< T >* CelAtFrame( uint64 iFrame, bool iUseTimeRemap = false ) {
        if( iUseTimeRemap ) {
            const uint64 frame = static_cast< uint64> ( FMath::Clamp( mTimeRemap.GetValueAtFrame( static_cast< ufloat >( iFrame ) ), 0.f, float( mInstances.Size() - 1 ) ) );
            return  CelAtIndex( IndexAtFrame( frame ) );
        } else {
            return  CelAtIndex( IndexAtFrame( iFrame ) );
        }
    }

    const TCel< T >* CelAtFrame( uint64 iFrame, bool iUseTimeRemap = false ) const {
        if( iUseTimeRemap ) {
            const uint64 frame = static_cast< uint64> ( FMath::Clamp( mTimeRemap.GetValueAtFrame( static_cast< ufloat >( iFrame ) ), 0.f, float( mInstances.Size() - 1 ) ) );
            return  CelAtIndex( IndexAtFrame( frame ) );
        } else {
            return  CelAtIndex( IndexAtFrame( iFrame ) );
        }
    }

    uint64 TotalFrames() const {
        // Assume start at 0
        const uint64 size = mInstances.Size();
        uint64 counter = 0;
        for( uint64 i = 0; i < size; ++i )
            counter += mInstances[i]->Exposure() + 1;
        return  counter;
    }

    TAnimatedProperty< ufloat >& GetTimeRemapProperty() {
        return  mTimeRemap;
    }

    const TAnimatedProperty< ufloat >& GetTimeRemapProperty() const {
        return  mTimeRemap;
    }

private:
    TCel< T >* CelAtIndex( uint64 iIndex ) {
        return  mInstances[iIndex];
    }

    const TCel< T >* CelAtIndex( uint64 iIndex ) const {
        return  mInstances[iIndex];
    }
     
    uint64 IndexAtFrame( uint64 iFrame ) const {
        const uint64 size = mInstances.Size();
        uint64 counter = 0;
        for( uint64 i = 0; i < size; ++i ) {
            const uint64 exposure = mInstances[i]->Exposure();
            if( counter + exposure >= iFrame )
                return  i;
            counter += exposure + 1;
        }
        return  0; // Shut warning C4715
    }

private:
    TArray< TCel< T >* > mInstances;
    TAnimatedProperty< ufloat > mTimeRemap;
};

ULIS_NAMESPACE_END

