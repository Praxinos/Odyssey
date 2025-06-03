// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneDetailsView.h"

#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorVectorObjectView.h"
#include "OdysseyPainterEditorVectorPathView.h"
#include "OdysseyPainterEditorVectorGroupPaintView.h"
#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "HUD/OdysseyVectorHUD.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyPainterEditorVectorSceneDetailsView)
void
SOdysseyPainterEditorVectorSceneDetailsView::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mScene, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPainterEditorVectorSceneDetailsView&>(Widget).OnSceneChanged();
        }
    ));
}

SOdysseyPainterEditorVectorSceneDetailsView::~SOdysseyPainterEditorVectorSceneDetailsView()
{
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnNotifyDelegate().RemoveAll( this );
    }
}

SOdysseyPainterEditorVectorSceneDetailsView::SOdysseyPainterEditorVectorSceneDetailsView()
    : mEditor(nullptr)
    , mScene(*this, nullptr)
{
    mObjectView = NewObject<UOdysseyPainterEditorVectorObjectView>();
    mGroupView = NewObject<UOdysseyPainterEditorVectorGroupView>();
    mPathView = NewObject<UOdysseyPainterEditorVectorPathView>();
    mGroupPaintView = NewObject<UOdysseyPainterEditorVectorGroupPaintView>();
    mTagInbetweenerView = NewObject<UOdysseyPainterEditorVectorTagInbetweenerView>();

    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &SOdysseyPainterEditorVectorSceneDetailsView::OnCurrentLayerChanged);
}

void
SOdysseyPainterEditorVectorSceneDetailsView::Construct( const FArguments& InArgs, FOdysseyPainterEditor* iEditor)
{
    mEditor = iEditor;
    mScene.Assign(*this, InArgs._Scene);
    mDetailsView = CreateObjectPropertiesPanel();
    ChildSlot
    [
        mDetailsView.ToSharedRef()
    ];

    mEditor->OnSourceChanged().AddSP( this, &SOdysseyPainterEditorVectorSceneDetailsView::OnSourceChanged );
}

TSharedPtr<IDetailsView>
SOdysseyPainterEditorVectorSceneDetailsView::CreateObjectPropertiesPanel()
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

void
SOdysseyPainterEditorVectorSceneDetailsView::Update()
{
    mDetailsView->SetObject( nullptr );

    FOdysseyVectorGroupPaint* scene = mScene.Get();
    if (!scene)
        return;

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        if( mTagInbetweenerView->Update( mEditor, scene ) )
        {
            mDetailsView->SetObject( mTagInbetweenerView );
        }
    }
    else
    {
        // defaults to scene
        std::list<FOdysseyVectorObject*>& sceneAsList = scene->GetCell()->GetChildrenList();
        std::list<FOdysseyVectorObject*>& selectedObjectList = scene->GetCell()->GetSelectedObjectList();
        std::list<FOdysseyVectorObject*>& focusedObjectList = selectedObjectList.size() ? selectedObjectList
                                                                                        : sceneAsList;
        uint32 objectClass = FOdysseyVectorObject::GetCommonClass( focusedObjectList );

        if( objectClass )
        {
            if( objectClass == FOdysseyVectorPath::StaticClass() )
            {
                mPathView->Update( mEditor, scene, focusedObjectList );
                mDetailsView->SetObject( mPathView );
            }

            if( objectClass == FOdysseyVectorGroup::StaticClass() )
            {
                mGroupView->Update( mEditor, scene, focusedObjectList );
                mDetailsView->SetObject( mGroupView );
            }

            if( ( objectClass == FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                mGroupPaintView->Update( mEditor, scene, focusedObjectList );
                mDetailsView->SetObject( mGroupPaintView );
            }

            if( ( objectClass == FOdysseyVectorObject::StaticClass() ) )
            {
                // default
                mObjectView->Update( mEditor, scene, focusedObjectList );
                mDetailsView->SetObject( mObjectView );
            }
        }
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::OnVectorLayerNotify( FOdysseyVectorLayer* iLayer, uint64 iSignalFlags )
{
    FOdysseyVectorGroupPaint* currentScene = mScene.Get();

    // update the cached Value by calling the getter. Hence it will call OnSceneChanged()
    mScene.UpdateNow( *this );

    // if the attributes value does not changes, we force the update of the tree
    if( currentScene == mScene.Get() )
    {
        ParseVectorNotifications( iSignalFlags );
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::ParseVectorNotifications( uint64 iSignalFlags )
{
    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS )
    {
        Update();
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::BindLayerDelegates( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyAnimationLayerImageVector* imageVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayerStack->GetCurrentLayer());

    if( imageVectorLayer )
    {
        if( mOldVectorLayer.IsValid() )
        {
            mOldVectorLayer->OnNotifyDelegate().RemoveAll( this );
        }

        mOldVectorLayer = mVectorLayer;
        mVectorLayer = imageVectorLayer->GetVectorLayer();

        mVectorLayer->OnNotifyDelegate().AddSP( this, &SOdysseyPainterEditorVectorSceneDetailsView::OnVectorLayerNotify );
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::OnSourceChanged()
{
    if( mEditor->GetSource() )
    {
        UOdysseyLayerStack* layerStack = mEditor->GetSource()->GetLayerStack();

        if( layerStack )
        {
            BindLayerDelegates( layerStack );
        }
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    BindLayerDelegates( iLayerStack );
}

void
SOdysseyPainterEditorVectorSceneDetailsView::OnSceneChanged()
{
    ParseVectorNotifications( FOdysseyVectorEngine::NOTIFY_ALL );
}

FString
SOdysseyPainterEditorVectorSceneDetailsView::GetReferencerName() const
{
    return "SOdysseyPainterEditorVectorSceneDetailsView";
}

void
SOdysseyPainterEditorVectorSceneDetailsView::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Prevent these UObjects from being destroyed by garbage collection
    Collector.AddReferencedObject(mObjectView);
    Collector.AddReferencedObject(mPathView);
    Collector.AddReferencedObject(mGroupView);
    Collector.AddReferencedObject(mGroupPaintView);
    Collector.AddReferencedObject(mTagInbetweenerView);
}

#undef LOCTEXT_NAMESPACE
