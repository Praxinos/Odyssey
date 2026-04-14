// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "SArianeEditorLayerStackPanel.h"
#include "SArianeEditorLayerStack.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerStack.h"
#include "ArianeLayer.h"
#include "ArianeLayerFolder.h"
// Unreal Headers
#include "IStructureDetailsView.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

SArianeEditorLayerStackPanel::~SArianeEditorLayerStackPanel()
{}

SArianeEditorLayerStackPanel::SArianeEditorLayerStackPanel()
{
}

void
SArianeEditorLayerStackPanel::OnPre3DPaintingComponentSelectionChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        LayerStack->OnPostLayerSelectionChangedDelegate().RemoveAll( this );
        // Refresh the details view
        OnLayerSelectionChanged();
    }
}

void
SArianeEditorLayerStackPanel::OnPost3DPaintingComponentSelectionChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        LayerStack->OnPostLayerSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStackPanel::OnLayerSelectionChanged );
        // Refresh the details view
        OnLayerSelectionChanged();
    }
}

void
SArianeEditorLayerStackPanel::OnLayerSelectionChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    LayerDetailsView->SetObject( nullptr );

    UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
    const TArray<UArianeLayer*>& SelectedLayers = LayerStack->GetSelectedLayers();

    for( UArianeLayer* Layer : SelectedLayers )
    {
        if( Layer->GetClass() == UArianeLayerDrawing::StaticClass() )
        {
            LayerDrawingView->ImportLayerProperties( Layer );
            LayerDetailsView->SetObject( LayerDrawingView );
        }
        else
        {
            LayerView->ImportLayerProperties( Layer );
            LayerDetailsView->SetObject( LayerView );
        }

        break;
    }
}

void
SArianeEditorLayerStackPanel::Construct(const FArguments& InArgs, FArianeEditor* InEditor)
{
    FPropertyEditorModule& EditorModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    UArianePainting3DComponent* Painting3DComponent = InEditor->GetCurrentPainting3DComponent();
    FDetailsViewArgs DetailsViewArgs;

    Editor = InEditor;

    LayerView = NewObject<UArianeEditorLayerView>();
    LayerDrawingView = NewObject<UArianeEditorLayerDrawingView>();

    LayerView->SetEditor( Editor );
    LayerDrawingView->SetEditor( Editor );

    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    LayerDetailsView = EditorModule.CreateDetailView( DetailsViewArgs );
    LayerDetailsView->SetObject( nullptr );

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            SNew( SButton )
                .Text( LOCTEXT("layer-stack-panel-new-layer","New Layer") )
                .OnClicked( FOnClicked::CreateSP( this, &SArianeEditorLayerStackPanel::NewLayer ) )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .AutoSize()
            [
                SNew( SArianeEditorLayerStack, Editor )
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            LayerDetailsView.ToSharedRef()
        ]
    ];

    Editor->OnPre3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStackPanel::OnPre3DPaintingComponentSelectionChanged );
    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStackPanel::OnPost3DPaintingComponentSelectionChanged );

    if( Painting3DComponent )
    {
        OnLayerSelectionChanged(); // First call at Widget creation (if we are in a floating tab for example)
    }
}

FReply
SArianeEditorLayerStackPanel::NewLayer()
{
    UArianePainting3DComponent* CurrentPainting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( CurrentPainting3DComponent )
    {
        CurrentPainting3DComponent->GetLayerStack()->CreateDrawingLayer( nullptr );
    }

    return FReply::Handled();
}

void
SArianeEditorLayerStackPanel::AddReferencedObjects( FReferenceCollector& Collector )
{
    // Prevent UObjects from being garbage collected
    Collector.AddReferencedObject( LayerView );
    Collector.AddReferencedObject( LayerDrawingView );
}

FString
SArianeEditorLayerStackPanel::GetReferencerName() const
{
    return FString( "SArianeEditorLayerStackPanel" );
}

#undef LOCTEXT_NAMESPACE
