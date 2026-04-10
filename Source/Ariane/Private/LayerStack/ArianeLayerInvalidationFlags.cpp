// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerInvalidationFlags.h"

bool
FArianeLayerInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return false;
}

FArianeLayerInvalidationFlags&
FArianeLayerInvalidationFlags::AND( const FArianeLayerInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeLayerInvalidationFlags::StaticClass() ) )
    {
        Selected  &= ((FArianeLayerInvalidationFlags&)RHS).Selected;
        Transform &= ((FArianeLayerInvalidationFlags&)RHS).Transform;
    }

    return *this;
}

FArianeLayerInvalidationFlags&
FArianeLayerInvalidationFlags::OR( const FArianeLayerInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeLayerInvalidationFlags::StaticClass() ) )
    {
        Selected  |= ((FArianeLayerInvalidationFlags&)RHS).Selected;
        Transform |= ((FArianeLayerInvalidationFlags&)RHS).Transform;
    }

    return *this;
}

FArianeLayerInvalidationFlags&
FArianeLayerInvalidationFlags::SetAll()
{
    Selected  =
    Transform = 1;

    return *this;
}

FArianeLayerInvalidationFlags&
FArianeLayerInvalidationFlags::Clear()
{
    Selected  =
    Transform = 0;

    return *this;
}

bool
FArianeLayerInvalidationFlags::HasAny()
{
    return ( Selected
          || Transform );
}
