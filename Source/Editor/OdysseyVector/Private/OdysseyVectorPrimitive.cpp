#include "OdysseyVectorPrimitive.h"

FOdysseyVectorPrimitive::~FOdysseyVectorPrimitive()
{
}

FOdysseyVectorPrimitive::FOdysseyVectorPrimitive( const FString& iName )
    : FOdysseyVectorPathCubic( iName )
{
}

bool
FOdysseyVectorPrimitive::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPathCubic::HasBaseClass( iBaseClassID );
}
