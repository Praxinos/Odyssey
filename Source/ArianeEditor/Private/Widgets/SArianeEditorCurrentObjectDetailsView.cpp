// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "SArianeEditorCurrentObjectDetailsView.h"
#include "ArianeEditor.h"
#include "ArianeEditorObjectProxy.h"
#include "ArianeEditorPathProxy.h"
#include "ArianeEditorGroupProxy.h"
#include "ArianeEditorEllipseProxy.h"
#include "ArianeEditorRectangleProxy.h"
#include "ArianeEditorLineProxy.h"
#include "ArianeEditorPolygonProxy.h"
#include "ArianeEditorCycleProxy.h"
// Ariane Headers
#include "ArianePainting3DComponent.h"
#include "ArianeLayer.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
#include "ArianeLayerStack.h"
#include "ArianeObject.h"
#include "ArianeGroup.h"
#include "ArianeEllipse.h"
#include "ArianeRectangle.h"
#include "ArianeLine.h"
#include "ArianePolygon.h"
#include "ArianeCycle.h"
// Unreal Headers
#include "Editor.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"


#include "HUD/OdysseyVectorHUD.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

SArianeEditorCurrentObjectDetailsView::~SArianeEditorCurrentObjectDetailsView()
{
}

SArianeEditorCurrentObjectDetailsView::SArianeEditorCurrentObjectDetailsView()
    : Editor(nullptr)
    , bDoUpdate(false)
{
}

void
SArianeEditorCurrentObjectDetailsView::Construct( const FArguments& InArgs, FArianeEditor* InEditor )
{
    Editor = InEditor;

    DetailsView = CreateObjectPropertiesPanel();

    ChildSlot
    [
        DetailsView.ToSharedRef()
    ];

    ObjectProxy = NewObject<UArianeEditorObjectProxy>();
    GroupProxy = NewObject<UArianeEditorGroupProxy>();
    PathProxy = NewObject<UArianeEditorPathProxy>();
    EllipseProxy = NewObject<UArianeEditorEllipseProxy>();
    RectangleProxy = NewObject<UArianeEditorRectangleProxy>();
    LineProxy = NewObject<UArianeEditorLineProxy>();
    PolygonProxy = NewObject<UArianeEditorPolygonProxy>();
    CycleProxy = NewObject<UArianeEditorCycleProxy>();

    DetailsView->OnFinishedChangingProperties().AddSP( this, &SArianeEditorCurrentObjectDetailsView::PropertyValueChanged );

    Editor->OnPre3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPre3DPaintingComponentSelectionChanged );
    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPost3DPaintingComponentSelectionChanged );

    // First update. Following updates will be triggered by delegates
    Update();
}

void
SArianeEditorCurrentObjectDetailsView::OnPrePainting3DComponentUpdate( bool bInteractive )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        // the bInteractive is voluntarily ignored. During a MouseDown, the flag is set but we still need to mark the widget
        // as needing an update
        if( VectorLayer )
        {
            FArianeGroup* RootGroup = VectorLayer->GetDrawing()->GetRootGroup();

            FArianeObject::Traverse ( RootGroup
                                    , [this] ( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
                {
                    if( Object->GetInvalidationFlags().HasAny() )
                    {
                        bDoUpdate = true;
                    }

                    return bDoUpdate ? FArianeObject::ETraversalReturnValue::Stop
                                     : FArianeObject::ETraversalReturnValue::Continue;
                } );
        }
    }
}

void
SArianeEditorCurrentObjectDetailsView::OnPostPainting3DComponentUpdate( bool bInteractive )
{
    if( bInteractive == false )
    {
        if( bDoUpdate )
        {
            Update();

            bDoUpdate = false;
        }
    }
}

void
SArianeEditorCurrentObjectDetailsView::OnPre3DPaintingComponentSelectionChanged()
{
    UnbindComponentDelegates();
}

void
SArianeEditorCurrentObjectDetailsView::OnPost3DPaintingComponentSelectionChanged()
{
    BindComponentDelegates();

    Update();
}

void
SArianeEditorCurrentObjectDetailsView::OnPostDrawingChanged()
{
    Update();
}

void
SArianeEditorCurrentObjectDetailsView::OnPreLayerStackSelectionChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( VectorLayer )
        {
            // commented-out: nothing to do on Pre. Left there for consistency
            //VectorLayer->OnPreImageChangedDelegate().RemoveAll( this );
            VectorLayer->OnPostDrawingChangedDelegate().RemoveAll( this );
        }
    }
}

void
SArianeEditorCurrentObjectDetailsView::OnPostLayerStackSelectionChanged()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( VectorLayer )
        {
            // commented-out: nothing to do on Pre. Left there for consistency
            //VectorLayer->OnPreImageChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPreImageChanged );
            VectorLayer->OnPostDrawingChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPostDrawingChanged );
        }
    }

    Update();
}

void
SArianeEditorCurrentObjectDetailsView::OnPreLayerStackHierarchyChanged()
{
}

void
SArianeEditorCurrentObjectDetailsView::OnPostLayerStackHierarchyChanged()
{
    Update();
}

void
SArianeEditorCurrentObjectDetailsView::UnbindComponentDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->GetLayerStack()->OnPreHierarchyChangedDelegate().RemoveAll( this );
        Painting3DComponent->GetLayerStack()->OnPostHierarchyChangedDelegate().RemoveAll( this );

        Painting3DComponent->GetLayerStack()->OnPreSelectionChangedDelegate().RemoveAll( this );
        Painting3DComponent->GetLayerStack()->OnPostSelectionChangedDelegate().RemoveAll( this );

        Painting3DComponent->OnPreUpdateDelegate().RemoveAll( this );
        Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );
    }
}

void
SArianeEditorCurrentObjectDetailsView::BindComponentDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->GetLayerStack()->OnPreHierarchyChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPreLayerStackHierarchyChanged );
        Painting3DComponent->GetLayerStack()->OnPostHierarchyChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPostLayerStackHierarchyChanged );

        Painting3DComponent->GetLayerStack()->OnPreSelectionChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPreLayerStackSelectionChanged );
        Painting3DComponent->GetLayerStack()->OnPostSelectionChangedDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPostLayerStackSelectionChanged );

        Painting3DComponent->OnPreUpdateDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPrePainting3DComponentUpdate );
        Painting3DComponent->OnPostUpdateDelegate().AddSP( this, &SArianeEditorCurrentObjectDetailsView::OnPostPainting3DComponentUpdate );
    }
}

TSharedPtr<IDetailsView>
SArianeEditorCurrentObjectDetailsView::CreateObjectPropertiesPanel()
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs;
    TSharedPtr<IDetailsView> detailsView;

    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    detailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    detailsView->SetObject(nullptr);

    return detailsView;
}

FArianeGroup*
SArianeEditorCurrentObjectDetailsView::GetRootGroup()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( VectorLayer )
        {
            return VectorLayer->GetDrawing()->GetRootGroup();
        }
    }

    return nullptr;
}

void
SArianeEditorCurrentObjectDetailsView::Update()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    DetailsView->SetObject( nullptr );
    CurrentObjectProxy = nullptr;

    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( VectorLayer )
        {
            // work on a copy because we may add the root group in the array
            TArray<FArianeObject*> SelectedObjects = VectorLayer->GetDrawing()->GetSelectedObjects();

            if( SelectedObjects.IsEmpty() )
            {
                // Use the root group as the default object
                SelectedObjects.Add( VectorLayer->GetDrawing()->GetRootGroup() );
            }

            uint32 ObjectClass = FArianeObject::GetCommonClass( SelectedObjects );

            if( ObjectClass )
            {
                if( ObjectClass == FArianePath::StaticClass() )
                {
                    CurrentObjectProxy = PathProxy;
                }

                if( ObjectClass == FArianeGroup::StaticClass() )
                {
                    CurrentObjectProxy = GroupProxy;
                }

                if( ObjectClass == FArianeEllipse::StaticClass() )
                {
                    CurrentObjectProxy = EllipseProxy;
                }

                if( ObjectClass == FArianeRectangle::StaticClass() )
                {
                    CurrentObjectProxy = RectangleProxy;
                }

                if( ObjectClass == FArianeLine::StaticClass() )
                {
                    CurrentObjectProxy = LineProxy;
                }

                if( ObjectClass == FArianePolygon::StaticClass() )
                {
                    CurrentObjectProxy = PolygonProxy;
                }

                if( ObjectClass == FArianeCycle::StaticClass() )
                {
                    // default
                    CurrentObjectProxy = CycleProxy;
                }

                if( ObjectClass == FArianeObject::StaticClass() )
                {
                    // default
                    CurrentObjectProxy = ObjectProxy;
                }

                if( CurrentObjectProxy )
                {
                    CurrentObjectProxy->Update( SelectedObjects );
                    DetailsView->SetObject( CurrentObjectProxy );
                }
            }
        }
    }
}

void
SArianeEditorCurrentObjectDetailsView::PropertyValueChanged( const FPropertyChangedEvent& iEvent )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        // for undos
        GEditor->BeginTransaction(LOCTEXT("ariane-object.transaction.property-changed","Property Changed"));

        if( VectorLayer && CurrentObjectProxy )
        {
            TArray<FArianeObject*>& SelectedObjects = VectorLayer->GetDrawing()->GetSelectedObjects();
            FArianeGroup* RootGroup = GetRootGroup();

            // for undos
            VectorLayer->GetDrawing()->Modify();

            // Unregister this widget's updates when the scene is updated. We don't want this widget to be
            // rebuilt while it's processing stuff
            UnbindComponentDelegates();

            if( SelectedObjects.IsEmpty() )
            {
                // Use the root group as the default object
                SelectedObjects.Add( VectorLayer->GetDrawing()->GetRootGroup() );
            }

            CurrentObjectProxy->ValidateProperties( SelectedObjects, true );

            Painting3DComponent->Update( false );

            // Re-register this widget after we are done
            BindComponentDelegates();
        }

        GEditor->EndTransaction();
    }
}

FString
SArianeEditorCurrentObjectDetailsView::GetReferencerName() const
{
    return "SArianeEditorCurrentObjectDetailsView";
}

void
SArianeEditorCurrentObjectDetailsView::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Prevent these UObjects from being destroyed by garbage collection
    Collector.AddReferencedObject(ObjectProxy);
    Collector.AddReferencedObject(PathProxy);
    Collector.AddReferencedObject(GroupProxy);
    Collector.AddReferencedObject(EllipseProxy);
    Collector.AddReferencedObject(RectangleProxy);
    Collector.AddReferencedObject(LineProxy);
    Collector.AddReferencedObject(PolygonProxy);
    Collector.AddReferencedObject(CycleProxy);
}

#undef LOCTEXT_NAMESPACE
