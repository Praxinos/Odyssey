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
{
    RootFolder = NewObject<UArianeLayerFolder>( this, "Root Folder" );

    // Create a default drawing layer
    UArianeLayerDrawing* DrawingLayer = NewObject<UArianeLayerDrawing>( RootFolder, "Drawing Layer" );

    RootFolder->AddChild( DrawingLayer );
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
        Cast<UArianeLayerFolder>(Layer->GetOuter())->RemoveChild( Layer );
    }

    ClearLayerSelection();
}

void
UArianeLayerStack::SelectAllLayers()
{
    SelectedLayers.Empty();

    SelectLayer( RootFolder, true );
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
    // root folder cannot be selected
    if( Cast<UArianeLayer>(RootFolder) != Layer )
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
                for( UArianeLayer* ChildLayer : LayerFolder->GetChildren() )
                {
                    SelectLayer_Private( ChildLayer, bRecurse );
                }
            }
        }
    }
}

void
UArianeLayerStack::ClearLayerSelection()
{
    OnPreCurrentLayerChanged.Broadcast();

    SelectedLayers.RemoveAll([] ( UArianeLayer* Layer )
                             {
                                 Layer->SetSelected( false );

                                 return true;
                             });

    OnPostCurrentLayerChanged.Broadcast();
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
    UArianeLayerDrawing* NewDrawingLayer = NewObject<UArianeLayerDrawing>( ParentLayerFolder );

    OnPreLayerStackChanged.Broadcast();

    ParentLayerFolder->AddChild( NewDrawingLayer );

    OnPostLayerStackChanged.Broadcast();

    return NewDrawingLayer;
}

UArianeLayerFolder*
UArianeLayerStack::CreateFolderLayer( UArianeLayerFolder* InParentLayerFolder )
{
    UArianeLayerFolder* ParentLayerFolder = InParentLayerFolder ? InParentLayerFolder
                                                                : RootFolder;
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
