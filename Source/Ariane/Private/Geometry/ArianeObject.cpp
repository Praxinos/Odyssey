// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeObject.h"
#include "StructUtils/InstancedStruct.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerDrawing.h"

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
    , DrawingLayer ( nullptr )
{
}

FArianeObject::FArianeObject( UArianePainting3DComponent* InPainting3DComponent )
    : FArianeObject()
{
    Painting3DComponent = InPainting3DComponent;
}

void
FArianeObject::RemoveChild( FArianeObject* ChildToRemove, bool bRemoveFromInstancedObjects )
{
    ChildrenID.RemoveAll( [ ChildToRemove ](  FArianeObjectID& ChildObjectID ) -> bool
    {
        return ( ChildToRemove->GetGuid() == ChildObjectID.Guid );
    } );

    InvalidatedChildrenID.RemoveAll( [ ChildToRemove ](  FArianeObjectID& ChildObjectID ) -> bool
    {
        return ( ChildToRemove->GetGuid() == ChildObjectID.Guid );
    } );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );

    ChildToRemove->SetParent( nullptr );
    ChildToRemove->Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
    // update now because the child won't be recursively updatable from a parent object
    ChildToRemove->Update( true );

    if( bRemoveFromInstancedObjects )
    {
        Painting3DComponent->DeleteInstancedObject( ChildToRemove );
    }
}

void
FArianeObject::AppendChild( FArianeObject* Child )
{
    ChildrenID.Add( FArianeObjectID( Child ) );

    Child->SetParent( this );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::PrependChild( FArianeObject* Child )
{
    ChildrenID.Insert( FArianeObjectID( Child ), 0 );

    Child->SetParent( this );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::InsertChild( FArianeObject* Child, FArianeObject* InsertAfter )
{
    int FoundObjectIndex = ChildrenID.IndexOfByPredicate( [Child]( const FArianeObjectID& ObjectID ) -> bool
                                                          {
                                                              return ( ObjectID.Guid == Child->Guid ) ? true : false;
                                                          } );

    ChildrenID.Insert( FArianeObjectID( Child ), FoundObjectIndex + 1 );

    Child->SetParent( this );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::InvalidateChild( FArianeObject* Child )
{
    if( InvalidatedChildrenID.FindByPredicate( [Child]( const FArianeObjectID& ObjectID ) -> bool
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
FArianeObject::GetInvalidatedChildren( TArray<FArianeObject*> OutInvalidatedObjects, bool bRecurse )
{
    for( FArianeObjectID& InvalidatedObject : InvalidatedChildrenID )
    {
        OutInvalidatedObjects.Add( InvalidatedObject.GetObject() );

        if( bRecurse )
        {
            InvalidatedObject.GetObject()->GetInvalidatedChildren( OutInvalidatedObjects, bRecurse );
        }
    }
}

FArianeObject::TraversalReturnValue
FArianeObject::Traverse_Private( TFunction<TraversalReturnValue(FArianeObject*)> Callback )
{
    TraversalReturnValue Ret = Callback( this );

    if( Ret == TraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ( Ret == TraversalReturnValue::IgnoreChildren ) == 0 )
    {
        for( FArianeObjectID& ChildID : ChildrenID )
        {
            TraversalReturnValue ChildRet = ChildID.GetObject()->Traverse_Private( Callback );

            if( ChildRet == TraversalReturnValue::Stop )
            {
                return ChildRet;
            }
        }
    }

    return Ret;
}

void
FArianeObject::Traverse( TFunction<TraversalReturnValue(FArianeObject*)> Callback )
{
    Traverse_Private( Callback );
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

FVector
FArianeObject::GetTranslation()
{
    return Translation;
}

FVector
FArianeObject::GetRotationInDegrees()
{
    return RotationInDegrees;
}

FVector
FArianeObject::GetScaling()
{
    return Scaling;
}

UArianeLayerDrawing*
FArianeObject::GetDrawingLayer()
{
    return DrawingLayer;
}

void
FArianeObject::SetDrawingLayer( UArianeLayerDrawing* InDrawingLayer )
{
    DrawingLayer = InDrawingLayer;
}

bool
FArianeObject::IsVisible( bool bInHierarchical )
{
    if( bInHierarchical )
    {
        // Unimplemented
    }

    return ( ( DrawingLayer == nullptr ) || DrawingLayer->IsVisible( true ) == true ) ? true : false;
}

UArianePainting3DComponent*
FArianeObject::GetPainting3DComponent()
{
    return Painting3DComponent;
}

const FGuid&
FArianeObject::GetGuid()
{
    return Guid;
}

void
FArianeObject::ExportProperties( FArianeObject* DestObject )
{
    DestObject->Name = Name;
    DestObject->Translation = Translation;
    DestObject->RotationInDegrees = RotationInDegrees;
    DestObject->Scaling = Scaling;
}

FArianeObject*
FArianeObject::GetParent()
{
    return ParentID.GetObject();
}

void
FArianeObject::SetParent( FArianeObject* Parent )
{
    ParentID = FArianeObjectID( Parent );
}
