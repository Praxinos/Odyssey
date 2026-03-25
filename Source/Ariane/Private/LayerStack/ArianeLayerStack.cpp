// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerStack.h"
#include "ArianeLayerFolder.h"

UArianeLayerStack::~UArianeLayerStack()
{
}

UArianeLayerStack::UArianeLayerStack()
{
    RootLayerFolder = NewObject<UArianeLayerFolder>( this, "Root Layer" );
}

UArianeLayerFolder*
UArianeLayerStack::GetRootLayerFolder()
{
    return RootLayerFolder;
}

void
UArianeLayerStack::RemoveSelectedLayers()
{
    for( UArianeLayer* Layer : SelectedLayers )
    {
        Cast<UArianeLayerFolder>(Layer->GetOuter())->RemoveChild( Layer );
    }
}

void
UArianeLayerStack::SelectAllLayers()
{
    SelectedLayers.Empty();

    SelectLayer( RootLayerFolder, true );
}

void
UArianeLayerStack::SelectLayers( const TArray<UArianeLayer*> LayerSelection, bool bRecurse )
{
    OnPreCurrentLayerChanged.Broadcast();

    for( UArianeLayer* Layer : LayerSelection )
    {
        SelectLayer_Private( Layer, bRecurse );
    }

    OnPreCurrentLayerChanged.Broadcast();
}

void
UArianeLayerStack::SelectLayer( UArianeLayer* Layer, bool bRecurse )
{
    OnPreCurrentLayerChanged.Broadcast();

    SelectLayer_Private( Layer, bRecurse );

    OnPreCurrentLayerChanged.Broadcast();
}

void
UArianeLayerStack::SelectLayer_Private( UArianeLayer* Layer, bool bRecurse )
{
    if( SelectedLayers.Find( Layer ) == INDEX_NONE )
    {
        SelectedLayers.Add( Layer );
    }

    if( bRecurse )
    {
        UArianeLayerFolder* LayerFolder = Cast<UArianeLayerFolder>( Layer );

        if ( LayerFolder )
        {
            for( UArianeLayer* ChildLayer : LayerFolder->GetChildren() )
            {
                SelectLayer_Private( ChildLayer, bRecurse );
            }
        }
    }
}

void
UArianeLayerStack::ClearLayerSelection()
{
    OnPreCurrentLayerChanged.Broadcast();

    SelectedLayers.Empty();

    OnPostCurrentLayerChanged.Broadcast();
}

const TArray<UArianeLayer*>&
UArianeLayerStack::GetSelectedLayers()
{
    return SelectedLayers;
}

UArianeLayer*
UArianeLayerStack::CreateLayer( UArianeLayerFolder* InParentLayerFolder )
{
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootLayerFolder;
    UArianeLayer* NewLayer = NewObject<UArianeLayer>( ParentLayerFolder );

    OnPreLayerStackChanged.Broadcast();

    ParentLayerFolder->AddChild( NewLayer );

    OnPostLayerStackChanged.Broadcast();

    return NewLayer;
}

UArianeLayerFolder*
UArianeLayerStack::CreateLayerFolder( UArianeLayerFolder* InParentLayerFolder )
{
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootLayerFolder;
    UArianeLayerFolder* NewLayerFolder = NewObject<UArianeLayerFolder>( ParentLayerFolder );

    OnPreLayerStackChanged.Broadcast();

    ParentLayerFolder->AddChild( NewLayerFolder );

    OnPostLayerStackChanged.Broadcast();

    return NewLayerFolder;
}

UArianeLayerStack::FOnLayerStackChanged&
UArianeLayerStack::OnPreLayerStackChangedDelegate()
{
    return OnPreLayerStackChanged;
}

UArianeLayerStack::FOnLayerStackChanged&
UArianeLayerStack::OnPostLayerStackChangedDelegate()
{
    return OnPostLayerStackChanged;
}

UArianeLayerStack::FOnCurrentLayerChanged&
UArianeLayerStack::OnPreCurrentLayerChangedDelegate()
{
    return OnPreCurrentLayerChanged;
}

UArianeLayerStack::FOnCurrentLayerChanged&
UArianeLayerStack::OnPostCurrentLayerChangedDelegate()
{
    return OnPostCurrentLayerChanged;
}
