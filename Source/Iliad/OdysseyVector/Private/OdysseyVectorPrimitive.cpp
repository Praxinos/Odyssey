// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(this->FOdysseyVectorPath::CopyShape( 0 ));

    this->CopySettings( path );

    return path;
}
