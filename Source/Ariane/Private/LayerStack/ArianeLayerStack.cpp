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

    SelectLayer( NewDrawingLayer, true );
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

#if WITH_EDITOR
void
UArianeLayerStack::PreEditUndo()
{
    // trigger an event
    OnPreHierarchyChanged.Broadcast();

    Super::PreEditUndo();
}

void
UArianeLayerStack::PostEditUndo()
{
    Super::PostEditUndo();

    // trigger an event
    OnPostHierarchyChanged.Broadcast();
}
#endif

void
UArianeLayerStack::PostLoad()
{
    Super::PostLoad();

    SelectLayer( RootFolder->GetChildLayers()[0], true );
}

void
UArianeLayerStack::RemoveSelectedLayers()
{
    Modify();

    for( UArianeLayer* Layer : SelectedLayers )
    {
        Layer->Modify();

        Layer->GetParentFolder()->RemoveChildLayer( Layer );
    }

    ClearLayerSelection( true );
}

void
UArianeLayerStack::GetLayers( TArray<UArianeLayer*>& OutLayers )
{
    RootFolder->Traverse( [ &OutLayers ] ( UArianeLayer* Layer ) -> UArianeLayerFolder::TraversalReturnValue
    {
        OutLayers.Add( Layer );

        return UArianeLayerFolder::TraversalReturnValue::Continue;
    } );
}

void
UArianeLayerStack::OnComponentDestroyed( bool bDestroyingHierarchy )
{
    TArray<UArianeLayer*> Layers;

    GetLayers( Layers );
/*
    for ( UArianeLayer* Layer : Layers )
    {
        Layer->Destroy();
    }
*/
    Super::OnComponentDestroyed( bDestroyingHierarchy );
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
                               , bool bTriggerevent )
{
    if( bTriggerevent )
        OnPreSelectionChanged.Broadcast();

    if( bClearSelectionFirst )
        ClearLayerSelection( false );

    for( UArianeLayer* Layer : LayerSelection )
    {
        SelectLayer_Private( Layer );
    }

    if( bTriggerevent )
        OnPostSelectionChanged.Broadcast();
}

void
UArianeLayerStack::SelectLayer( UArianeLayer* Layer, bool bTriggerevent )
{
    if( bTriggerevent )
        OnPreSelectionChanged.Broadcast();

    SelectLayer_Private( Layer );

    if( bTriggerevent )
        OnPostSelectionChanged.Broadcast();
}

void
UArianeLayerStack::SelectLayer_Private( UArianeLayer* Layer )
{
    // root folder cannot be selected
    //if( Cast<UArianeLayer>(RootFolder) != Layer )
    {
        if( SelectedLayers.Find( Layer ) == INDEX_NONE )
        {
            SelectedLayers.Add( Layer );

            Layer->SetSelected( true );
        }
    }
}

UArianeLayer*
UArianeLayerStack::GetCurrentLayer()
{
    return SelectedLayers.Num() ? SelectedLayers.Last() : nullptr;
}

void
UArianeLayerStack::ClearLayerSelection( bool bTriggerEvent )
{
    if( bTriggerEvent )
        OnPreSelectionChanged.Broadcast();

    SelectedLayers.RemoveAll([] ( UArianeLayer* Layer )
                             {
                                 Layer->SetSelected( false );

                                 return true;
                             });

    if( bTriggerEvent )
        OnPostSelectionChanged.Broadcast();
}

const TArray<UArianeLayer*>&
UArianeLayerStack::GetSelectedLayers()
{
    return SelectedLayers;
}

UArianeLayerDrawing*
UArianeLayerStack::CreateDrawingLayer( UArianeLayerFolder* InParentLayerFolder, bool bTriggerEvent )
{
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootFolder;
                                                                           // The outer must be the AActor or else the TEDS system could crash
    UArianeLayerDrawing* NewDrawingLayer = NewObject<UArianeLayerDrawing>( GetPainting3DComponent()->GetOwner()
                                                                         , NAME_None
                                                                         , RF_Transactional ); // for undos

    // Below 2 lines are mandatory to register the component, otherwise undos won't work (RF_TRANSACTIONAL will be erased)
    NewDrawingLayer->SetupAttachment( ParentLayerFolder );
    NewDrawingLayer->RegisterComponent();

    if( bTriggerEvent )
        OnPreHierarchyChanged.Broadcast();

    ParentLayerFolder->AddChildLayer( NewDrawingLayer );

    if( bTriggerEvent )
        OnPostHierarchyChanged.Broadcast();

    return NewDrawingLayer;
}

UArianeLayerFolder*
UArianeLayerStack::CreateFolderLayer( UArianeLayerFolder* InParentLayerFolder, bool bTriggerEvent )
{
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootFolder;
                                                                        // The outer must be the AActor or else the TEDS system could crash
    UArianeLayerFolder* NewLayerFolder = NewObject<UArianeLayerFolder>( GetPainting3DComponent()->GetOwner()
                                                                      , NAME_None
                                                                      , RF_Transactional ); // for undos

    // Below 2 lines are mandatory to register the component, otherwise undos won't work (RF_TRANSACTIONAL will be erased)
    NewLayerFolder->SetupAttachment( ParentLayerFolder );
    NewLayerFolder->RegisterComponent();

    if( bTriggerEvent )
        OnPreHierarchyChanged.Broadcast();

    ParentLayerFolder->AddChildLayer( NewLayerFolder );

    if( bTriggerEvent )
        OnPostHierarchyChanged.Broadcast();

    return NewLayerFolder;
}

UArianeLayerStack::FOnHierarchyChanged&
UArianeLayerStack::OnPreHierarchyChangedDelegate()
{
    return OnPreHierarchyChanged;
}

UArianeLayerStack::FOnHierarchyChanged&
UArianeLayerStack::OnPostHierarchyChangedDelegate()
{
    return OnPostHierarchyChanged;
}

UArianeLayerStack::FOnSelectionChanged&
UArianeLayerStack::OnPreSelectionChangedDelegate()
{
    return OnPreSelectionChanged;
}

UArianeLayerStack::FOnSelectionChanged&
UArianeLayerStack::OnPostSelectionChangedDelegate()
{
    return OnPostSelectionChanged;
}
