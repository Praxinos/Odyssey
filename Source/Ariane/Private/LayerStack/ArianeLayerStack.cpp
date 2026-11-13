// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerStack.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerVector.h"
#include "ArianeDrawing.h"
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DActor.h"
#include "ArianeGroup.h"

UArianeLayerStack::~UArianeLayerStack()
{
}

UArianeLayerStack::UArianeLayerStack()
{
    FString RootFolderName = FString("Root Folder");

    // init the root folder
    UArianeLayerFolder* RootFolder = CreateDefaultSubobject<UArianeLayerFolder>(*RootFolderName);

    RootFolder->SetDisplayName( RootFolderName );
    RootFolder->SetupAttachment(this);

    Layers.Add( RootFolder );
}

void
UArianeLayerStack::PostDuplicate( EDuplicateMode::Type DuplicateMode )
{
    Super::PostDuplicate( DuplicateMode );
}

void
UArianeLayerStack::Serialize( FArchive& Ar )
{
    Super::Serialize( Ar );
}

UArianePainting3DComponent*
UArianeLayerStack::GetPainting3DComponent()
{
    AArianePainting3DActor* Painting3DActor = Cast<AArianePainting3DActor>(GetOwner());

    return Painting3DActor->GetPainting3DComponent();
}

/*
UArianePainting3DComponent*
UArianeLayerStack::GetPainting3DComponent()
{
    return Cast<UArianePainting3DComponent>(GetOuter());
}
*/

void
UArianeLayerStack::OnRegister()
{
    AArianePainting3DActor* Painting3DActor = Cast<AArianePainting3DActor>(GetOwner());

    Super::OnRegister();

    UArianeLayerFolder* RootFolder = GetRootFolder();

/*
    if( RootFolder == nullptr )
    {
        RootFolder = NewObject<UArianeLayerFolder>( this
                                                  , "Root Folder"
                                                  , RF_Transactional | RF_Public );

        //Painting3DActor->AddInstanceComponent(RootFolder);

        Layers.Add( RootFolder );
    }
*/
    if( RootFolder->GetAttachParent() != this )
    {
        RootFolder->AttachToComponent( this, FAttachmentTransformRules::KeepRelativeTransform );
    }

    RootFolder->RegisterComponent();
}

void
UArianeLayerStack::Init()
{
}

UArianeLayerFolder*
UArianeLayerStack::GetRootFolder()
{
    return Layers.Num() ? Cast<UArianeLayerFolder>(Layers[0]) : nullptr;
}

#if WITH_EDITOR
void
UArianeLayerStack::PreEditUndo()
{
    Super::PreEditUndo();
}

void
UArianeLayerStack::PostEditUndo()
{
    Super::PostEditUndo();
}
#endif

void
UArianeLayerStack::PostLoad()
{
    UArianeLayerFolder* RootFolder = GetRootFolder();

    Super::PostLoad();

    // Patch to clean NULLPTR layers
    Layers.RemoveAll( [] ( const UArianeLayer* Layer )
    {
        return ( Layer == nullptr );
    });
    // End-Patch

    Init();

    OnPreSelectionChanged.Broadcast();
    OnPreHierarchyChanged.Broadcast();

    SelectedLayers.Empty();

    if( RootFolder )
    {
        //fix after a change in the naming of th UArianeLayerVector class.
        //RootFolder->GetChildLayers().Empty();

        // At that step layers are only marked "selected" but not in the list of selected layers
        UArianeLayerFolder::Traverse( RootFolder
                                    , [this]( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
            {
                if( Layer->IsSelected() )
                {
                    SelectedLayers.Add( Layer );
                }

                return UArianeLayerFolder::ETraversalReturnValue::Continue;
            } );
    }

    OnPostSelectionChanged.Broadcast();
    OnPostHierarchyChanged.Broadcast();
}

void
UArianeLayerStack::RemoveSelectedLayers()
{
    Modify();

    for( UArianeLayer* Layer : SelectedLayers )
    {
        Layer->GetParentFolder()->Modify();
        Layer->Modify();

        Layer->GetParentFolder()->RemoveChildLayer( Layer, true );
    }

    ClearLayerSelection( true );
}

void
UArianeLayerStack::UnselectLayers( TArray<UArianeLayer*> LayersToUnselect, bool bTriggerEvent )
{
    Modify();

    if( bTriggerEvent )
        OnPreSelectionChanged.Broadcast();

    for( UArianeLayer* Layer : LayersToUnselect )
    {
        SelectedLayers.Remove( Layer );
    }

    if( bTriggerEvent )
        OnPostSelectionChanged.Broadcast();
}

void
UArianeLayerStack::RemoveLayers( TArray<UArianeLayer*> LayersToRemove, bool bDestroy, bool bTriggerEvent )
{
    Modify();

    if( bTriggerEvent )
        OnPreHierarchyChanged.Broadcast();

    UnselectLayers( LayersToRemove, true );

    for( UArianeLayer* Layer : LayersToRemove )
    {
        Layer->GetParentFolder()->Modify();
        Layer->Modify();

        Layer->GetParentFolder()->RemoveChildLayer( Layer, bDestroy );
    }

    if( bTriggerEvent )
        OnPostHierarchyChanged.Broadcast();
}

uint32
UArianeLayerStack::GetFolderLayerCount()
{
    UArianeLayerFolder* RootFolder = GetRootFolder();
    uint32 FolderLayerCount = 0;

    UArianeLayerFolder::Traverse( RootFolder
                                 , [ &FolderLayerCount ] ( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
    {
        if( Cast<UArianeLayerFolder>(Layer) )
        {
            FolderLayerCount++;
        }

        return UArianeLayerFolder::ETraversalReturnValue::Continue;
    } );

    return FolderLayerCount;
}

uint32
UArianeLayerStack::GetVectorLayerCount()
{
    UArianeLayerFolder* RootFolder = GetRootFolder();
    uint32 VectorLayerCount = 0;

    UArianeLayerFolder::Traverse( RootFolder
                                 , [ &VectorLayerCount ] ( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
    {
        if( Cast<UArianeLayerVector>(Layer) )
        {
            VectorLayerCount++;
        }

        return UArianeLayerFolder::ETraversalReturnValue::Continue;
    } );

    return VectorLayerCount;
}

void
UArianeLayerStack::OnComponentDestroyed( bool bDestroyingHierarchy )
{
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

        Layers.Add( OrphanLayer );
    }

    if( bTriggerEvent )
        OnPostHierarchyChanged.Broadcast();
}

void
UArianeLayerStack::Update( bool bInteractive )
{
    UArianeLayerFolder* RootFolder = GetRootFolder();

    if( RootFolder )
    {
        RootFolder->Update( bInteractive );
    }
}

void
UArianeLayerStack::AddLayer( UArianeLayerFolder* FosterFolder, UArianeLayer* OrphanLayer, bool bTriggerEvent )
{
    AddLayers( FosterFolder, { OrphanLayer}, bTriggerEvent );
}

void
UArianeLayerStack::SelectAllLayers()
{
    UArianeLayerFolder* RootFolder = GetRootFolder();

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
        else // add it at the end of the list, it will become the current layer
        {
            SelectedLayers.Remove( Layer );
            SelectedLayers.Add( Layer );
        }
    }
}

void
UArianeLayerStack::UnselectLayer( UArianeLayer* Layer, bool bTriggerevent )
{
    if( bTriggerevent )
        OnPreSelectionChanged.Broadcast();

    UnselectLayer_Private( Layer );

    if( bTriggerevent )
        OnPostSelectionChanged.Broadcast();
}

void
UArianeLayerStack::UnselectLayer_Private( UArianeLayer* Layer )
{
    if( SelectedLayers.Find( Layer ) != INDEX_NONE )
    {
        SelectedLayers.Remove( Layer );

        Layer->SetSelected( false );
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

UArianeLayerVector*
UArianeLayerStack::CreateVectorLayer( UArianeLayerFolder* InParentLayerFolder, bool bTriggerEvent )
{
    UArianeLayerFolder* RootFolder = GetRootFolder();
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootFolder;
                                                                           // The outer must be the AActor or else the TEDS system could crash
    FString LayerName = FString::Printf(TEXT("Drawing Layer %d"), GetVectorLayerCount() + 1 );
    UArianeLayerVector* NewVectorLayer = NewObject<UArianeLayerVector>( this
                                                                      , NAME_None
                                                                      , RF_Transactional | RF_Public ); // for undos

    // Below 2 lines are mandatory to register the component, otherwise undos won't work (RF_TRANSACTIONAL will be erased)
    //NewVectorLayer->SetupAttachment( ParentLayerFolder );
    //NewVectorLayer->RegisterComponent();

    NewVectorLayer->SetDisplayName( LayerName );

    AddLayer( ParentLayerFolder, NewVectorLayer, bTriggerEvent );

    return NewVectorLayer;
}

UArianeLayerFolder*
UArianeLayerStack::CreateFolderLayer( UArianeLayerFolder* InParentLayerFolder, bool bTriggerEvent )
{
    UArianeLayerFolder* RootFolder = GetRootFolder();
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootFolder;
                                                                        // The outer must be the AActor or else the TEDS system could crash
    FString LayerName = FString::Printf(TEXT("Folder Layer %d"), GetFolderLayerCount() + 1 );
    UArianeLayerFolder* NewLayerFolder = NewObject<UArianeLayerFolder>( this
                                                                      , NAME_None
                                                                      , RF_Transactional | RF_Public  ); // for undos

    // Below 2 lines are mandatory to register the component, otherwise undos won't work (RF_TRANSACTIONAL will be erased)
    //NewLayerFolder->SetupAttachment( ParentLayerFolder );
    //NewLayerFolder->RegisterComponent();

    NewLayerFolder->SetDisplayName( LayerName );

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
