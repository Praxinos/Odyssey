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
    , OwnerID( nullptr )
    , AllocationModel ( EArianeAllocationModel::InstancedStruct )
    , Flags( 0 )
    , bShared( false )
{
}

FArianeTag::FArianeTag( FArianeObject* InOwnerObject, EArianeAllocationModel InAllocationModel )
    : Guid ( FGuid::NewGuid() )
    , OwnerID( InOwnerObject )
    , AllocationModel( InAllocationModel )
    , Flags( 0 )
    , bShared( false )
{
}

EArianeAllocationModel
FArianeTag::GetAllocationModel()
{
    return AllocationModel;
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
    return OwnerID.GetObject();
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
