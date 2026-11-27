// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeObject.h"
#include "ArianeGroup.h"
#include "ArianeTag.h"
#include "ArianePainting3DComponent.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
#include "ArianeLayerStack.h"
#include "ArianeKeyedObject.h"
#include "ArianeMesh.h"
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
        Tags      &= ((FArianeObjectInvalidationFlags&)RHS).Tags;
        Name      &= ((FArianeObjectInvalidationFlags&)RHS).Name;
        Transform &= ((FArianeObjectInvalidationFlags&)RHS).Transform;
        Visibility &= ((FArianeObjectInvalidationFlags&)RHS).Visibility;
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
        Tags      |= ((FArianeObjectInvalidationFlags&)RHS).Tags;
        Name      |= ((FArianeObjectInvalidationFlags&)RHS).Name;
        Transform |= ((FArianeObjectInvalidationFlags&)RHS).Transform;
        Visibility |= ((FArianeObjectInvalidationFlags&)RHS).Visibility;
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
    Tags      = 1;
    Name      = 1;
    Transform = 1;
    Visibility = 1;

    return *this;
}

FArianeObjectInvalidationFlags&
FArianeObjectInvalidationFlags::Clear()
{
    Selected  = 0;
    Altered   = 0;
    Hierarchy = 0;
    Color     = 0;
    Tags      = 0;
    Name      = 0;
    Transform = 0;
    Visibility = 0;

    return *this;
}

bool
FArianeObjectInvalidationFlags::HasAny()
{
    return ( Selected
          || Altered
          || Hierarchy
          || Color
          || Tags
          || Name
          || Transform
          || Visibility );
}


///---------------------------------------------------------

void
FArianeObject::Animate( const FArianeKeyedObject* KeyedObject, const FArianeKeyedObject* NextKeyedObject, float T )
{

}

FArianeObject::~FArianeObject()
{
    // free mallocated Tags.
    for( FArianeTagID& TagID : Tags )
    {
        FArianeTag* Tag = TagID.GetTag( Drawing );

        if( Tag->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            delete Tag;
        }
    }
}

FArianeObject::FArianeObject()
    : FArianeObject( nullptr
                   , FName( "Ariane Object" )
                   , EArianeAllocationModel::InstancedStruct )
{
}

FArianeObject::FArianeObject( UArianeDrawing* InDrawing
                            , const FName& InName
                            , EArianeAllocationModel InAllocationModel )
    : Name ( InName )
    , Guid ( FGuid::NewGuid() )
    , ParentID ( FArianeObjectID() )
    , bVisible ( true )
    , bExpanded ( true )
    , AllocationModel ( InAllocationModel  )
    , Drawing( InDrawing )
    , WorldTransformVersion( 0 )
    , BoundingBox( FBox(ForceInit) )
    , bSelected ( false )
    , bInvalidatedInParent ( false )
{
}

const FGuid&
FArianeObject::GetAllocatorGuid()
{
    return AllocatorGuid;
}

FArianeObjectInvalidationFlags&
FArianeObject::GetInvalidationFlags()
{
    FArianeObjectInvalidationFlags* InvalidationFlagsPtr = InstancedInvalidationFlags.GetMutablePtr<FArianeObjectInvalidationFlags>();

    if( InvalidationFlagsPtr == nullptr )
    {
        InstancedInvalidationFlags = FInstancedStruct::Make<FArianeObjectInvalidationFlags>();

        InvalidationFlagsPtr = InstancedInvalidationFlags.GetMutablePtr<FArianeObjectInvalidationFlags>();
    }

    return *InvalidationFlagsPtr;
}

/*
FArianeObject::FObjectTransientData&
FArianeObject::GetTransientData()
{
    FObjectTransientData* TransientDataPtr = InstancedTransientData.GetMutablePtr<FObjectTransientData>();

    if( TransientDataPtr == nullptr )
    {
        InstancedTransientData = FInstancedStruct::Make<FObjectTransientData>();

        TransientDataPtr = InstancedTransientData.GetMutablePtr<FObjectTransientData>();
    }

    return *TransientDataPtr;
}
*/

void
FArianeObject::ResetDrawing( UArianeDrawing* InDrawing )
{
    InstancedInvalidationFlags.Reset();
    Drawing = InDrawing;
    AllocatorGuid = FGuid::NewGuid();
    WorldTransform = FTransform();
    WorldTransformVersion = 0;
    InvalidatedChildren.Empty();
    Children.Empty();
    BoundingBox = FBox(ForceInit);
    bSelected = false;
    //OnPostInvalidated.Clear();
}

EArianeAllocationModel
FArianeObject::GetAllocationModel()
{
    return AllocationModel;
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
    Children.Remove( ChildToRemove );
    RenumberChildren();

    InvalidatedChildren.Remove( ChildToRemove );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );

    ChildToRemove->SetParent( nullptr );
    ChildToRemove->Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
    // update now because the child won't be recursively updatable from a parent object
    ChildToRemove->Update( EUpdateFlags::None, true );
    ChildToRemove->Removed();

    if( ChildToRemove->IsSelected() )
    {
        Drawing->UnselectObject( ChildToRemove );
    }

    if( bRemoveFromInstancedObjects )
    {
        TArray<FArianeObject*> ObjectsToDelete;

        // Free the whole subtree, or else the descendants stay in InstancedObjects as invisible orphans.
        // Collect first: deleting an instanced struct frees the object, its children could not be read afterwards
        Traverse( ChildToRemove
                , ETraversalPolicy::AllChildren
                , [&ObjectsToDelete]( FArianeObject* Object ) -> ETraversalReturnValue
            {
                ObjectsToDelete.Add( Object );

                return ETraversalReturnValue::Continue;
            } );

        for( FArianeObject* ObjectToDelete : ObjectsToDelete )
        {
            // The callers don't always clear the selection first (eraser, edit tool)
            if( ObjectToDelete->IsSelected() )
            {
                Drawing->UnselectObject( ObjectToDelete );
            }

            Drawing->DeleteInstancedObject( ObjectToDelete );
        }
    }
}

void
FArianeObject::AppendChild( FArianeObject* Child, FArianeObject::ETransformPolicy TransformPolicy, bool bRenumber )
{
    InsertChild( Child, Children.Num() ? Children.Last() : nullptr, TransformPolicy, bRenumber );
}

void
FArianeObject::PrependChild( FArianeObject* Child, FArianeObject::ETransformPolicy TransformPolicy )
{
    InsertChild( Child, nullptr, TransformPolicy );
}

void
FArianeObject::InsertChild( FArianeObject* Child, FArianeObject* InsertAfter, FArianeObject::ETransformPolicy TransformPolicy, bool bRenumber )
{
    FTransform RelativeTransform = Child->GetTransform().GetRelativeTransform(WorldTransform);

    int FoundObjectIndex = Children.IndexOfByPredicate( [InsertAfter]( const FArianeObject* Object ) -> bool
                                                        {
                                                            return ( Object == InsertAfter );
                                                        } );

    Children.Insert( Child, FoundObjectIndex + 1 );

    if( bRenumber )
    {
        RenumberChildren();
    }

    Child->SetParent( this );
    Child->Added();

    if( TransformPolicy == ETransformPolicy::KeepAbsolute )
    {
        FQuat Q = RelativeTransform.GetRotation();
        FRotator R = Q.Rotator();

        Child->SetTransform( RelativeTransform.GetTranslation(), R.Euler(), RelativeTransform.GetScale3D(), FVector::Zero() );
    }

    Child->Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::SetInvalidatedInParent( bool bInInvalidatedInParent )
{
    bInvalidatedInParent = bInInvalidatedInParent;
}

bool
FArianeObject::IsInvalidatedInParent()
{
    return bInvalidatedInParent;
}

bool
FArianeObject::IsInvalidated()
{
    // InvalidatedChildren: a descendant may stay invalidated on purpose, while this object's own flags are clear
    return ( GetInvalidationFlags().HasAny() || InvalidatedChildren.Num() );
}

void
FArianeObject::InvalidateChild( FArianeObject* Child )
{
    FArianeObject* Parent = ParentID.GetObject( Drawing );

    if( Child->IsInvalidatedInParent() == false )
    {
        InvalidatedChildren.Add( Child );

        Child->SetInvalidatedInParent( true );
    }

     // This must always be called to guarantee child-to-parent propagation
    if( Parent )
    {
        Parent->InvalidateChild( this );
    }
    else
    {
        Drawing->Invalidate();
    }
}

void
FArianeObject::Invalidate( const FArianeObjectInvalidationFlags& InInvalidationFlags )
{
    FArianeObject* Parent = ParentID.GetObject( Drawing );

    // Flags first: the propagation ends in UArianeDrawing::Invalidate(), which reads them (e.g. Color for UsedMaterials)
    GetInvalidationFlags().OR( InInvalidationFlags );

    if ( Parent )
    {
        Parent->InvalidateChild( this );
    }

    //OnPostInvalidated.Broadcast();
}

TArray<FArianeObject*>&
FArianeObject::GetInvalidatedChildren()
{
    return InvalidatedChildren;
}

void
FArianeObject::GetInvalidatedChildren( TArray<FArianeObject*>& OutInvalidatedObjects, bool bRecurse )
{
    for( FArianeObject* InvalidatedObject : InvalidatedChildren )
    {
        OutInvalidatedObjects.Add( InvalidatedObject );

        if( bRecurse )
        {
            InvalidatedObject->GetInvalidatedChildren( OutInvalidatedObjects, bRecurse );
        }
    }
}


FArianeObject::ETraversalReturnValue
FArianeObject::Traverse_Private( FArianeObject* Object
                               , ETraversalPolicy TraversalPolicy
                               , TFunctionRef<ETraversalReturnValue(FArianeObject*)> Callback )
{
    UArianeDrawing* Drawing = Object->GetDrawing();
    ETraversalReturnValue Ret = Callback( Object );

    if( Ret == ETraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ( Ret == ETraversalReturnValue::IgnoreChildren ) == 0 )
    {
        // No copy: the callbacks must not modify the traversed children
        TArray<FArianeObject*>* ChildrenToTraverse = nullptr;

        switch( TraversalPolicy )
        {
            case ETraversalPolicy::AllChildren :
                ChildrenToTraverse = &Object->GetChildren();
            break;

            case ETraversalPolicy::InvalidatedChildren :
                ChildrenToTraverse = &Object->GetInvalidatedChildren();
            break;

            default:
            break;
        }

        if( ChildrenToTraverse )
        {
            for( FArianeObject* Child : *ChildrenToTraverse )
            {
                ETraversalReturnValue ChildRet = Traverse_Private( Child, TraversalPolicy, Callback );

                if( ChildRet == ETraversalReturnValue::Stop )
                {
                    return ChildRet;
                }
            }
        }
    }

    return Ret;
}

void
FArianeObject::Traverse( FArianeObject* Object, ETraversalPolicy TraversalPolicy, TFunctionRef<ETraversalReturnValue(FArianeObject*)> Callback )
{
    Traverse_Private( Object, TraversalPolicy, Callback );
}

// static
FArianeObject::ETraversalReturnValue
FArianeObject::TraverseBackwards_Private( FArianeObject* Object, TFunctionRef<ETraversalReturnValue(FArianeObject*)> Callback )
{
    ETraversalReturnValue Ret = Callback( Object );
    FArianeObject* Parent = Object->GetParent();

    if( Ret == ETraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( Parent )
    {
        ETraversalReturnValue ParentRet = TraverseBackwards_Private( Parent, Callback );

        if( ParentRet == ETraversalReturnValue::Stop )
        {
            return ParentRet;
        }
    }

    return Ret;
}

// static
void
FArianeObject::TraverseBackwards( FArianeObject* Object, TFunctionRef<ETraversalReturnValue(FArianeObject*)> Callback )
{
    TraverseBackwards_Private( Object, Callback );
}

const FTransform&
FArianeObject::GetTransform()
{
    return WorldTransform;
}

const FTransform&
FArianeObject::GetLocalTransform()
{
    return LocalTransform;
}


//static
uint32
FArianeObject::CheckCommonClass( const TArray<FArianeObject*>& Objects, uint32 CommonClass )
{
    if( Objects.Num() )
    {
        for( FArianeObject* Object : Objects )
        {
            if( Object->HasBaseClass( CommonClass ) )
            {
                uint32 ObjectClass = Object->GetClass();

                if( ObjectClass != CommonClass )
                {
                    uint32 NewCommonClass = CheckCommonClass( Objects, ObjectClass );

                    if( NewCommonClass )
                    {
                        return NewCommonClass;
                    }
                }
            }
            else
            {
                return 0;
            }
        }

        return CommonClass;
    }

    return 0;
}

// static
uint32
FArianeObject::GetCommonClass( const TArray<FArianeObject*>& Objects )
{
    return CheckCommonClass( Objects, FArianeObject::StaticClass() );
}

void
FArianeObject::UpdateShape( EUpdateFlags UpdateFlags )
{
}

bool
FArianeObject::Update( EUpdateFlags UpdateFlags, bool Recurse )
{
    UpdateShape( UpdateFlags );

    if( Recurse )
    {
        InvalidatedChildren.RemoveAll( [ this
                                       , &UpdateFlags
                                       , &Recurse](  FArianeObject* InvalidatedChild )
            {
                bool bUpdated = InvalidatedChild->Update( UpdateFlags, Recurse );

                if( bUpdated == true )
                {
                    InvalidatedChild->SetInvalidatedInParent( false );
                }

                return bUpdated;
            } );
    }

    UpdateBoundingBox( UpdateFlags );

    // After UpdateBoundingBox: the item copies the new bounding box
    RefreshRenderItem();

    if( EnumHasAllFlags( UpdateFlags, EUpdateFlags::KeepInvalidated ) == false )
    {
        GetInvalidationFlags().Clear();
    }

    return InvalidatedChildren.Num() ? false : true;
}

void
FArianeObject::SetTranslation( double InX, double InY, double InZ )
{
    SetTranslation( FVector( InX, InY, InZ ) );
}

void
FArianeObject::SetTranslation( const FVector& InTranslation )
{
    LocalTransform.SetTranslation( InTranslation );

    // reset world Transform. It wil lbe updated at the next call to UpdateTransform();
    WorldTransformVersion = 0;

    Invalidate( FArianeObjectInvalidationFlags().SetTransform() );
}

void
FArianeObject::SetRotation( double InX, double InY, double InZ )
{
    SetRotation( FVector( InX, InY, InZ ) );
}

void
FArianeObject::SetRotation( const FVector& InRotation )
{
    FRotator Rotator( InRotation.Y, InRotation.Z, InRotation.X );

    LocalTransform.SetRotation( Rotator.Quaternion() );

    // reset world Transform. It wil lbe updated at the next call to UpdateTransform();
    WorldTransformVersion = 0;

    Invalidate( FArianeObjectInvalidationFlags().SetTransform() );
}

void
FArianeObject::SetScaling( double InX, double InY, double InZ )
{
    SetScaling( FVector( InX, InY, InZ ) );
}

void
FArianeObject::SetScaling( const FVector& InScaling )
{
    LocalTransform.SetScale3D( InScaling );

    // reset world Transform. It wil lbe updated at the next call to UpdateTransform();
    WorldTransformVersion = 0;

    Invalidate( FArianeObjectInvalidationFlags().SetTransform() );
}

/*
void
FArianeObject::SetSkew( double InX, double InY, double InZ )
{
    SetSkew( FVector( InX, InY, InZ ) );
}

void
FArianeObject::SetSkew( const FVector& InSkew )
{
    Skew = InSkew;

    Invalidate( FArianeObjectInvalidationFlags().SetTransform() );
}
*/

FVector
FArianeObject::GetTranslation()
{
    return LocalTransform.GetTranslation();
}

FVector
FArianeObject::GetScaling()
{
    return LocalTransform.GetScale3D();
}

FVector
FArianeObject::GetRotation()
{
    return LocalTransform.GetRotation().Euler();
}

/*
FVector
FArianeObject::GetSkew()
{
    return Skew;
}
*/

void
FArianeObject::GetTransform( FVector& OutTranslation
                           , FVector& OutRotation
                           , FVector& OutScaling
                           , FVector& OutSkewing )
{
    OutTranslation = GetTranslation();
    OutRotation = GetRotation();
    OutScaling = GetScaling();
    //OutSkewing = Skew;
}

void
FArianeObject::SetTransform( const FVector& InTranslation
                           , const FVector& InRotation
                           , const FVector& InScaling
                           , const FVector& InSkew )
{
    SetTranslation( InTranslation );
    SetRotation( InRotation );
    SetScaling( InScaling );
    //Skew = InSkew ;
    // reset world Transform. It wil lbe updated at the next call to UpdateTransform();
    WorldTransformVersion = 0;
}

void
FArianeObject::ResetTransform()
{
    SetTranslation( FVector::Zero() );
    SetScaling( FVector::One() );
    SetRotation( FVector::Zero() );
    //SetSkew( FVector::Zero() );
}

void
FArianeObject::UpdateTransform()
{
    Traverse( this
            , FArianeObject::ETraversalPolicy::AllChildren
            , []( FArianeObject* Object  ) -> ETraversalReturnValue
        {
            FArianeObject* Parent = Object->GetParent();
            UArianeLayerVector* VectorLayer = Object->GetVectorLayer();

            if( VectorLayer )
            {
                const FTransform& ParentWorldTransform = Parent ? Parent->WorldTransform
                                                                : VectorLayer->GetComponentTransform();
                uint32 ParentWorldTransformVersion = Parent ? Parent->WorldTransformVersion
                                                            : VectorLayer->GetWorldTransformVersion();

                if( Object->WorldTransformVersion != ParentWorldTransformVersion )
                {
                    Object->WorldTransform = Object->LocalTransform * ParentWorldTransform;

                    Object->WorldTransformVersion = ParentWorldTransformVersion;

                    // The render items hold a copy of the world matrix
                    if( Object->HasBaseClass( FArianeMesh::StaticClass() ) )
                    {
                        static_cast<FArianeMesh*>(Object)->RefreshRenderItemTransform();
                    }
                }
            }

//UE_LOG(LogTemp, Warning, TEXT("Object:%s - Parent:%p - Transform: %s"), *Name.ToString(), Parent, *WorldTransform.ToString());

            return ETraversalReturnValue::Continue;
        } );
}

UArianePainting3DComponent*
FArianeObject::GetPainting3DComponent()
{
    UArianeLayerVector* VectorLayer = GetVectorLayer();

    return VectorLayer ? VectorLayer->GetLayerStack()->GetPainting3DComponent() : nullptr;
}

UArianeLayerVector*
FArianeObject::GetVectorLayer()
{
    return Drawing ? Drawing->GetVectorLayer().Get() : nullptr;
}

void
FArianeObject::RefreshRenderItem()
{
}

void
FArianeObject::InvalidateTree( const FArianeObjectInvalidationFlags& InInvalidationFlags )
{
    Traverse( this
            , ETraversalPolicy::AllChildren
            , [ &InInvalidationFlags ]( FArianeObject* Object ) -> ETraversalReturnValue
        {
            Object->Invalidate( InInvalidationFlags );

            return ETraversalReturnValue::Continue;
        } );
}

void
FArianeObject::SetExpanded( bool bInExpanded )
{
    bExpanded = bInExpanded;
}


const
FBox&
FArianeObject::GetBoundingBox() const
{
    return BoundingBox;
}

void
FArianeObject::UpdateBoundingBox( EUpdateFlags UpdateFlags )
{
    BoundingBox = FBox(ForceInit);
}

/*
FSimpleMulticastDelegate &
FArianeObject::GetOnPostInvalidatedDelegate()
{
    return OnPostInvalidated;
}
*/

UArianeDrawing*
FArianeObject::GetDrawing()
{
    return Drawing;
}

TArray<FArianeObject*>&
FArianeObject::GetChildren()
{
    return Children;
}

const TArray<FArianeObject*>&
FArianeObject::GetChildren() const
{
    return Children;
}

void
FArianeObject::SetName( const FName& InName )
{
    Name = InName;

    Invalidate( FArianeObjectInvalidationFlags().SetName() );
}

const FName&
FArianeObject::GetName()
{
    return Name;
}

FArianeObject*
FArianeObject::GetAncestorByClass( uint32 iClass, bool bHasBaseObjectClass, bool bSelf )
{
    FArianeObject* Ancestor = bSelf ? this : ParentID.GetObject( Drawing );

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

bool
FArianeObject::HasAncestor( FArianeObject* CandidateAncestor )
{
    bool bHasAncestor = false;

    TraverseBackwards( this
                     , [ this
                       , CandidateAncestor
                       , &bHasAncestor ]( FArianeObject* Ancestor ) -> ETraversalReturnValue
        {
            // TraverseBackwards starts on the object itself, which is not its own ancestor
            if( ( Ancestor != this ) && ( Ancestor == CandidateAncestor ) )
            {
                bHasAncestor = true;

                return ETraversalReturnValue::Stop;
            }

            return ETraversalReturnValue::Continue;
        } );

    return bHasAncestor;
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
FArianeObject::GetNextChild( FArianeObject* Child )
{
    FArianeObject* NextChild = nullptr;
    int IndexOfChild = Children.IndexOfByPredicate( [this, Child] ( const FArianeObjectID& ObjectId )
        {
            return ( const_cast<FArianeObjectID&>(ObjectId).GetObject( Drawing ) == Child ) ? true : false;
        } );
    int IndexOfNext = IndexOfChild + 1;

    if( IndexOfNext < Children.Num() )
    {
        return Children[IndexOfNext];
    }

    return nullptr;
}

FArianeObject*
FArianeObject::GetPreviousChild( FArianeObject* Child )
{
    FArianeObject* PreviousItem = nullptr;

    for( FArianeObject* Item : Children )
    {
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

            if( InsertAfter )
            {
                int InsertAfterIndex = Children.IndexOfByPredicate(
                    [this,InsertAfter] ( const FArianeObjectID& ItemID ) -> bool
                    {
                       if( const_cast<FArianeObjectID&>(ItemID).GetObject( Drawing ) == InsertAfter )
                       {
                           return true;
                       }

                       return false;
                    } );

                // TArray::Insert accepts Num() as index: no need to special-case the end
                Children.Insert( FosterChild, InsertAfterIndex + 1 );
            }
            else
            {
                // InsertAfter == nullptr means "insert first", like UArianeLayerFolder::InsertChild
                Children.Insert( FosterChild, 0 );
            }

            RenumberChildren();

            FosterChild->SetParent( this );
            FosterChild->Added();

            Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
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

void
FArianeObject::CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate )
{
    FName NewName = EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::Rename ) ? FName( Name.ToString() + "_copy" )
                                                                          : Name;

    DestinationObject->Name = NewName;
    DestinationObject->LocalTransform = LocalTransform;
    DestinationObject->WorldTransform = WorldTransform;
    DestinationObject->bVisible = bVisible;
    DestinationObject->bExpanded = bExpanded;
    //DestinationObject->Opacity = Opacity;

    if( bInvalidate )
    {
        DestinationObject->Invalidate( FArianeObjectInvalidationFlags().SetAll() );
    }
}

FArianeObject*
FArianeObject::CopyShape( const FCopyArgs& CopyArgs )
{
    FArianeObject* ObjectCopy = CopyArgs.Drawing->AllocObject( Name, CopyArgs.AllocationModel );

    return ObjectCopy;
}

FArianeObject*
FArianeObject::Copy( const FCopyArgs& CopyArgs
                   , TFunctionRef<void( FArianeObject*, const FCopyArgs& )> PreCallback
                   , TFunctionRef<void( FArianeObject*, FArianeObject*, const FCopyArgs& )> PostCallback )
{
    PreCallback( this, CopyArgs );

    FArianeObject* ObjectCopy = CopyShape( CopyArgs );

    if( ObjectCopy )
    {
        CopySettings( ObjectCopy, CopyArgs, true );

        // recurse
        for( FArianeObject* Child : Children )
        {
            FCopyArgs ChildCopyArgs = CopyArgs;

            // We only rename the first item of the tree, that's why we reset the flag
            EnumRemoveFlags( ChildCopyArgs.Flags, ECopyFlags::Rename );

            FArianeObject* ChildCopy = Child->Copy( ChildCopyArgs, PreCallback, PostCallback );

            ObjectCopy->AppendChild( ChildCopy, FArianeObject::ETransformPolicy::KeepRelative );
        }

        // copy tags
        if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::IgnoreTags ) == false )
        {
            for( FArianeTagID& TagID : Tags )
            {
                FArianeTag* Tag = TagID.GetTag( Drawing );

                FArianeTag* TagCopy = Tag->Copy( ObjectCopy );

                ObjectCopy->AddTag( TagCopy );
            }
        }
    }

    PostCallback( this, ObjectCopy, CopyArgs );

    return ObjectCopy;
}

FArianeObject*
FArianeObject::Copy( const FCopyArgs& CopyArgs )
{
    return Copy( CopyArgs
               , []( FArianeObject* Object, const FCopyArgs& CopyArgs ){ return 0; }
               , []( FArianeObject* SourceObject
                   , FArianeObject* ObjectCopy, const FCopyArgs& CopyArgs ){ return 0; } );
}

void
FArianeObject::AddTag( FArianeTag* Tag )
{
    Tags.Add( FArianeTagID( Tag ) );

    Tag->Added();

    Invalidate( FArianeObjectInvalidationFlags().SetTags() );
}

FArianeGroup*
FArianeObject::GetRootGroup()
{
    return Drawing ? Drawing->GetRootGroup() : nullptr;
}

#if WITH_EDITOR
FColor
FArianeObject::GetHUDForegroundColor()
{
    FArianeObject* Group = GetAncestorByClass( FArianeGroup::StaticClass()
                                             , true
                                             , true );

    return Group ? Group->GetHUDForegroundColor()
                 : GetVectorLayer()->GetLayerStack()->GetPainting3DComponent()->GetHUDForegroundColor();
}
#endif

void
FArianeObject::SetVisible( bool bInVisible )
{
    bVisible = bInVisible;

    // The render items store the inherited visibility
    InvalidateTree( FArianeObjectInvalidationFlags().SetVisibility() );
}

bool
FArianeObject::IsVisible( bool bInHierarchical )
{
    FArianeObject* Parent = ParentID.GetObject( Drawing );

    //return ( ( VectorLayer == nullptr ) || VectorLayer->IsVisible() == true ) ? true : false;
    return ( bInHierarchical && Parent ) ? bVisible && Parent->IsVisible( bInHierarchical )
                                         : bVisible;
}

FGuid
FArianeObject::GetGuid()
{
    return Guid;
}

void
FArianeObject::ExportProperties( FArianeObject* DestObject )
{
    DestObject->Name = Name;
    DestObject->LocalTransform = LocalTransform;
}

FArianeObject*
FArianeObject::GetParent()
{
    return ParentID.GetObject( Drawing );
}

void
FArianeObject::SetParent( FArianeObject* Parent )
{
    ParentID = FArianeObjectID( Parent );
}

void
FArianeObject::PostEditUndo()
{
}

void
FArianeObject::PostLoad()
{
}

/*
void
FArianeObject::PostDuplicate(EDuplicateMode::Type)
{
}
*/

void
FArianeObject::PostReset()
{
    if( GetParent() )
    {
        // Don't renumber: the loaded ChildIndex is used by SortChildren() once every child is attached
        GetParent()->AppendChild( this, FArianeObject::ETransformPolicy::KeepRelative, false );
    }
}

void
FArianeObject::SortChildren()
{
    // The children were appended in allocation order by PostReset(), without renumbering: sort by the loaded
    // indices. Stable sort: drawings saved before ChildIndex existed have 0 everywhere and keep their order.
    // Note: TArray sorts arrays of pointers through the pointed objects
    Children.StableSort( []( const FArianeObject& A, const FArianeObject& B ) -> bool
        {
            return ( A.ChildIndex < B.ChildIndex );
        } );

    RenumberChildren();
}

void
FArianeObject::RenumberChildren()
{
    for( int32 i = 0; i < Children.Num(); i++ )
    {
        Children[i]->ChildIndex = i;
    }
}
