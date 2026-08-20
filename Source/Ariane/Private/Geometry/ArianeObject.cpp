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
        Name      &= ((FArianeObjectInvalidationFlags&)RHS).Name;
        Transform &= ((FArianeObjectInvalidationFlags&)RHS).Transform;
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
        Name      |= ((FArianeObjectInvalidationFlags&)RHS).Name;
        Transform |= ((FArianeObjectInvalidationFlags&)RHS).Transform;
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
    Name      = 1;
    Transform = 1;

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
    Name      = 0;
    Transform = 0;

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
          || Tags
          || Name
          || Transform );
}


FArianeObjectGeometry3D::~FArianeObjectGeometry3D()
{
    // Some rendering commands use the vertex factory, flush them first
    FlushRenderingCommands();

    if( VertexFactory )
    {
        PositionBuffer.ReleaseResource();
        StaticMeshVB.ReleaseResource();
        ColorBuffer.ReleaseResource();
        IndexBuffer.ReleaseResource();
        VertexFactory->ReleaseResource();

        delete VertexFactory;
    }
}

FArianeObjectGeometry3D::FArianeObjectGeometry3D( FArianeObject* InObject )
    : Object( InObject )
    , VertexCount( 0 )
    , VertexFactory( nullptr )
{

}

const uint32
FArianeObjectGeometry3D::GetVertexCount() const
{
    return VertexCount;
}

const FRawStaticIndexBuffer&
FArianeObjectGeometry3D::GetIndexBuffer() const
{
    return IndexBuffer;
}

FArianeObject*
FArianeObjectGeometry3D::GetObject()
{
     return Object;
}

FLocalVertexFactory*
FArianeObjectGeometry3D::GetVertexFactory()
{
    return VertexFactory;
}

void
FArianeObjectGeometry3D::InitVertexFactory( TArray<FDynamicMeshVertex>& Vertices
                                          , TArray<uint32>& Indices )
{
    if( VertexFactory == nullptr )
    {
        VertexFactory = new FLocalVertexFactory( Object->GetDrawingLayer()->GetLayerStack()->GetPainting3DComponent()->GetWorld()->GetFeatureLevel(), "Object Vertex Factory" );
    }

    ENQUEUE_RENDER_COMMAND(StaticMeshVertexBuffersLegacyInit)(
        [ this
        ,  VerticesAsync = CopyTemp(Vertices) ] ( FRHICommandListImmediate& RHICmdList )
        {
            FLocalVertexFactory::FDataType Data;

            VertexCount = VerticesAsync.Num();

            if( PositionBuffer.IsInitialized() == false ) PositionBuffer.InitResource( RHICmdList );
            if( StaticMeshVB.IsInitialized()   == false ) StaticMeshVB.InitResource( RHICmdList );
            if( ColorBuffer.IsInitialized()    == false ) ColorBuffer.InitResource( RHICmdList );

            if( VertexCount )
            {
                PositionBuffer.Init( VertexCount );
                StaticMeshVB.Init( VertexCount, 1 );
                ColorBuffer.Init( VertexCount );

                for ( uint32 i = 0; i < VertexCount; ++i )
                {
                    FVector3f TangentX = VerticesAsync[i].TangentX.ToFVector3f();
                    FVector3f TangentZ = VerticesAsync[i].TangentZ.ToFVector3f();

                    PositionBuffer.VertexPosition( i ) = VerticesAsync[i].Position;
                    ColorBuffer.VertexColor( i ) = VerticesAsync[i].Color;
                    StaticMeshVB.SetVertexUV( i, 0, VerticesAsync[i].TextureCoordinate[0] );
                    StaticMeshVB.SetVertexTangents( i, TangentX, TangentX.Cross( TangentZ ), TangentZ );
                }

                // Copy RAM to VRAM
                PositionBuffer.UpdateRHI( RHICmdList );
                StaticMeshVB.UpdateRHI( RHICmdList );
                ColorBuffer.UpdateRHI( RHICmdList );

                PositionBuffer.BindPositionVertexBuffer( VertexFactory, Data );
                StaticMeshVB.BindTangentVertexBuffer( VertexFactory, Data );
                StaticMeshVB.BindPackedTexCoordVertexBuffer( VertexFactory, Data );
                ColorBuffer.BindColorVertexBuffer( VertexFactory, Data );

                VertexFactory->SetData( RHICmdList, Data );

                // Init / update the factory after SetData
                if (!VertexFactory->IsInitialized()) {
                    VertexFactory->InitResource(RHICmdList);
                } else {
                    VertexFactory->UpdateRHI(RHICmdList);
                }
            }
            else
            {
                if (VertexFactory->IsInitialized()) VertexFactory->ReleaseResource();
                if (PositionBuffer.IsInitialized()) PositionBuffer.ReleaseResource();
                if (StaticMeshVB.IsInitialized()) StaticMeshVB.ReleaseResource();
                if (ColorBuffer.IsInitialized()) ColorBuffer.ReleaseResource();

                VertexCount = 0;
            }
        } );

    ENQUEUE_RENDER_COMMAND(IndexBufferInit)(
        [ this
        , IndicesAsync = CopyTemp(Indices) ] ( FRHICommandListImmediate& RHICmdList )
        {
            uint32 IndexCount = IndicesAsync.Num();

            if( IndexCount )
            {
                IndexBuffer.SetIndices( IndicesAsync, EIndexBufferStride::Type::Force32Bit );

                if( IndexBuffer.IsInitialized() )
                {
                    IndexBuffer.UpdateRHI( RHICmdList );
                }
                else
                {
                    IndexBuffer.InitResource( RHICmdList );
                }
            }
            else
            {
                if( IndexBuffer.IsInitialized() ) IndexBuffer.ReleaseResource();
            }
        } );
}

///---------------------------------------------------------

FArianeObject::~FArianeObject()
{
    // free mallocated Tags.
    for( FArianeTagID& TagID : Tags )
    {
        FArianeTag* Tag = TagID.GetTag();

        if( Tag->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            delete Tag;
        }
    }
}

FArianeObject::FArianeObject()
    : FArianeObject( nullptr
                   , FName( "Ariane Object" )
                   , EArianeAllocationModel::InstancedStruct
                   , new FArianeObjectInvalidationFlags() )
{
}

FArianeObject::FArianeObject( UArianeLayerDrawing* InDrawingLayer
                            , const FName& InName
                            , EArianeAllocationModel InAllocationModel
                            , FArianeObjectInvalidationFlags* InInvalidationFlags )
    : Name ( InName )
    , Guid ( FGuid::NewGuid() )
    , ParentID ( FArianeObjectID() )
    , DrawingLayer( InDrawingLayer )
    , bVisible ( true )
    , bExpanded ( true )
    , AllocationModel ( InAllocationModel  )
    , InvalidationFlags ( InInvalidationFlags ? InInvalidationFlags
                                              : new FArianeObjectInvalidationFlags() )
    , bSelected ( false )

{
}

FArianeObjectInvalidationFlags&
FArianeObject::GetInvalidationFlags()
{
    return *InvalidationFlags;
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
    ChildToRemove->Update( EUpdateFlags::None, true );
    ChildToRemove->Removed();

    if( ChildToRemove->IsSelected() )
    {
        DrawingLayer->UnselectObject( ChildToRemove );
    }

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

    Child->UpdateTransform();

    InvalidateChild( Child );

    Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
}

void
FArianeObject::PrependChild( FArianeObject* Child )
{
    Children.Insert( FArianeObjectID( Child ), 0 );

    Child->SetParent( this );
    Child->Added();

    Child->UpdateTransform();

    InvalidateChild( Child );

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

    GetDrawingLayer()->MarkPackageDirty();


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

FArianeObject::ETraversalReturnValue
FArianeObject::Traverse_Private( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback )
{
    ETraversalReturnValue Ret = Callback( Object );

    if( Ret == ETraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ( Ret == ETraversalReturnValue::IgnoreChildren ) == 0 )
    {
        for( FArianeObjectID& ChildID : Object->GetChildren() )
        {
            FArianeObject* Child = ChildID.GetObject();
            ETraversalReturnValue ChildRet = Traverse_Private( Child, Callback );

            if( ChildRet == ETraversalReturnValue::Stop )
            {
                return ChildRet;
            }
        }
    }

    return Ret;
}

void
FArianeObject::Traverse( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback )
{
    Traverse_Private( Object, Callback );
}

// static
FArianeObject::ETraversalReturnValue
FArianeObject::TraverseBackwards_Private( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback )
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
FArianeObject::TraverseBackwards( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback )
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
        InvalidatedChildren.RemoveAll( [ &UpdateFlags
                                       , &Recurse](  FArianeObjectID& InvalidatedChildID )
            {
                return InvalidatedChildID.GetObject()->Update( UpdateFlags, Recurse );
            } );
    }

    UpdateBoundingBox( UpdateFlags );

    if( EnumHasAllFlags( UpdateFlags, EUpdateFlags::KeepInvalidated ) == false )
    {
        InvalidationFlags->Clear();
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
    Traverse( this, []( FArianeObject* Object  ) -> ETraversalReturnValue
        {
            UArianeLayerDrawing* DrawingLayer = Object->GetDrawingLayer();
            FArianeObject* Parent = Object->GetParent();

            Object->WorldTransform =  Parent ? Object->LocalTransform * Parent->WorldTransform
                                             : DrawingLayer->GetComponentTransform();

//UE_LOG(LogTemp, Warning, TEXT("Object:%s - Parent:%p - Transform: %s"), *Name.ToString(), Parent, *WorldTransform.ToString());

            return ETraversalReturnValue::Continue;
        } );
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

            if( InsertAfter )
            {
                FArianeObjectID* InsertAfterID = Children.FindByPredicate(
                    [InsertAfter] ( FArianeObjectID& ItemID ) -> bool
                    {
                       if( ItemID.Guid == InsertAfter->GetGuid() )
                       {
                           return true;
                       }

                       return false;
                    } );

                Children.Insert( FosterChild, InsertAfterID - Children.GetData() );
            }
            else
            {
                Children.Add( FosterChild );
            }

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
    FArianeObject* ObjectCopy = CopyArgs.DrawingLayer->AllocObject( Name, CopyArgs.AllocationModel );

    return ObjectCopy;
}

FArianeObject*
FArianeObject::Copy( const FCopyArgs& CopyArgs
                   , TFunction<void( FArianeObject*, const FCopyArgs& )> PreCallback
                   , TFunction<void( FArianeObject*, FArianeObject*, const FCopyArgs& )> PostCallback )
{
    PreCallback( this, CopyArgs );

    FArianeObject* ObjectCopy = CopyShape( CopyArgs );

    if( ObjectCopy )
    {
        CopySettings( ObjectCopy, CopyArgs, true );

        // recurse
        for( FArianeObjectID& ChildID : Children )
        {
            FArianeObject* Child = ChildID.GetObject();
            FCopyArgs ChildCopyArgs = CopyArgs;

            // We only rename the first item of the tree, that's why we reset the flag
            EnumRemoveFlags( ChildCopyArgs.Flags, ECopyFlags::Rename );

            FArianeObject* ChildCopy = Child->Copy( ChildCopyArgs, PreCallback, PostCallback );

            ObjectCopy->AppendChild( ChildCopy );
        }

        // copy tags
        if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::IgnoreTags ) == false )
        {
            for( FArianeTagID& TagID : Tags )
            {
                FArianeTag* Tag = TagID.GetTag();

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
    return DrawingLayer ? DrawingLayer->GetRootGroup() : nullptr;
}

#ifdef WITH_EDITOR
FColor
FArianeObject::GetHUDForegroundColor()
{
    FArianeObject* Group = GetAncestorByClass( FArianeGroup::StaticClass()
                                             , true
                                             , true );

    return Group ? Group->GetHUDForegroundColor()
                 : DrawingLayer->GetLayerStack()->GetPainting3DComponent()->GetHUDForegroundColor();
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
    FArianeObject* Parent = ParentID.GetObject();

    //return ( ( DrawingLayer == nullptr ) || DrawingLayer->IsVisible() == true ) ? true : false;
    return ( bInHierarchical && Parent ) ? bVisible && Parent->IsVisible( bInHierarchical )
                                         : bVisible;
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
    DestObject->LocalTransform = LocalTransform;
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
