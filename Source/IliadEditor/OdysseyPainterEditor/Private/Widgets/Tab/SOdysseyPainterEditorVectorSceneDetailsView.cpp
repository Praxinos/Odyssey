// IDDN.FR.001.060015.014.S.X.2019.000.00000
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
#include "OdysseyTextureLayerImageVector.h"
#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyTextureLayerImageVector.h"

#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"

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

    // Keep commented-out until we convert mEditor to a sharedptr
    //mEditor->OnSourceChanged().RemoveAll( this );

    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll( this );
}

SOdysseyPainterEditorVectorSceneDetailsView::SOdysseyPainterEditorVectorSceneDetailsView()
    : mEditor(nullptr)
    , mScene(*this, nullptr)
    , mOldVectorLayer ( nullptr )
    , mVectorLayer ( nullptr )
{
}

void
SOdysseyPainterEditorVectorSceneDetailsView::Construct( const FArguments& InArgs )
{
    mEditor = InArgs._Editor;
    mScene.Assign(*this, InArgs._Scene);

    mDetailsView = CreateObjectPropertiesPanel();
    ChildSlot
    [
        mDetailsView.ToSharedRef()
    ];

    mObjectView = NewObject<UOdysseyPainterEditorVectorObjectView>();
    mGroupView = NewObject<UOdysseyPainterEditorVectorGroupView>();
    mPathView = NewObject<UOdysseyPainterEditorVectorPathView>();
    mGroupPaintView = NewObject<UOdysseyPainterEditorVectorGroupPaintView>();
    mTagInbetweenerView = NewObject<UOdysseyPainterEditorVectorTagInbetweenerView>();

    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &SOdysseyPainterEditorVectorSceneDetailsView::OnCurrentLayerChanged);

    mEditor->OnSourceChanged().AddSP( this, &SOdysseyPainterEditorVectorSceneDetailsView::OnSourceChanged );

    mDetailsView->OnFinishedChangingProperties().AddSP( this, &SOdysseyPainterEditorVectorSceneDetailsView::PropertyValueChanged );
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
    mCurrentObjectView = nullptr;

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
                mPathView->Update( focusedObjectList );
                mDetailsView->SetObject( mPathView );
                mCurrentObjectView = mPathView;
            }

            if( objectClass == FOdysseyVectorGroup::StaticClass() )
            {
                mGroupView->Update( focusedObjectList );
                mDetailsView->SetObject( mGroupView );
                mCurrentObjectView = mGroupView;
            }

            if( objectClass == FOdysseyVectorGroupPaint::StaticClass() )
            {
                mGroupPaintView->Update( focusedObjectList );
                mDetailsView->SetObject( mGroupPaintView );
                mCurrentObjectView = mGroupPaintView;
            }

            if( objectClass == FOdysseyVectorObject::StaticClass() )
            {
                // default
                mObjectView->Update( focusedObjectList );
                mDetailsView->SetObject( mObjectView );
                mCurrentObjectView = mObjectView;
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
SOdysseyPainterEditorVectorSceneDetailsView::UnbindLayerDelegates( UOdysseyLayerStack* iLayerStack )
{
    mVectorLayer->OnNotifyDelegate().RemoveAll( this );
}

void
SOdysseyPainterEditorVectorSceneDetailsView::BindLayerDelegates( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyAnimationLayerImageVector* animationVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayerStack->GetCurrentLayer());
    UOdysseyTextureLayerImageVector* textureVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->GetCurrentLayer());

    if( mOldVectorLayer.IsValid() )
    {
        mOldVectorLayer->OnNotifyDelegate().RemoveAll( this );
    }

    if( animationVectorLayer )
    {
        mOldVectorLayer = mVectorLayer;
        mVectorLayer = animationVectorLayer->GetVectorLayer();

        mVectorLayer->OnNotifyDelegate().AddSP( this, &SOdysseyPainterEditorVectorSceneDetailsView::OnVectorLayerNotify );
    }

    if( textureVectorLayer )
    {
        mOldVectorLayer = mVectorLayer;
        mVectorLayer = textureVectorLayer->GetVectorLayer();

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
            UOdysseyLayer* layer = layerStack->GetCurrentLayer();
            UOdysseyAnimationLayerImageVector* animationLayer = Cast<UOdysseyAnimationLayerImageVector>(layer);
            UOdysseyTextureLayerImageVector* textureLayer = Cast<UOdysseyTextureLayerImageVector>(layer);

            if( animationLayer || textureLayer )
            {
                TSharedPtr<FOdysseyVectorLayer> vectorLayer = animationLayer ? animationLayer->GetVectorLayer()
                                                                             : textureLayer->GetVectorLayer();

                mObjectView->SetVectorLayer( vectorLayer );
                mGroupView->SetVectorLayer( vectorLayer );
                mPathView->SetVectorLayer( vectorLayer );
                mGroupPaintView->SetVectorLayer( vectorLayer );
            }

            BindLayerDelegates( layerStack );
        }
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::PropertyValueChanged( const FPropertyChangedEvent& iEvent )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
    FOdysseyVectorGroupPaint* scene = mScene.Get();

    if (!scene)
        return;

    // defaults to scene
    std::list<FOdysseyVectorObject*>& sceneAsList = scene->GetCell()->GetChildrenList();
    std::list<FOdysseyVectorObject*>& selectedObjectList = scene->GetCell()->GetSelectedObjectList();
    std::list<FOdysseyVectorObject*>& focusedObjectList = selectedObjectList.size() ? selectedObjectList
                                                                                    : sceneAsList;
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));
    if( GUndo )
    {
        FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectParam( scene->GetLayer()
                                                                    , focusedObjectList
                                                                    , FName(iEvent.Property->GetMetaData(TEXT("Category")))
                                                                    , FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                                                                    | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                                                                    | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD );
        // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
        // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
        // of a call to GEditor::PostEditChangeProperty()
        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    mCurrentObjectView->ValidateProperties( focusedObjectList, true );

    // prevent THIS widget update by unregistering its delegates
    UnbindLayerDelegates( mEditor->GetSource()->GetLayerStack() );
    // Update vector scene
    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    scene->GetLayer()->RequestRedraw( nullptr, 0 );
    // update widgets
    scene->GetLayer()->Notify( notificationFlags );
    // Re-register THIS widget delegates
    BindLayerDelegates( mEditor->GetSource()->GetLayerStack() );
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
