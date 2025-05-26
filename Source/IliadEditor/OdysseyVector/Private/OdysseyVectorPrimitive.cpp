// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorPrimitive.h"

FOdysseyVectorPrimitive::~FOdysseyVectorPrimitive()
{
}

FOdysseyVectorPrimitive::FOdysseyVectorPrimitive( const FString& iName )
    : FOdysseyVectorPath( iName )
{
}

bool
FOdysseyVectorPrimitive::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPath::HasBaseClass( iBaseClassID );
}

FOdysseyVectorPath*
FOdysseyVectorPrimitive::Convert()
{
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(this->FOdysseyVectorPath::CopyShape( 0 ) );

    this->CopySettings( path, 0 );

    return path;
}
