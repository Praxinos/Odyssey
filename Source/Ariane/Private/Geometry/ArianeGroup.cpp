// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeGroup.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"

bool
FArianeGroupInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClass );
}

FArianeGroupInvalidationFlags&
FArianeGroupInvalidationFlags::AND( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeGroupInvalidationFlags::StaticClass() ) )
    {
        //VertexAltered &= ((FArianeGroupInvalidationFlags&)RHS).VertexAltered;
        //SegmentAltered &= ((FArianeGroupInvalidationFlags&)RHS).SegmentAltered;
        //VertexAddedOrRemoved &= ((FArianeGroupInvalidationFlags&)RHS).VertexAddedOrRemoved;
        //SegmentAddedOrRemoved &= ((FArianeGroupInvalidationFlags&)RHS).SegmentAddedOrRemoved;
    }

    Super::AND( RHS );

    return *this;
}

FArianeGroupInvalidationFlags&
FArianeGroupInvalidationFlags::OR( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeGroupInvalidationFlags::StaticClass() ) )
    {
        //VertexAltered |= ((FArianeGroupInvalidationFlags&)RHS).VertexAltered;
        //SegmentAltered |= ((FArianeGroupInvalidationFlags&)RHS).SegmentAltered;
        //VertexAddedOrRemoved |= ((FArianeGroupInvalidationFlags&)RHS).VertexAddedOrRemoved;
        //SegmentAddedOrRemoved |= ((FArianeGroupInvalidationFlags&)RHS).SegmentAddedOrRemoved;
    }

    Super::OR( RHS );

    return *this;
}

FArianeGroupInvalidationFlags&
FArianeGroupInvalidationFlags::SetAll()
{
    //VertexAltered  =
    //SegmentAltered =
    //VertexAddedOrRemoved  =
    //SegmentAddedOrRemoved = 1;

    Super::SetAll();

    return *this;
}

FArianeGroupInvalidationFlags&
FArianeGroupInvalidationFlags::Clear()
{
    Super::Clear();

    //VertexAltered  =
    //SegmentAltered =
    //VertexAddedOrRemoved  =
    //SegmentAddedOrRemoved = 0;

    return *this;
}

bool
FArianeGroupInvalidationFlags::HasAny()
{
    return Super::HasAny();
    //return ( VertexAltered
    //      || SegmentAltered
    //      || VertexAddedOrRemoved
    //      || SegmentAddedOrRemoved ) ? true : Super::HasAny();
}

//--------------------- Group

FArianeGroup::~FArianeGroup()
{
}

FArianeGroup::FArianeGroup()
    : FArianeObject()
#if WITH_EDITOR
    , HUDForegroundColor( FColor::Black )
    , bUseEditorHUDForegroundColor( true )
#endif
{
    InvalidationFlags = new FArianeGroupInvalidationFlags();

    // Default material interface. color only
    //MaterialInterface = GEngine->VertexColorMaterial;
}

FArianeGroup::FArianeGroup( UArianeLayerDrawing* InDrawingLayer
                          , const FName& InName
                          , EArianeAllocationModel InAllocationModel   )
    : FArianeObject ( InDrawingLayer, InName, InAllocationModel )
#if WITH_EDITOR
    , HUDForegroundColor( FColor::Black )
    , bUseEditorHUDForegroundColor( true )
#endif
{
    InvalidationFlags = new FArianeGroupInvalidationFlags();

    // Default material interface. color only
    //MaterialInterface = GEngine->VertexColorMaterial;
}

bool
FArianeGroup::HasBaseClass( uint32 BaseClass )
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass(BaseClass);
}

void
FArianeGroup::CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate )
{
    FArianeGroup* DestinationGroup = static_cast<FArianeGroup*>(DestinationObject);

    Super::CopySettings( DestinationObject, CopyArgs, false );

    DestinationGroup->HUDForegroundColor = HUDForegroundColor;
    DestinationGroup->bUseEditorHUDForegroundColor = bUseEditorHUDForegroundColor;

    if( bInvalidate )
    {
        DestinationGroup->Invalidate( FArianeGroupInvalidationFlags().SetAll() );
    }
}

FArianeGroup*
FArianeGroup::CopyShape( const FCopyArgs& CopyArgs )
{
    FArianeGroup* GroupCopy = nullptr;

    if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::AllocByOperatingSystem ) )
    {
        GroupCopy = new FArianeGroup( nullptr
                                    , Name
                                    , EArianeAllocationModel::OperatingSystem );
    }

    if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::AllocAsInstancedStruct ) )
    {
        GroupCopy = CopyArgs.DrawingLayer->AllocGroup( Name );
    }

    return GroupCopy;
}

/*
void
FArianeGroup::PostLoad()
{
    //if( Geometry3D == nullptr )
    {
        //Geometry3D = new FArianePathGeometry3D( this );

        InvalidationFlags = new FArianeGroupInvalidationFlags();

        Material = NewObject<UMaterial>();
    }
}

void
FArianeGroup::Added()
{
}

void
FArianeGroup::Removed()
{
}

void
FArianeGroup::PostEditUndo()
{
    Super::PostEditUndo();
}

void
FArianeGroup::PostLoad()
{
    Super::PostLoad();
}

bool
FArianeGroup::Update( bool Recurse, bool bClearFlags )
{
    FArianeObject::Update( Recurse , false );
}
*/

#if WITH_EDITOR
void
FArianeGroup::SetHUDForegroundColor( const FColor& InHUDForegroundColor )
{
    HUDForegroundColor = InHUDForegroundColor;
}

void
FArianeGroup::UseEditorHUDForegroundColor( bool bInUseEditorHUDForegroundColor )
{
    bUseEditorHUDForegroundColor = bInUseEditorHUDForegroundColor;
}

FColor
FArianeGroup::GetHUDForegroundColor()
{
    return bUseEditorHUDForegroundColor ? DrawingLayer->GetLayerStack()->GetPainting3DComponent()->GetHUDForegroundColor()
                                        : HUDForegroundColor;
}

#endif
