// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tab/SOdysseyPainterEditorVectorMassModifierView.h"

#include "Editor.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"

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
    : mPreviewUndo ( nullptr )
{
/*
    FSlateApplication::Get().GetOnModalLoopTickEvent().AddLambda([this](float DeltaTime)
    {
    } );
*/
}

void
SOdysseyPainterEditorVectorMassModifierView::Construct( const FArguments& InArgs )
{
    FString detailCategoriesSectionName = FString ( "DetailCategories" );

    mVectorLayer = InArgs._VectorLayer;
    mSceneArray = InArgs._SceneArray;
    mPreviewScene = InArgs._PreviewScene;
    mEditor = InArgs._Editor;

    mObjectDetailsView = CreateViewPanel();
    mOptionsDetailsView = CreateViewPanel();

    ChildSlot
    [
        SNew(SVerticalBox)
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

    mObjectView->OverrideConfigSection( detailCategoriesSectionName );
    mGroupView->OverrideConfigSection( detailCategoriesSectionName );
    mPathView->OverrideConfigSection( detailCategoriesSectionName );
    mGroupPaintView->OverrideConfigSection( detailCategoriesSectionName );

    mPathView->SetDisplayWideningOptions( true );

    mObjectDetailsView->SetExtensionHandler(SharedThis(this));
    mObjectDetailsView->SetGenericLayoutDetailsDelegate( FOnGetDetailCustomizationInstance::CreateSP( this, &SOdysseyPainterEditorVectorMassModifierView::GetCustomizationInstance ) );
    mObjectDetailsView->OnFinishedChangingProperties().AddSP( this, &SOdysseyPainterEditorVectorMassModifierView::PropertyValueChanged );
    mObjectDetailsView->SetObject( mObjectView );

    mCurrentObjectView = mObjectView;

}

// I could not find a way to collapse all categories without the config being saved to the disk so I ended
// up with this solution which is a bit complicated but I'm open to other options
void
SOdysseyPainterEditorVectorMassModifierView::DetailCustomizationHandler::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
/*
    // store the collapse status. We don't want to collapse all the time,
    // just once, the first time a widget of type SOdysseyPainterEditorVectorMassModifierView is launched,
    // that's why we declare it as static
    //static TMap<FName,bool> expanded;
    TArray<FName> categoryNames;

    DetailBuilder.GetCategoryNames( categoryNames );

    for( FName& categoryName : categoryNames )
    {
        //bool& value = expanded.FindOrAdd( categoryName );

        DetailBuilder.EditCategory( categoryName ).InitiallyCollapsed( true );
        DetailBuilder.EditCategory( categoryName ).RestoreExpansionState( false );

        //value = true;
    }
*/
}

bool
SOdysseyPainterEditorVectorMassModifierView::HasAnyPropertyBit()
{
    return mCurrentObjectView->HasAnyPropertyBit();
}

TSharedRef<IDetailCustomization>
SOdysseyPainterEditorVectorMassModifierView::GetCustomizationInstance()
{
    // Note: DetailCustomizationHandler is a nested class of SOdysseyPainterEditorVectorMassModifierView
    // Note 2 : the instance will be deleted by unreal. Implement PendingDelete() if needed.
    return MakeShared<DetailCustomizationHandler>( mOptionsView );
}

TSharedRef<SWidget>
SOdysseyPainterEditorVectorMassModifierView::MakeWidgetForOption( TSharedPtr<FString> InOption )
{
    return SNew(STextBlock).Text(FText::FromString(*InOption));
}

TObjectPtr<UOdysseyPainterEditorVectorObjectView>
SOdysseyPainterEditorVectorMassModifierView::GetCurrentObjectView()
{
    return mCurrentObjectView;
}

void
SOdysseyPainterEditorVectorMassModifierView::ObjectTypeSelectionChanged( EMassModifierApplyTo NewValue )
{
    TObjectPtr<UOdysseyPainterEditorVectorObjectView> formerObjectView = mCurrentObjectView;

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

    mCurrentObjectView->ImportParamFromOtherView( formerObjectView );
}

void
SOdysseyPainterEditorVectorMassModifierView::UndoPreview()
{
    if( mPreviewUndo )
    {
        mPreviewUndo->Revert( nullptr );

        delete mPreviewUndo;

        mPreviewUndo = nullptr;
    }
}

SOdysseyPainterEditorVectorMassModifierView::FOnPreviewPropertiesDelegate&
SOdysseyPainterEditorVectorMassModifierView::GetOnPreviewPropertiesDelegate()
{
    return OnPreviewPropertiesDelegate;
}

void
SOdysseyPainterEditorVectorMassModifierView::PreviewProperties()
{
    std::list<FOdysseyVectorObject*> objectList;
    uint64 objectClass;

    UndoPreview();

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
    FOdysseyVectorObject::Traverse( mPreviewScene
                                  , 0
                                  , [ this
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

    mPreviewUndo = new FOdysseyVectorUndoMassModifier( mVectorLayer.Get()
                                                     , objectList
                                                     , ""
                                                     , ""
                                                     , ""
                                                     , true );
    // because this is a standalone undo, we don't call update functions via delegates but directly
    mPreviewUndo->SetUpdateViaDelegation( false );

    if( mCurrentObjectView )
    {
        mCurrentObjectView->ValidateProperties( objectList, false );

        // Update vector scene
        mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mVectorLayer->RequestRedraw( nullptr, 0 );
    }

    OnPreviewPropertiesDelegate.Broadcast();

    //GEditor->RedrawAllViewports();
    //FSlateApplication::Get().ForceRedrawWindow( FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef() );
}

void
SOdysseyPainterEditorVectorMassModifierView::ValidateProperties()
{
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
                                                                         , ""
                                                                         , ""
                                                                         , saveVertices );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        mCurrentObjectView->ValidateProperties( objectList, true );

        // Update vector scene
        mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mVectorLayer->RequestRedraw( nullptr, 0 );
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

    // This if statement allows us to discriminate between properties that belong to the object and children
    // properties in ustruct. We don't want a checkbox for child properties.
    if( ( mCurrentObjectView->HasProperty( property->GetFName() ) )
    // We also don't want a checkbox for the Path Widening Mode property
     && ( property->GetFName() != GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorPathView, WideningMode) ) )
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
    if( mPreviewScene )
    {
        // commented-out: no preview for now because the viewport does not refresh behind a modal window
        //PreviewProperties();
    }
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
