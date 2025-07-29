// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tab/SOdysseyPainterEditorVectorMassModifierView.h"

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
#include "Undo/OdysseyVectorUndoMassModifier.h"

#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"

#include "HUD/OdysseyVectorHUD.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyPainterEditorVectorMassModifierView)
void
SOdysseyPainterEditorVectorMassModifierView::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
}

SOdysseyPainterEditorVectorMassModifierView::~SOdysseyPainterEditorVectorMassModifierView()
{
}

SOdysseyPainterEditorVectorMassModifierView::SOdysseyPainterEditorVectorMassModifierView()
{
}

void
SOdysseyPainterEditorVectorMassModifierView::Construct( const FArguments& InArgs )
{
    mVectorLayer = InArgs._VectorLayer;
    mSceneArray = InArgs._SceneArray;

    mObjectDetailsView = CreateViewPanel();
    mOptionsDetailsView = CreateViewPanel();

    ChildSlot
    [
        SNew(SVerticalBox)
/*
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .MinWidth( 100 )
            [
                SNew(STextBlock)
                .Text( LOCTEXT("vector-mass-modifier.apply-to-label.name", "Apply to") )
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SComboBox<TSharedPtr<FString>>)
                .OptionsSource(&_objectTypeOptions)
                .OnGenerateWidget(this, &SOdysseyPainterEditorVectorMassModifierView::MakeWidgetForOption)
                .OnSelectionChanged(this, &SOdysseyPainterEditorVectorMassModifierView::ObjectTypeSelectionChanged )
                .InitiallySelectedItem(_objectTypeOptions[0])
                [
                    SNew(STextBlock)
                    .Text( FText::FromString( *_objectTypeOptions[0].Get() ) )
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .MinWidth( 100 )
            [
                SNew(STextBlock)
                .Text( LOCTEXT("vector-mass-modifier.filter-label.name", "Filter") )
            ]
            + SHorizontalBox::Slot()
            .MinWidth( 100 )
            [
                SNew(SEditableText)
                .HintText( LOCTEXT("vector-mass-modifier.filter-hint-text.name", "e.g: path_*, group_*, etc...") )
                .OnTextChanged_Raw( this, &SOdysseyPainterEditorVectorMassModifierView::FilterTextChanged )
            ]
        ]
*/
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            mOptionsDetailsView.ToSharedRef()
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            mObjectDetailsView.ToSharedRef()
        ]
    ];

    mOptionsView = NewObject<UOdysseyPainterEditorVectorMassModifierOptionsView>();

    mOptionsView->SetMassModifierView( this );

    mOptionsDetailsView->SetObject( mOptionsView );

    mObjectView = NewObject<UOdysseyPainterEditorVectorObjectView>();
    mGroupView = NewObject<UOdysseyPainterEditorVectorGroupView>();
    mPathView = NewObject<UOdysseyPainterEditorVectorPathView>();
    mGroupPaintView = NewObject<UOdysseyPainterEditorVectorGroupPaintView>();

    mObjectView->SetVectorLayer( mVectorLayer );
    mGroupView->SetVectorLayer( mVectorLayer );
    mPathView->SetVectorLayer( mVectorLayer );
    mGroupPaintView->SetVectorLayer( mVectorLayer );

    mObjectDetailsView->SetExtensionHandler(SharedThis(this));
    mObjectDetailsView->OnFinishedChangingProperties().AddSP( this, &SOdysseyPainterEditorVectorMassModifierView::PropertyValueChanged );
    mObjectDetailsView->SetObject( mObjectView );

    mCurrentObjectView = mObjectView;
}

TSharedRef<SWidget>
SOdysseyPainterEditorVectorMassModifierView::MakeWidgetForOption( TSharedPtr<FString> InOption )
{
    return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void
SOdysseyPainterEditorVectorMassModifierView::ObjectTypeSelectionChanged( EMassModifierApplyTo NewValue )
{
    if( mOptionsView.Get()->ApplyTo == EMassModifierApplyTo::Paths )
    {
        //mPathView->Update( mEditor, scene, focusedObjectList );
        mObjectDetailsView->SetObject( mPathView );
        mCurrentObjectView = mPathView;
    }

    if( mOptionsView.Get()->ApplyTo == EMassModifierApplyTo::Groups )
    {
        //mGroupView->Update( mEditor, scene, focusedObjectList );
        mObjectDetailsView->SetObject( mGroupView );
        mCurrentObjectView = mGroupView;
    }

    if( mOptionsView.Get()->ApplyTo == EMassModifierApplyTo::PaintGroups )
    {
        //mGroupPaintView->Update( mEditor, scene, focusedObjectList );
        mObjectDetailsView->SetObject( mGroupPaintView );
        mCurrentObjectView = mGroupPaintView;
    }

    if( mOptionsView.Get()->ApplyTo == EMassModifierApplyTo::AllTypes )
    {
        // default
        //mObjectView->Update( mEditor, scene, focusedObjectList );
        mObjectDetailsView->SetObject( mObjectView );
        mCurrentObjectView = mObjectView;
    }
}

void
SOdysseyPainterEditorVectorMassModifierView::ValidateProperties()
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
    std::list<FOdysseyVectorObject*> objectList;
    uint64 objectClass;

    switch( mOptionsView.Get()->ApplyTo )
    {
        case EMassModifierApplyTo::Paths :
            objectClass = FOdysseyVectorPath::StaticClass();
        break;

        case EMassModifierApplyTo::Groups :
            objectClass = FOdysseyVectorGroup::StaticClass();
        break;

        case EMassModifierApplyTo::PaintGroups :
            objectClass = FOdysseyVectorGroupPaint::StaticClass();
        break;

        default :
            objectClass = FOdysseyVectorObject::StaticClass();
        break;
    }

    // Get all objects that will be modified according to the options selected by the user.
    for( FOdysseyVectorGroupPaint* scene : mSceneArray )
    {
        FOdysseyVectorObject::Traverse( scene
                                      , 0
                                      , [ this
                                        , scene
                                        , objectClass
                                        , &objectList ] ( FOdysseyVectorObject* object, uint64 traverseFlags ) -> uint64
                                        {
                                            if( object->HasBaseClass( objectClass ) )
                                            {
                                                if( mOptionsView.Get()->Filter.Len() )
                                                {
                                                    if( object->GetName().MatchesWildcard( mOptionsView.Get()->Filter
                                                                                         , mOptionsView.Get()->MatchCase ? ESearchCase::Type::IgnoreCase
                                                                                                                         : ESearchCase::Type::CaseSensitive ) )
                                                    {
                                                        objectList.push_back( object );
                                                    }
                                                }
                                                else
                                                {
                                                    objectList.push_back( object );
                                                }
                                            }

                                            return FOdysseyVectorObject::TRAVERSE_CONTINUE;
                                        } );

    }

    if( mCurrentObjectView )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            bool saveVertices = ( mCurrentObjectView->GetPropertyBit( GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathView, PathWidthInPercent ) )
                               || mCurrentObjectView->GetPropertyBit( GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorPathView, PathWidthInUnits   ) ) );

            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoMassModifier( mVectorLayer.Get()
                                                                         , objectList
                                                                         , ""
                                                                         , saveVertices
                                                                         , FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                                                                         | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                                                                         | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        mCurrentObjectView->ValidateProperties( objectList );

        // Update vector scene
        mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mVectorLayer->RequestRedraw( nullptr, 0 );
        // update widgets
        mVectorLayer->Notify( notificationFlags );
    }
}

TSharedPtr<IDetailsView>
SOdysseyPainterEditorVectorMassModifierView::CreateViewPanel()
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

// IDetailPropertyExtensionHandler::ExtendWidgetRow
void
SOdysseyPainterEditorVectorMassModifierView::ExtendWidgetRow ( FDetailWidgetRow& InWidgetRow
                                                             , const IDetailLayoutBuilder& InDetailBuilder
                                                             , const UClass* InObjectClass
                                                             , TSharedPtr< IPropertyHandle > iPropertyHandle )
{
    FProperty* property = iPropertyHandle.Get()->GetProperty();
    TSharedPtr<IPropertyHandle> parentPropertyHandle = iPropertyHandle.Get()->GetParentHandle();
    // We retrieve the top-most handle (useful for nested properties)
    TSharedPtr<IPropertyHandle> topPropertyHandle = parentPropertyHandle->GetProperty() ? parentPropertyHandle
                                                                                        : iPropertyHandle;

    if( mCurrentObjectView->HasProperty( property->GetFName() ) )
    {
        // Add a checkbox to the name content to be able to chose what should be modified en-masse
        InWidgetRow.NameContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SCheckBox)
                .OnCheckStateChanged( this
                                    , &SOdysseyPainterEditorVectorMassModifierView::PropertyCheckStateChanged
                                    , iPropertyHandle
                                    , InWidgetRow.NameContent().Widget
                                    , InWidgetRow.ValueContent().Widget )
                .IsChecked_Raw( this
                              , &SOdysseyPainterEditorVectorMassModifierView::GetPropertyCheckState
                              , iPropertyHandle )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SHorizontalBox)
                .IsEnabled_Lambda( [ this
                                   , topPropertyHandle ] () -> bool
                                   {
                                       return mCurrentObjectView->GetPropertyBit( topPropertyHandle->GetProperty()->GetFName() ) ;
                                   } )
                + SHorizontalBox::Slot()
                [
                    InWidgetRow.NameContent().Widget
                ]
            ]
        ];
    }
    else
    {
        InWidgetRow.NameContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SHorizontalBox)
                .IsEnabled_Lambda( [ this
                                   , topPropertyHandle ] () -> bool
                                   {
                                       return mCurrentObjectView->GetPropertyBit( topPropertyHandle->GetProperty()->GetFName() ) ;
                                   } )
                + SHorizontalBox::Slot()
                [
                    InWidgetRow.NameContent().Widget
                ]
            ]
        ];
    }

    InWidgetRow.ValueContent()
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SHorizontalBox)
            .IsEnabled_Lambda( [ this
                                , topPropertyHandle ] () -> bool
                                {
                                    return mCurrentObjectView->GetPropertyBit( topPropertyHandle->GetProperty()->GetFName() ) ;
                                } )
            + SHorizontalBox::Slot()
            [
                InWidgetRow.ValueContent().Widget
            ]
        ]
    ];
}

ECheckBoxState
SOdysseyPainterEditorVectorMassModifierView::GetPropertyCheckState( TSharedPtr<IPropertyHandle> iPropertyHandle ) const
{
    FProperty* property = iPropertyHandle.Get()->GetProperty();

    return mCurrentObjectView->GetPropertyBit( property->GetFName() ) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyPainterEditorVectorMassModifierView::PropertyCheckStateChanged( ECheckBoxState iState
                                                                      , TSharedPtr<IPropertyHandle> iPropertyHandle
                                                                      , TSharedRef<SWidget> NameWidget
                                                                      , TSharedRef<SWidget> ValueWidget )
{
    FProperty* property = iPropertyHandle.Get()->GetProperty();

    mCurrentObjectView->SetPropertyBit( property->GetFName()
                                        //, PropertyChangedEvent.MemberProperty->GetFName()
                                      , FName()
                                      , FName( property->GetMetaData( TEXT( "Category" ) ) )
                                      , ( iState == ECheckBoxState::Checked ) );
}

bool
SOdysseyPainterEditorVectorMassModifierView::IsPropertyExtendable( const UClass* InObjectClass
                                                                 , const IPropertyHandle& iPropertyHandle) const
{
    //return mCurrentObjectView->HasProperty( property->GetFName() );
    return true;
}

void
SOdysseyPainterEditorVectorMassModifierView::PropertyValueChanged( const FPropertyChangedEvent& iEvent )
{

}

FString
SOdysseyPainterEditorVectorMassModifierView::GetReferencerName() const
{
    return "SOdysseyPainterEditorVectorMassModifierView";
}

void
SOdysseyPainterEditorVectorMassModifierView::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Prevent these UObjects from being destroyed by garbage collection
    Collector.AddReferencedObject(mObjectView);
    Collector.AddReferencedObject(mPathView);
    Collector.AddReferencedObject(mGroupView);
    Collector.AddReferencedObject(mGroupPaintView);
    Collector.AddReferencedObject(mOptionsView);
}

void
UOdysseyPainterEditorVectorMassModifierOptionsView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if( PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorMassModifierOptionsView, ApplyTo ) )
    {
        mModifierView->ObjectTypeSelectionChanged( ApplyTo );
    }
}

#undef LOCTEXT_NAMESPACE
