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
        Color     &= ((FArianeObjectInvalidationFlags&)RHS).Color;
        Children  &= ((FArianeObjectInvalidationFlags&)RHS).Children;
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
        Color     |= ((FArianeObjectInvalidationFlags&)RHS).Color;
        Children  |= ((FArianeObjectInvalidationFlags&)RHS).Children;
    }

    return *this;
}

FArianeObjectInvalidationFlags&
FArianeObjectInvalidationFlags::SetAll()
{
    Selected  = 1;
    Altered   = 1;
    Hierarchy = 1;
    Color     = 1;
    Children     = 1;

    return *this;
}

FArianeObjectInvalidationFlags&
FArianeObjectInvalidationFlags::Clear()
{
    Selected  = 0;
    Altered   = 0;
    Hierarchy = 0;
    Color     = 0;
    Children  = 0;

    return *this;
}

bool
FArianeObjectInvalidationFlags::HasAny()
{
    return ( Selected
          || Altered
          || Hierarchy
          || Color
          || Children );
}

FArianeObject::~FArianeObject()
{
}

FArianeObject::FArianeObject()
    : DrawingLayer( nullptr )
    , Guid ( FGuid::NewGuid() )
    , ParentID ()
    , InvalidationFlags ( new FArianeObjectInvalidationFlags() )
{
}

FArianeObject::FArianeObject( UArianeLayerDrawing* InDrawingLayer )
    : FArianeObject()
{
    DrawingLayer = InDrawingLayer;
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
        DrawingLayer->DeleteInstancedObject( ChildToRemove );
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

    Invalidate( FArianeObjectInvalidationFlags().SetChildren() );
}

void
FArianeObject::Invalidate( const FArianeObjectInvalidationFlags& InInvalidationFlags )
{
    if ( ParentID.GetObject() )
    {
        ParentID.GetObject()->InvalidateChild( this );
    }

    InvalidationFlags->OR( InInvalidationFlags );

    OnPostInvalidated.Broadcast();
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
FArianeObject::Update( bool Recurse, bool bClearFlags )
{
    if( Recurse )
    {
        InvalidatedChildrenID.RemoveAll( [&Recurse](  FArianeObjectID& InvalidatedChildID )
            {
                return InvalidatedChildID.GetObject()->Update( Recurse );
            } );
    }

    if( bClearFlags )
    {
        InvalidationFlags->Clear();
    }

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

FSimpleMulticastDelegate &
FArianeObject::GetOnPostInvalidatedDelegate()
{
    return OnPostInvalidated;
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

    return ( ( DrawingLayer == nullptr ) || DrawingLayer->IsVisible() == true ) ? true : false;
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
