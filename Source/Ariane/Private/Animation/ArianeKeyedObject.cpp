// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeKeyedObject.h"
#include "ArianeObject.h"

FArianeKeyedObject::~FArianeKeyedObject()
{
}

FArianeKeyedObject::FArianeKeyedObject()
{
}

FArianeKeyedObject::FArianeKeyedObject( FArianeObject* Object )
    : Guid( Object->GetGuid() )
{
}

const FGuid&
FArianeKeyedObject::GetGuid() const
{
    return Guid;
}

void
FArianeKeyedObject::PostLoad()
{
}

void
FArianeKeyedObject::PostEditUndo()
{
}
