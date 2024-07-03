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
