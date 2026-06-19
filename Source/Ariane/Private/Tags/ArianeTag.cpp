// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeTag.h"
#include "ArianeObject.h"
#include "ArianeLayer.h"

FArianeTag::~FArianeTag()
{
}

FArianeTag::FArianeTag()
    : Guid ( FGuid::NewGuid() )
    , Owner( nullptr )
    , Flags( 0 )
    , bShared( false )
{
}

FArianeTag::FArianeTag( FArianeObject* InOwnerObject )
    : Guid ( FGuid::NewGuid() )
    , Owner( InOwnerObject )
    , Flags( 0 )
    , bShared( false )
{
}

const FGuid&
FArianeTag::GetGuid()
{
    return Guid;
}

void
FArianeTag::Draw( double iAncestorsOpacity, uint64 iDrawingFlags )
{
}

void
FArianeTag::Update( uint32 UpdateFlags, FArianeObjectInvalidationFlags& OwnerInvalidationFlags )
{
}

FArianeTag*
FArianeTag::Copy( FArianeObject* InOwnerObject )
{
    return nullptr;
}

FArianeObject*
FArianeTag::GetOwner()
{
    return Owner;
}

void
FArianeTag::ObjectAdded()
{
}

void
FArianeTag::ObjectRemoved()
{
}

void
FArianeTag::Added()
{
}

void
FArianeTag::Removed()
{
}
