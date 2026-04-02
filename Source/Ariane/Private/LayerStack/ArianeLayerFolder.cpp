// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerFolder.h"

UArianeLayerFolder::~UArianeLayerFolder()
{
}

UArianeLayerFolder::UArianeLayerFolder()
    : bExpanded ( true )
{
/*
    ResetHierarchy();

    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;
*/
}

void
UArianeLayerFolder::SetExpanded( bool bInExpanded )
{
    bExpanded = bInExpanded;
}

bool
UArianeLayerFolder::IsExpanded()
{
    return bExpanded;
}

const TArray<UArianeLayer*>&
UArianeLayerFolder::GetChildren()
{
    return Children;
}

void
UArianeLayerFolder::AddChild( UArianeLayer* Orphan )
{
    // Rename() is used to define the parent object
    Orphan->Rename( nullptr, this );

    Children.Add( Orphan );
}

void
UArianeLayerFolder::RemoveChild( UArianeLayer* Child )
{
    Children.Remove( Child );

    InvalidatedChildren.Remove( Child );

    // Rename() is used to define the parent object
    Child->Rename( nullptr, nullptr );
}


UArianeLayerFolder::TraversalReturnValue
UArianeLayerFolder::Traverse_Private( TFunction<TraversalReturnValue(UArianeLayer*)> Callback )
{
    TraversalReturnValue Ret = Callback( this );

    if( Ret == TraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ( Ret == TraversalReturnValue::IgnoreChildren ) == 0 )
    {
        for( UArianeLayer* Child : Children )
        {
            UArianeLayerFolder* ChildFolder = Cast<UArianeLayerFolder>(Child);
            TraversalReturnValue ChildRet = ChildFolder ? ChildFolder->Traverse_Private( Callback )
                                                        : Callback( Child );

            if( ChildRet == TraversalReturnValue::Stop )
            {
                return ChildRet;
            }
        }
    }

    return Ret;
}

void
UArianeLayerFolder::Traverse( TFunction<TraversalReturnValue(UArianeLayer*)> Callback )
{
    Traverse_Private( Callback );
}

void
UArianeLayerFolder::InvalidateChild( UArianeLayer* Child )
{
    if( Child->IsInvalidated() == false )
    {
        InvalidatedChildren.Add( Child );

        Child->SetInvalidated( true );
    }
}

void
UArianeLayerFolder::Update()
{
    InvalidatedChildren.RemoveAll( [] ( UArianeLayer* Layer ) -> bool
    {
        Layer->Update();

        return ( Layer->IsInvalidated() == false );
    } );

    UpdateBounds();

    Super::Update();
}

void
UArianeLayerFolder::UpdateBounds()
{
    // ForceInit makes the box invalid and excludes it from the computation unitl it is valid
    Bounds = FBoxSphereBounds(ForceInit);

    for( UArianeLayer* Child : Children )
    {
        Bounds = Bounds + Child->GetBounds();
    }
}
