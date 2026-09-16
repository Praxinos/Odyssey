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
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

// this will filter what we choose to display for the UArianeLayer class and derived classes
class FArianeLayerFilteredDetails : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShared<FArianeLayerFilteredDetails>();
    }

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override
    {
        TArray<FName> CategoryNames;

        DetailBuilder.GetCategoryNames(CategoryNames);

        // We only display some sections in that view and not the whole parameters of a UMeshComponent
        static const TSet<FName> Keep = { "DrawingLayerOptions", "TransformCommon" };

        for (const FName& Category : CategoryNames)
        {
            if (!Keep.Contains(Category))
            {
                DetailBuilder.HideCategory(Category);
            }
        }
    }
};

SArianeEditorLayerStackPanel::~SArianeEditorLayerStackPanel()
{
}

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

        LayerStack->OnPreSelectionChangedDelegate().RemoveAll( this );
        LayerStack->OnPostSelectionChangedDelegate().RemoveAll( this );
    }
}

void
SArianeEditorLayerStackPanel::OnPost3DPaintingComponentSelectionChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        LayerStack->OnPreSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStackPanel::OnPreLayerSelectionChanged );
        LayerStack->OnPostSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStackPanel::OnPostLayerSelectionChanged );
        // Refresh the details view now
        OnPostLayerSelectionChanged();
    }
}

void
SArianeEditorLayerStackPanel::OnPreLayerSelectionChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
    const TArray<UArianeLayer*>& SelectedLayers = LayerStack->GetSelectedLayers();

    for( UArianeLayer* Layer : SelectedLayers )
    {
        Layer->GetOnTransformChangedDelegate().RemoveAll( this );
    }
}

UArianeLayer*
SArianeEditorLayerStackPanel::GetEditedLayer()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
        const TArray<UArianeLayer*>& SelectedLayers = LayerStack->GetSelectedLayers();

        if( SelectedLayers.Num() )
        {
            return SelectedLayers[0];
        }
    }

    return nullptr;
}

void
SArianeEditorLayerStackPanel::OnTransformChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
}

void
SArianeEditorLayerStackPanel::OnPostLayerSelectionChanged()
{
    UArianeLayer* EditedLayer =  GetEditedLayer();

    LayerDetailsView->SetObject( nullptr );

    if( EditedLayer )
    {
        EditedLayer->GetOnTransformChangedDelegate().AddSP( this, &SArianeEditorLayerStackPanel::OnTransformChanged );

        LayerDetailsView->SetObject( EditedLayer );
    }
}

void
SArianeEditorLayerStackPanel::Construct(const FArguments& InArgs, FArianeEditor* InEditor)
{
    FPropertyEditorModule& EditorModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    UArianePainting3DComponent* Painting3DComponent = InEditor->GetCurrentPainting3DComponent();
    FDetailsViewArgs DetailsViewArgs;

    Editor = InEditor;

    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    LayerDetailsView = EditorModule.CreateDetailView( DetailsViewArgs );
    LayerDetailsView->SetObject( nullptr );

    LayerDetailsView->RegisterInstancedCustomPropertyLayout(
        UArianeLayer::StaticClass(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FArianeLayerFilteredDetails::MakeInstance)
    );

    ChildSlot
    [
        SNew(SVerticalBox)
        .Visibility_Lambda( [InEditor]() -> EVisibility
            {
                return InEditor->GetCurrentPainting3DComponent() ? EVisibility::Visible : EVisibility::Hidden;
            } )
        + SVerticalBox::Slot()
        .AutoHeight()
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
        OnPostLayerSelectionChanged(); // First call at Widget creation (if we are in a floating tab for example)
    }
}

FReply
SArianeEditorLayerStackPanel::NewLayer()
{
    UArianePainting3DComponent* CurrentPainting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( CurrentPainting3DComponent )
    {
        GEditor->BeginTransaction(LOCTEXT("ariane-layer-stack-panel.new-layer","New Layer"));

        CurrentPainting3DComponent->GetLayerStack()->Modify();
        CurrentPainting3DComponent->GetLayerStack()->GetRootFolder()->Modify();

        UArianeLayerDrawing* DrawingLayer = CurrentPainting3DComponent->GetLayerStack()->CreateDrawingLayer( nullptr, true );

        CurrentPainting3DComponent->GetLayerStack()->ClearLayerSelection( false );
        CurrentPainting3DComponent->GetLayerStack()->SelectLayer( DrawingLayer, true );

        GEditor->EndTransaction();
    }

    return FReply::Handled();
}

void
SArianeEditorLayerStackPanel::AddReferencedObjects( FReferenceCollector& Collector )
{
    // Prevent UObjects from being garbage collected
}

FString
SArianeEditorLayerStackPanel::GetReferencerName() const
{
    return FString( "SArianeEditorLayerStackPanel" );
}

#undef LOCTEXT_NAMESPACE
