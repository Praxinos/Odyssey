// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeObject.h"
#include "StructUtils/InstancedStruct.h"
#include "ArianePainting3DComponent.h"

void
FArianeInvalidationFlags::AND( FArianeInvalidationFlags& Result
                             , const FArianeInvalidationFlags& LHS
                             , const FArianeInvalidationFlags& RHS )
{
    uint8* MemRes = (uint8*)&Result;
    const uint8* MemLHS = (uint8*)&LHS;
    const uint8* MemRHS = (uint8*)&RHS;

    for( uint32 i = 0; i < Result.GetSize(); i++ )
    {
        uint8 MemLHSValue = ( i < LHS.GetSize() ) ? MemLHS[i] : 0;
        uint8 MemRHSValue = ( i < RHS.GetSize() ) ? MemRHS[i] : 0;

        MemRes[i] = MemLHSValue & MemRHSValue;
    }
}

void
FArianeInvalidationFlags::OR( FArianeInvalidationFlags& Result
                            , const FArianeInvalidationFlags& LHS
                            , const FArianeInvalidationFlags& RHS )
{
    uint8* MemRes = (uint8*)&Result;
    const uint8* MemLHS = (uint8*)&LHS;
    const uint8* MemRHS = (uint8*)&RHS;

    for( uint32 i = 0; i < Result.GetSize(); i++ )
    {
        uint8 MemLHSValue = ( i < LHS.GetSize() ) ? MemLHS[i] : 0;
        uint8 MemRHSValue = ( i < RHS.GetSize() ) ? MemRHS[i] : 0;

        MemRes[i] = MemLHS[i] | MemRHS[i];
    }
}

void
FArianeInvalidationFlags::Clear()
{
    FMemory::Memzero( this, GetSize() );
}





FArianeObject::~FArianeObject()
{
}

FArianeObject::FArianeObject()
    : Painting3DComponent( nullptr )
    , Guid ( FGuid::NewGuid() )
    , ParentID ()
    , InvalidationFlags ( new FArianeObjectInvalidationFlags() )
{
}

FArianeObject::FArianeObject( UArianePainting3DComponent* InPainting3DComponent )
    : FArianeObject()
{
    Painting3DComponent = InPainting3DComponent;
}

void
FArianeObject::AppendChild( FArianeObject* Child )
{
    ChildrenID.Add( FArianeObjectID( Child ) );

    Child->ParentID = FArianeObjectID( this );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::PrependChild( FArianeObject* Child )
{
    ChildrenID.Insert( FArianeObjectID( Child ), 0 );

    Child->ParentID = FArianeObjectID( this );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::AddChild( FArianeObject* Child, FArianeObject* InsertAfter )
{
/*
    ChildrenID.Insert( FArianeObjectID( Child ), ChildrenID.Find( FArianeObjectID( InsertAfter ) ) );

    Child->ParentID = FArianeObjectID( this );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
*/
}

void
FArianeObject::InvalidateChild( FArianeObject* Child )
{
    if( InvalidatedChildren.Find( Child ) == INDEX_NONE )
    {
        InvalidatedChildren.Add( Child );
    }

    if( ParentID.GetObject() )
    {
        ParentID.GetObject()->InvalidateChild( this );
    }
}

void
FArianeObject::Invalidate( const FArianeObjectInvalidationFlags& InInvalidationFlags )
{
    if ( ParentID.GetObject() )
    {
        ParentID.GetObject()->InvalidateChild( this );
    }

    FArianeInvalidationFlags::OR( *InvalidationFlags, *InvalidationFlags, InInvalidationFlags );
}

void
FArianeObject::GetInvalidatedObjects( TArray<FArianeObject*> OutInvalidatedObjects, bool Recurse )
{
    for( FArianeObject* InvalidatedObject : InvalidatedChildren )
    {
        OutInvalidatedObjects.Add( InvalidatedObject );

        if( Recurse )
        {
            InvalidatedObject->GetInvalidatedObjects( OutInvalidatedObjects, Recurse );
        }
    }
}

bool
FArianeObject::Update( bool Recurse )
{
    if( Recurse )
    {
        InvalidatedChildren.RemoveAll( [&Recurse](  FArianeObject* InvalidatedObject )
            {
                return InvalidatedObject->Update( Recurse );
            } );
    }

    return InvalidatedChildren.Num() ? false : true;
}

FArianeInvalidationFlags&
FArianeObject::GetInvalidationFlags()
{
    return *InvalidationFlags;
}

const
FBoxSphereBounds&
FArianeObject::GetBounds()
{
    return Bounds;
}

void
FArianeObject::UpdateBounds()
{

}
