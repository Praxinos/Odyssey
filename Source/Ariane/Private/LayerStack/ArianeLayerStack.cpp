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

// Legacy compatibility
UArianeLayerStack::UArianeLayerStack()
{
    // Do not load the default drawing layer. this was a bad legacy design
    //ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("Drawing Layer"));

    // init the root folder
    //RootFolder = CreateDefaultSubobject<UArianeLayerFolder>(TEXT("Root Folder"));
    //RootFolder->SetupAttachment(this);
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

    if( RootFolder == nullptr )
    {
        RootFolder = NewObject<UArianeLayerFolder>( this
                                                  , "Root Folder"
                                                  , RF_Transactional | RF_Public );

        RootFolder->AttachToComponent( this,  FAttachmentTransformRules::KeepWorldTransform );

        Painting3DActor->AddInstanceComponent(RootFolder);

        Layers.Add( RootFolder );
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
    // trigger an event
    OnPreHierarchyChanged.Broadcast();

    Super::PreEditUndo();
}

void
UArianeLayerStack::PostEditUndo()
{
    UArianeLayerFolder* RootFolder = GetRootFolder();

    Super::PostEditUndo();

    OnPreSelectionChanged.Broadcast();
    OnPreHierarchyChanged.Broadcast();

    SelectedLayers.Empty();

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

    OnPostSelectionChanged.Broadcast();
    OnPostHierarchyChanged.Broadcast();
}
#endif

void
UArianeLayerStack::PostLoad()
{
    UArianeLayerFolder* RootFolder = GetRootFolder();

    Super::PostLoad();

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

        Layer->GetParentFolder()->RemoveChildLayer( Layer );
    }

    ClearLayerSelection( true );
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
    FName LayerName = FName( *FString::Printf(TEXT("Drawing Layer %d"), GetVectorLayerCount() + 1 ) );
    UArianeLayerVector* NewVectorLayer = NewObject<UArianeLayerVector>( this
                                                                         , LayerName
                                                                         , RF_Transactional | RF_Public ); // for undos

    // Below 2 lines are mandatory to register the component, otherwise undos won't work (RF_TRANSACTIONAL will be erased)
    //NewVectorLayer->SetupAttachment( ParentLayerFolder );
    //NewVectorLayer->RegisterComponent();

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
    UArianeLayerFolder* NewLayerFolder = NewObject<UArianeLayerFolder>( this
                                                                      , NAME_None
                                                                      , RF_Transactional | RF_Public  ); // for undos

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
