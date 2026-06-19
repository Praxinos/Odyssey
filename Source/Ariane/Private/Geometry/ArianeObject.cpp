// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeObject.h"
#include "ArianeGroup.h"
#include "ArianeTag.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
// Unreal headers
#include "StructUtils/InstancedStruct.h"

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
        Tags      &= ((FArianeObjectInvalidationFlags&)RHS).Tags;
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
        Tags      |= ((FArianeObjectInvalidationFlags&)RHS).Tags;
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
    Children  = 1;
    Tags      = 1;

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
    Tags      = 0;

    return *this;
}

bool
FArianeObjectInvalidationFlags::HasAny()
{
    return ( Selected
          || Altered
          || Hierarchy
          || Color
          || Children
          || Tags  );
}


///---------------------------------------------------------

FArianeObject::~FArianeObject()
{
}

FArianeObject::FArianeObject()
    : Name ( FName( "Ariane Object" ) )
    , Guid ( FGuid::NewGuid() )
    , ParentID ( FArianeObjectID() )
    , Translation ( 0.0f, 0.0f, 0.0f )
    , RotationInDegrees( 0.0f )
    , Scaling( 1.0f, 1.0f, 1.0f )
    , DrawingLayer( nullptr )
    , bVisible ( true )
    , bExpanded ( false )
    , InvalidationFlags ( new FArianeObjectInvalidationFlags() )
{
}

FArianeObject::FArianeObject( UArianeLayerDrawing* InDrawingLayer, const FName& InName )
    : FArianeObject()
{
    DrawingLayer = InDrawingLayer;
    Name = InName;
}

bool
FArianeObject::HasBaseClass( uint32 BaseClass )
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return false;
}

void
FArianeObject::RemoveChild( FArianeObject* ChildToRemove, bool bRemoveFromInstancedObjects )
{
    Children.RemoveAll( [ ChildToRemove ](  FArianeObjectID& ChildObjectID ) -> bool
    {
        return ( ChildToRemove->GetGuid() == ChildObjectID.Guid );
    } );

    InvalidatedChildren.RemoveAll( [ ChildToRemove ](  FArianeObjectID& ChildObjectID ) -> bool
    {
        return ( ChildToRemove->GetGuid() == ChildObjectID.Guid );
    } );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );

    ChildToRemove->SetParent( nullptr );
    ChildToRemove->Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
    // update now because the child won't be recursively updatable from a parent object
    ChildToRemove->Update( true );
    ChildToRemove->Removed();

    if( bRemoveFromInstancedObjects )
    {
        DrawingLayer->DeleteInstancedObject( ChildToRemove );
    }
}

void
FArianeObject::AppendChild( FArianeObject* Child )
{
    Children.Add( FArianeObjectID( Child ) );

    Child->SetParent( this );
    Child->Added();

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::PrependChild( FArianeObject* Child )
{
    Children.Insert( FArianeObjectID( Child ), 0 );

    Child->SetParent( this );
    Child->Added();

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::InsertChild( FArianeObject* Child, FArianeObject* InsertAfter )
{
    int FoundObjectIndex = Children.IndexOfByPredicate( [Child]( const FArianeObjectID& ObjectID ) -> bool
                                                        {
                                                            return ( ObjectID.Guid == Child->Guid ) ? true : false;
                                                        } );

    Children.Insert( FArianeObjectID( Child ), FoundObjectIndex + 1 );

    Child->SetParent( this );
    Child->Added();

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::InvalidateChild( FArianeObject* Child )
{

    if( InvalidatedChildren.FindByPredicate( [Child]( const FArianeObjectID& ObjectID ) -> bool
                                             {
                                                 return ( ObjectID.Guid == Child->Guid ) ? true : false;
                                             } ) == nullptr )
    {
        InvalidatedChildren.Add( FArianeObjectID( Child ) );
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
    for( FArianeObjectID& InvalidatedObject : InvalidatedChildren )
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
        for( FArianeObjectID& ChildID : Children )
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

const FTransform&
FArianeObject::GetTransform()
{
    return GetDrawingLayer()->GetComponentTransform();
}

bool
FArianeObject::Update( bool Recurse, bool bClearFlags )
{
    if( Recurse )
    {
        InvalidatedChildren.RemoveAll( [&Recurse](  FArianeObjectID& InvalidatedChildID )
            {
                return InvalidatedChildID.GetObject()->Update( Recurse );
            } );
    }

    if( bClearFlags )
    {
        InvalidationFlags->Clear();
    }

    return InvalidatedChildren.Num() ? false : true;
}

FArianeObjectInvalidationFlags&
FArianeObject::GetInvalidationFlags()
{
    return *InvalidationFlags;
}


UArianePainting3DComponent*
FArianeObject::GetPainting3DComponent()
{
    return DrawingLayer ? DrawingLayer->GetLayerStack()->GetPainting3DComponent() : nullptr;
}

void
FArianeObject::SetExpanded( bool bInExpanded )
{
    bExpanded = bInExpanded;
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

TArray<FArianeObjectID>&
FArianeObject::GetChildren()
{
    return Children;
}

const TArray<FArianeObjectID>&
FArianeObject::GetChildren() const
{
    return Children;
}

void
FArianeObject::SetName( const FName& InName )
{
    Name = InName;
}

const FName&
FArianeObject::GetName()
{
    return Name;
}

FArianeObject*
FArianeObject::GetAncestorByClass( uint32 iClass, bool bHasBaseObjectClass, bool bSelf )
{
    FArianeObject* Ancestor = bSelf ? this : ParentID.GetObject();

    while ( Ancestor )
    {
        if( ( bHasBaseObjectClass && Ancestor->HasBaseClass( iClass )  ) || Ancestor->GetClass() == iClass )
        {
            return Ancestor;
        }

        Ancestor = Ancestor->GetParent();
    }

    return nullptr;
}

FArianeTag*
FArianeObject::GetTagByGuid( const FGuid& InGuid )
{
    for( FInstancedStruct& InstancedTag : InstancedTags )
    {
        FArianeTag* Tag = InstancedTag.GetMutablePtr<FArianeTag>();

        if( Tag->GetGuid() == InGuid )
        {
            return Tag;
        }
    }

    return nullptr;
}

const TArray<FArianeTagID>&
FArianeObject::GetTags() const
{
    return Tags;
}

TArray<FArianeTagID>&
FArianeObject::GetTags()
{
    return Tags;
}

bool
FArianeObject::IsExpanded()
{
    return bExpanded;
}

void
FArianeObject::SetSelected( bool bInSelected )
{
    bSelected = bInSelected;

    Invalidate( FArianeObjectInvalidationFlags().SetSelected() );
}

bool
FArianeObject::IsSelected()
{
    return bSelected;
}

FArianeObject*
FArianeObject::GetPreviousChild( FArianeObject* Child )
{
    FArianeObject* PreviousItem = nullptr;

    for( FArianeObjectID& ItemID : Children )
    {
        FArianeObject* Item = ItemID.GetObject();

        if( Item == Child )
        {
            return PreviousItem;
        }

        PreviousItem = Item;
    }

    return PreviousItem;
}

void
FArianeObject::TransferChild( FArianeObject* FosterChild
                            , FArianeObject* InsertAfter )
{
    if( ( FosterChild != this ) && ( FosterChild != InsertAfter ) )
    {
        FArianeObject* FormerParent = FosterChild->GetParent();
        FArianeObject* PreviousChild = FormerParent->GetPreviousChild( FosterChild );

        //uint32 removalFlags = FosterChild->GetParent()->RemoveChild( FosterChild, false );
        FosterChild->GetParent()->RemoveChild( FosterChild, false );

        // removal succeeded
        ////if( removalFlags == HIERARCHY_CHANGE_SUCCESS )
        {
            //BLMatrix2D childFormerWorldMatrix = iFosterChild->mWorldMatrix;
            //uint32 additionFlags = AddChild( FosterChild, InsertAfter );

            FArianeObjectID* InsertAfterID = Children.FindByPredicate(
                [InsertAfter] ( FArianeObjectID& ItemID ) -> bool
                {
                   if( ItemID.Guid == InsertAfter->GetGuid() )
                   {
                       return true;
                   }

                    return false;
                } );

            if( InsertAfterID )
            {
                Children.Insert( FosterChild, InsertAfterID - Children.GetData() );
            }
/* Gary
            if( additionFlags == HIERARCHY_CHANGE_SUCCESS )
            {
                double translationX, translationY, rotation, scalingX, scalingY, skewX, skewY;
                BLMatrix2D localMatrix;

                FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, childFormerWorldMatrix, localMatrix );
                FOdysseyVector::ExtractTransformations( localMatrix
                                                      , &translationX
                                                      , &translationY
                                                      , &rotation
                                                      , &scalingX
                                                      , &scalingY
                                                      , &skewX
                                                      , &skewY
                                                      , true ); // in degrees

                iFosterChild->SetTransform( translationX
                                          , translationY
                                          , rotation
                                          , scalingX
                                          , scalingY
                                          , skewX
                                          , skewY );

                iFosterChild->UpdateMatrix();
            }
            else // add back
            {
                FormerParent->AddChild( FosterChild, PreviousChild );
            }

            //return additionFlags; // transfer succeeded
*/
        }

        //return removalFlags;
    }

    //return HIERARCHY_CHANGE_ERROR;
}

FArianeGroup*
FArianeObject::GetRootGroup()
{
    return DrawingLayer ? DrawingLayer->GetRootGroup() : nullptr;
}

#ifdef WITH_EDITOR
FColor
FArianeObject::GetHUDForegroundColor()
{
    FArianeObject* Group = GetAncestorByClass( FArianeGroup::StaticClass()
                                             , true
                                             , false );

    return Group ? Group->GetHUDForegroundColor() : DrawingLayer->GetLayerStack()->GetPainting3DComponent()->GetHUDForegroundColor();
}
#endif

void
FArianeObject::SetVisible( bool bInVisible )
{
    bVisible = bInVisible;
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

void
FArianeObject::PostEditUndo()
{
    if( ParentID.GetObject() )
    {
        ParentID.GetObject()->AppendChild( this );
    }
}

void
FArianeObject::PostLoad()
{
    if( ParentID.GetObject() )
    {
        ParentID.GetObject()->AppendChild( this );
    }
}
