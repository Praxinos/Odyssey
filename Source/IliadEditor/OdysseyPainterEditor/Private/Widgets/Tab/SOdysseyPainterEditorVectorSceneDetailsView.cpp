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
    UnbindLayerDelegates();

    // Keep commented-out until we convert mEditor to a sharedptr
    //mEditor->OnSourceChanged().RemoveAll( this );

    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll( this );
}

SOdysseyPainterEditorVectorSceneDetailsView::SOdysseyPainterEditorVectorSceneDetailsView()
    : mEditor(nullptr)
    , mScene(*this, nullptr)
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
                mPathView->Update( mVectorLayer, focusedObjectList );
                mDetailsView->SetObject( mPathView );
                mCurrentObjectView = mPathView;
            }

            if( objectClass == FOdysseyVectorGroup::StaticClass() )
            {
                mGroupView->Update( mVectorLayer, focusedObjectList );
                mDetailsView->SetObject( mGroupView );
                mCurrentObjectView = mGroupView;
            }

            if( objectClass == FOdysseyVectorGroupPaint::StaticClass() )
            {
                mGroupPaintView->Update( mVectorLayer, focusedObjectList );
                mDetailsView->SetObject( mGroupPaintView );
                mCurrentObjectView = mGroupPaintView;
            }

            if( objectClass == FOdysseyVectorObject::StaticClass() )
            {
                // default
                mObjectView->Update( mVectorLayer, focusedObjectList );
                mDetailsView->SetObject( mObjectView );
                mCurrentObjectView = mObjectView;
            }
        }
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::OnVectorLayerNotify( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                                , uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
    {
        FOdysseyVectorGroupPaint* currentScene = mScene.Get();

        // update the cached Value by calling the getter. Hence it will call OnSceneChanged()
        mScene.UpdateNow( *this );

        // if the attributes value does not changes, we force the update of the tree
        if( currentScene == mScene.Get() )
        {
            ParseVectorNotifications( iInvalidationFlags );
        }
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::ParseVectorNotifications( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags )
{
    if( ( iInvalidationFlags.bits.any() )
        // DO NOT react to color change events. First it is useless, second it creates issues when picking a color in a
        // color picker modal window
     && ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::COLOR] == false )
     && ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_COLOR] == false ) )
    {
        Update();
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::UnbindLayerDelegates()
{
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnUpdateDelegate().RemoveAll( this );
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::UpdateCurrentLayer( UOdysseyLayerStack* iLayerStack )
{
    //warning. The CurrentLayerStack delegate is STATIC. The iLayerStack received might not belong to this editor
    if( mEditor->LayerStack() == iLayerStack )
    {
        UOdysseyAnimationLayerImageVector* animationVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayerStack->GetCurrentLayer());
        UOdysseyTextureLayerImageVector* textureVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->GetCurrentLayer());

        if( animationVectorLayer )
        {
            mVectorLayer = animationVectorLayer->GetVectorLayer();
        }

        if( textureVectorLayer )
        {
            mVectorLayer = textureVectorLayer->GetVectorLayer();
        }
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::BindLayerDelegates()
{
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnUpdateDelegate().AddSP( this, &SOdysseyPainterEditorVectorSceneDetailsView::OnVectorLayerNotify );
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
            }

            UnbindLayerDelegates();
            UpdateCurrentLayer( layerStack );
            BindLayerDelegates();
        }
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::PropertyValueChanged( const FPropertyChangedEvent& iEvent )
{
    // mCurrentObjectView can be NULL in INBETWEENING mode
    if( mCurrentObjectView )
    {
        FOdysseyVectorGroupPaint* scene = mScene.Get();

        if (!scene)
            return;

        // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
        // rebuilt while it's processing stuff
        UnbindLayerDelegates();

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
                                                                        , iEvent.MemberProperty->GetFName()
                                                                        , iEvent.GetPropertyName() );
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

        // Update vector scene
        scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        scene->GetLayer()->RequestRedraw( nullptr, 0 );

        // Re-register this widget after we are done
        BindLayerDelegates();
    }
}

void
SOdysseyPainterEditorVectorSceneDetailsView::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UnbindLayerDelegates();
    UpdateCurrentLayer( iLayerStack );
    BindLayerDelegates();
}

void
SOdysseyPainterEditorVectorSceneDetailsView::OnSceneChanged()
{
    // Parse by simulating a scene invalidation
    ParseVectorNotifications( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::DEFAULT) );
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
