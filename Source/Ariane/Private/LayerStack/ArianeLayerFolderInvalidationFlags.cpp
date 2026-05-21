// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerFolderInvalidationFlags.h"

bool
FArianeLayerFolderInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClass );
}

FArianeLayerFolderInvalidationFlags&
FArianeLayerFolderInvalidationFlags::AND( const FArianeLayerInvalidationFlags& RHS )
{
    Super::AND( RHS );

    if( RHS.HasBaseClass( FArianeLayerFolderInvalidationFlags::StaticClass() ) )
    {
        Hierarchy  &= ((FArianeLayerFolderInvalidationFlags&)RHS).Hierarchy;
    }

    return *this;
}

FArianeLayerFolderInvalidationFlags&
FArianeLayerFolderInvalidationFlags::OR( const FArianeLayerInvalidationFlags& RHS )
{
    Super::OR( RHS );

    if( RHS.HasBaseClass( FArianeLayerFolderInvalidationFlags::StaticClass() ) )
    {
        Hierarchy  |= ((FArianeLayerFolderInvalidationFlags&)RHS).Hierarchy;
    }

    return *this;
}

FArianeLayerFolderInvalidationFlags&
FArianeLayerFolderInvalidationFlags::SetAll()
{
    Super::SetAll();

    Hierarchy = 1;

    return *this;
}

FArianeLayerFolderInvalidationFlags&
FArianeLayerFolderInvalidationFlags::Clear()
{
    Super::Clear();

    Hierarchy = 0;

    return *this;
}

bool
FArianeLayerFolderInvalidationFlags::HasAny()
{
    return Super::HasAny() || Hierarchy;
}
