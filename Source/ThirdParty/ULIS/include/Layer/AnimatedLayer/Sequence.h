// IDDN.FR.001.250001.005.S.P.2019.000.00000
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

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TSequence
/// @brief      Basic Animation Sequence of Cels
template< class Type >
class TSequence {

public:
    virtual ~TSequence() {
        Reset();
    }

    TSequence()
    {}

public:
    void
    Reset()
    {
        for( uint64 i = 0; i < mInstances.Size(); ++i )
            delete  mInstances[i];
        mInstances.Clear();
    }

    TArray< TCel< Type >* >& Instances() {
        return  mInstances;
    }

    const TArray< TCel< Type >* >& Instances() const {
        return  mInstances;
    }

private:
    TArray< TCel< Type >* > mInstances;
};

ULIS_NAMESPACE_END

