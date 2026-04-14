// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerStack.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerDrawing.h"
#include "ArianePainting3DComponent.h"

UArianeLayerStack::~UArianeLayerStack()
{
}

UArianeLayerStack::UArianeLayerStack()
    : RootFolder ( nullptr )
{
    // Create the default Root folder.
    RootFolder = CreateDefaultSubobject<UArianeLayerFolder>( "Root Folder" );
    RootFolder->SetupAttachment(this);

    // Create the default Drawing Layer folder.
    UArianeLayerDrawing* NewDrawingLayer = CreateDefaultSubobject<UArianeLayerDrawing>( "Drawing Layer" );

    RootFolder->AddChildLayer( NewDrawingLayer );
}

UArianePainting3DComponent*
UArianeLayerStack::GetPainting3DComponent()
{
    return Cast<UArianePainting3DComponent>(GetOuter());
}

UArianeLayerFolder*
UArianeLayerStack::GetRootFolder()
{
    return RootFolder;
}

void
UArianeLayerStack::RemoveSelectedLayers()
{
    for( UArianeLayer* Layer : SelectedLayers )
    {
        Cast<UArianeLayerFolder>(Layer->GetOuter())->RemoveChildLayer( Layer );
    }

    ClearLayerSelection( true );
}

void
UArianeLayerStack::GetLayers( TArray<UArianeLayer*>& Layers )
{
    RootFolder->Traverse( [ &Layers ] ( UArianeLayer* Layer ) -> UArianeLayerFolder::TraversalReturnValue
    {
        Layers.Add( Layer );

        return UArianeLayerFolder::TraversalReturnValue::Continue;
    } );
}

void
UArianeLayerStack::OnComponentDestroyed()
{
    TArray<UArianeLayer*> Layers;

    GetLayers( Layers );
/*
    for ( UArianeLayer* Layer : Layers )
    {
        Layer->Destroy();
    }
*/
}

void
UArianeLayerStack::SelectAllLayers()
{
    SelectedLayers.Empty();

    SelectLayer( RootFolder, true );
}

void
UArianeLayerStack::SelectLayers( const TArray<UArianeLayer*> LayerSelection
                               , bool bClearSelectionFirst
                               , bool bTriggerevent
                               , bool bRecurse )
{
    if( bTriggerevent )
        OnPreLayerSelectionChanged.Broadcast();

    if( bClearSelectionFirst )
        ClearLayerSelection( false );

    for( UArianeLayer* Layer : LayerSelection )
    {
        SelectLayer_Private( Layer, bRecurse );
    }

    if( bTriggerevent )
        OnPostLayerSelectionChanged.Broadcast();
}

void
UArianeLayerStack::SelectLayer( UArianeLayer* Layer, bool bTriggerevent, bool bRecurse )
{
    if( bTriggerevent )
        OnPreLayerSelectionChanged.Broadcast();

    SelectLayer_Private( Layer, bRecurse );

    if( bTriggerevent )
        OnPostLayerSelectionChanged.Broadcast();
}

void
UArianeLayerStack::SelectLayer_Private( UArianeLayer* Layer, bool bRecurse )
{
    // root folder cannot be selected
    //if( Cast<UArianeLayer>(RootFolder) != Layer )
    {
        if( SelectedLayers.Find( Layer ) == INDEX_NONE )
        {
            SelectedLayers.Add( Layer );

            Layer->SetSelected( true );
        }

        if( bRecurse )
        {
            UArianeLayerFolder* LayerFolder = Cast<UArianeLayerFolder>( Layer );

            if ( LayerFolder )
            {
                for( UArianeLayer* ChildLayer : LayerFolder->GetChildLayers() )
                {
                    SelectLayer_Private( ChildLayer, bRecurse );
                }
            }
        }
    }
}

UArianeLayer*
UArianeLayerStack::GetCurrentLayer()
{
    return SelectedLayers.Num() ? SelectedLayers[0] : nullptr;
}

void
UArianeLayerStack::ClearLayerSelection( bool bTriggerEvent )
{
    if( bTriggerEvent )
        OnPreLayerSelectionChanged.Broadcast();

    SelectedLayers.RemoveAll([] ( UArianeLayer* Layer )
                             {
                                 Layer->SetSelected( false );

                                 return true;
                             });

    if( bTriggerEvent )
        OnPostLayerSelectionChanged.Broadcast();
}

const TArray<UArianeLayer*>&
UArianeLayerStack::GetSelectedLayers()
{
    return SelectedLayers;
}

UArianeLayerDrawing*
UArianeLayerStack::GetFirstSelectedDrawingLayer()
{
    for( UArianeLayer* Layer : SelectedLayers )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( Layer );

        if( DrawingLayer )
        {
            return DrawingLayer;
        }
    }

    return nullptr;
}

UArianeLayerDrawing*
UArianeLayerStack::CreateDrawingLayer( UArianeLayerFolder* InParentLayerFolder )
{
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootFolder;
    UArianeLayerDrawing* NewDrawingLayer = NewObject<UArianeLayerDrawing>( this // The outer is the LayerStack
                                                                         , NAME_None
                                                                         , RF_Transactional ); // for undos

    //NewDrawingLayer->SetupAttachment( ParentLayerFolder );
    //NewDrawingLayer->RegisterComponent();

    OnPreLayerStackChanged.Broadcast();

    ParentLayerFolder->AddChildLayer( NewDrawingLayer );

    OnPostLayerStackChanged.Broadcast();

    return NewDrawingLayer;
}

UArianeLayerFolder*
UArianeLayerStack::CreateFolderLayer( UArianeLayerFolder* InParentLayerFolder )
{
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootFolder;
    UArianeLayerFolder* NewLayerFolder = NewObject<UArianeLayerFolder>( this // The outer is the LayerStack
                                                                      , NAME_None
                                                                      , RF_Transactional ); // for undos

    // Below 2 lines are mandatory to register the component, otherwise undos won't work (RF_TRANSACTIONAL will be erased)
    NewLayerFolder->SetupAttachment( ParentLayerFolder );
    NewLayerFolder->RegisterComponent();

    OnPreLayerStackChanged.Broadcast();

    ParentLayerFolder->AddChildLayer( NewLayerFolder );

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

UArianeLayerStack::FOnLayerSelectionChanged&
UArianeLayerStack::OnPreLayerSelectionChangedDelegate()
{
    return OnPreLayerSelectionChanged;
}

UArianeLayerStack::FOnLayerSelectionChanged&
UArianeLayerStack::OnPostLayerSelectionChangedDelegate()
{
    return OnPostLayerSelectionChanged;
}
