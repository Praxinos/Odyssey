// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeObject.h"
#include "StructUtils/InstancedStruct.h"
#include "ArianePainting3DComponent.h"


bool
FArianeObjectInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return false;
}

FArianeObjectInvalidationFlags&
FArianeObjectInvalidationFlags::AND( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeObjectInvalidationFlags::StaticClass() ) )
    {
        Selected  &= ((FArianeObjectInvalidationFlags&)RHS).Selected;
        Altered   &= ((FArianeObjectInvalidationFlags&)RHS).Altered;
        Hierarchy &= ((FArianeObjectInvalidationFlags&)RHS).Hierarchy;
    }

    return *this;
}

FArianeObjectInvalidationFlags&
FArianeObjectInvalidationFlags::OR( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeObjectInvalidationFlags::StaticClass() ) )
    {
        Selected  |= ((FArianeObjectInvalidationFlags&)RHS).Selected;
        Altered   |= ((FArianeObjectInvalidationFlags&)RHS).Altered;
        Hierarchy |= ((FArianeObjectInvalidationFlags&)RHS).Hierarchy;
    }

    return *this;
}

FArianeObjectInvalidationFlags&
FArianeObjectInvalidationFlags::SetAll()
{
    Selected  = 1;
    Altered   = 1;
    Hierarchy = 1;

    return *this;
}

FArianeObjectInvalidationFlags&
FArianeObjectInvalidationFlags::Clear()
{
    FArianeObjectInvalidationFlags::ClearOwn( *this );

    return *this;
}

bool
FArianeObjectInvalidationFlags::HasAny()
{
    return ( Selected
          || Altered
          || Hierarchy );
}

void
FArianeObjectInvalidationFlags::ClearOwn( FArianeObjectInvalidationFlags& Flags )
{
    Flags.Selected  = 0;
    Flags.Altered   = 0;
    Flags.Hierarchy = 0;
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
    if( InvalidatedChildrenID.FindByPredicate( [Child]( FArianeObjectID& ObjectID ) -> bool
                                               {
                                                   return ( ObjectID.Guid == Child->Guid ) ? true : false;
                                               } ) == nullptr )
    {
        InvalidatedChildrenID.Add( FArianeObjectID( Child ) );
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

    InvalidationFlags->OR( InInvalidationFlags );
}

void
FArianeObject::GetInvalidatedObjects( TArray<FArianeObject*> OutInvalidatedObjects, bool Recurse )
{
    for( FArianeObjectID& InvalidatedObject : InvalidatedChildrenID )
    {
        OutInvalidatedObjects.Add( InvalidatedObject.GetObject() );

        if( Recurse )
        {
            InvalidatedObject.GetObject()->GetInvalidatedObjects( OutInvalidatedObjects, Recurse );
        }
    }
}

bool
FArianeObject::Update( bool Recurse )
{
    if( Recurse )
    {
        InvalidatedChildrenID.RemoveAll( [&Recurse](  FArianeObjectID& InvalidatedObjectID )
            {
                return InvalidatedObjectID.GetObject()->Update( Recurse );
            } );
    }

    FArianeObjectInvalidationFlags::ClearOwn( *InvalidationFlags );

    return InvalidatedChildrenID.Num() ? false : true;
}

FArianeObjectInvalidationFlags&
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

void
FArianeObject::InvalidatePointerCache( TArray<FArianeObjectID>& ObjectIDArray )
{
    for( FArianeObjectID& ObjectID : ObjectIDArray )
    {
        ObjectID.InvalidatePointerCache();
    }
}

void
FArianeObject::InvalidatePointerCache()
{
    InvalidatePointerCache( ChildrenID );

    ParentID.InvalidatePointerCache();

    InvalidatePointerCache( InvalidatedChildrenID );
}
