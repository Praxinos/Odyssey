// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerStack.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerDrawing.h"
#include "ArianeImage.h"
#include "ArianePainting3DComponent.h"
#include "ArianeGroup.h"

UArianeLayerStack::~UArianeLayerStack()
{
}

// Legacy compatibility
UArianeLayerStack::UArianeLayerStack()
    : RootFolder ( nullptr )
{
    // Do not load the default drawing layer. this was a bad legacy design
    //ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("Drawing Layer"));

    // init the root folder
    RootFolder = CreateDefaultSubobject<UArianeLayerFolder>(TEXT("Root Folder"));
    RootFolder->SetupAttachment(this);
}

void
UArianeLayerStack::Serialize( FArchive& Ar )
{
    Super::Serialize(Ar);
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

    //SelectLayer( RootFolder->GetChildLayers()[0], true );
    OnPostHierarchyChanged.Broadcast();
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
    RootFolder->Traverse( [ &OutLayers ] ( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
    {
        OutLayers.Add( Layer );

        return UArianeLayerFolder::ETraversalReturnValue::Continue;
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
UArianeLayerStack::AddLayers( UArianeLayerFolder* FosterFolder, TArray<UArianeLayer*> OrphanLayers, bool bTriggerEvent )
{
    if( bTriggerEvent )
        OnPreHierarchyChanged.Broadcast();

    FosterFolder->Modify();

    for( UArianeLayer* OrphanLayer : OrphanLayers )
    {
        FosterFolder->AddChildLayer( OrphanLayer );
    }

    if( bTriggerEvent )
        OnPostHierarchyChanged.Broadcast();
}

void
UArianeLayerStack::AddLayer( UArianeLayerFolder* FosterFolder, UArianeLayer* OrphanLayer, bool bTriggerEvent )
{
    AddLayers( FosterFolder, { OrphanLayer}, bTriggerEvent );
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
    //NewDrawingLayer->SetupAttachment( ParentLayerFolder );
    //NewDrawingLayer->RegisterComponent();

    AddLayer( ParentLayerFolder, NewDrawingLayer, bTriggerEvent );

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
    //NewLayerFolder->SetupAttachment( ParentLayerFolder );
    //NewLayerFolder->RegisterComponent();

    AddLayer( ParentLayerFolder, NewLayerFolder, bTriggerEvent );

    return NewLayerFolder;
}

void
UArianeLayerStack::AppendSelectedTrees( TArray<UArianeLayer*>& SelectedTrees )
{
    SelectedTrees.Reserve( SelectedTrees.Num() + SelectedLayers.Num() );

    for( UArianeLayer* SelectedLayer : SelectedLayers )
    {
        bool bHasSelectedAncestor = false;

        // Group only objects that have no selected ancestors
        SelectedLayer->TraverseBackwards (
            [ SelectedLayer
            , &bHasSelectedAncestor
            , &SelectedTrees ]( UArianeLayer* TraversedLayer ) -> UArianeLayerFolder::ETraversalReturnValue
            {
                if( TraversedLayer != SelectedLayer )
                {
                    if( TraversedLayer->IsSelected() )
                    {
                        bHasSelectedAncestor = true;

                        return UArianeLayerFolder::ETraversalReturnValue::Stop;
                    }
                }

                return UArianeLayerFolder::ETraversalReturnValue::Continue;
            } );

        if( bHasSelectedAncestor == false )
        {
            SelectedTrees.Add( SelectedLayer );
        }
    }
}

void
UArianeLayerStack::GetSelectedTrees( TArray<UArianeLayer*>& SelectedTrees )
{
    SelectedTrees.Empty();

    AppendSelectedTrees( SelectedTrees );
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
