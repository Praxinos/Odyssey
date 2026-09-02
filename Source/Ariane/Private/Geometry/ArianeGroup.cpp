// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeGroup.h"
#include "ArianeImage.h"
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
        HUDForegroundColor &= ((FArianeGroupInvalidationFlags&)RHS).HUDForegroundColor;
    }

    Super::AND( RHS );

    return *this;
}

FArianeGroupInvalidationFlags&
FArianeGroupInvalidationFlags::OR( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeGroupInvalidationFlags::StaticClass() ) )
    {
        HUDForegroundColor |= ((FArianeGroupInvalidationFlags&)RHS).HUDForegroundColor;
    }

    Super::OR( RHS );

    return *this;
}

FArianeGroupInvalidationFlags&
FArianeGroupInvalidationFlags::SetAll()
{
    HUDForegroundColor = 1;

    Super::SetAll();

    return *this;
}

FArianeGroupInvalidationFlags&
FArianeGroupInvalidationFlags::Clear()
{
    Super::Clear();

    HUDForegroundColor= 0;

    return *this;
}

bool
FArianeGroupInvalidationFlags::HasAny()
{
    return ( HUDForegroundColor ) ? true : Super::HasAny();
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

FArianeGroup::FArianeGroup( UArianeImage* InImage
                          , const FName& InName
                          , EArianeAllocationModel InAllocationModel   )
    : FArianeObject ( InImage, InName, InAllocationModel )
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

    GroupCopy = CopyArgs.Image->AllocGroup( Name, CopyArgs.AllocationModel );


    return GroupCopy;
}

void
FArianeGroup::UpdateBoundingBox( EUpdateFlags UpdateFlags )
{
    FArianeGroupInvalidationFlags* GroupInvalidationFlags = static_cast<FArianeGroupInvalidationFlags*>(InvalidationFlags);

    if( ( GroupInvalidationFlags->Hierarchy )
     || ( GroupInvalidationFlags->Children  ) )
    {
        // FBox(ForceInit) creates an invalid box
        FBox CombinedBox(ForceInit);

        for (FArianeObjectID& ChildID : Children)
        {
            FArianeObject* Child = ChildID.GetObject();

            // Note: use "+=" and not "bound = bound + blah". Only += checks for the validity of the box.
            CombinedBox += Child->GetBoundingBox().TransformBy( Child->GetLocalTransform() );
        }

        BoundingBox = CombinedBox.IsValid ? FBox(CombinedBox)
                                          : FBox(ForceInit);
    }
}

void
FArianeGroup::UpdateShape( EUpdateFlags UpdateFlags )
{
    Super::UpdateShape( UpdateFlags );
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

    Invalidate( FArianeGroupInvalidationFlags().SetHUDForegroundColor() );
}

void
FArianeGroup::UseEditorHUDForegroundColor( bool bInUseEditorHUDForegroundColor )
{
    bUseEditorHUDForegroundColor = bInUseEditorHUDForegroundColor;
}

FColor
FArianeGroup::GetHUDForegroundColor()
{
    return bUseEditorHUDForegroundColor ? Image->GetDrawingLayer()->GetLayerStack()->GetPainting3DComponent()->GetHUDForegroundColor()
                                        : HUDForegroundColor;
}

#endif
