// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVector.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorSceneTreeViewTab"

const FName&
FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_VectorSceneTreeView"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorSceneTreeViewTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorSceneTreeViewTab::~FOdysseyPainterEditorVectorSceneTreeViewTab()
{

}

FOdysseyPainterEditorVectorSceneTreeViewTab::FOdysseyPainterEditorVectorSceneTreeViewTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab( LOCTEXT( "OdysseyPainterEditorVectorSceneTreeViewTab", "Vector Scene Tree View" ),
                         FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{
    mObjectView = NewObject<UOdysseyPainterEditorVectorObjectView>();
    mPathView = NewObject<UOdysseyPainterEditorVectorPathView>();
    mGroupPaintView = NewObject<UOdysseyPainterEditorVectorGroupPaintView>();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorVectorSceneTreeViewTab::GetId() const
{
    return StaticId();
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::UpdateObjectPropertiesPanel( FOdysseyVectorGroupPaint* iScene )
{
    // defaults to scene
    std::list<FOdysseyVectorObject*>& sceneAsList = iScene->GetEngine()->GetChildrenList();
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetEngine()->GetSelectedObjectList();
    std::list<FOdysseyVectorObject*>& focusedObjectList = selectedObjectList.size() ? selectedObjectList
                                                                                    : sceneAsList;
    uint32 objectClass = FOdysseyVectorObject::GetCommonClass( focusedObjectList );

    if( objectClass )
    {
        if( objectClass == FOdysseyVectorPath::StaticClass() )
        {
            mPathView->Update( iScene, focusedObjectList );
            mDetailsView->SetObject( mPathView );
        }

        if( ( objectClass == FOdysseyVectorGroupPaint::StaticClass() )
         || ( objectClass == FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            mGroupPaintView->Update( iScene, focusedObjectList );
            mDetailsView->SetObject( mGroupPaintView );
        }

        if( ( objectClass == FOdysseyVectorObject::StaticClass() )
         || ( objectClass == FOdysseyVectorGroup::StaticClass() ) )
        {
            // default
            mObjectView->Update( iScene, focusedObjectList );
            mDetailsView->SetObject( mObjectView );
        }
    }
    else
    {
        mDetailsView->SetObject( nullptr );
    }
}

TSharedPtr<IDetailsView>
FOdysseyPainterEditorVectorSceneTreeViewTab::CreateObjectPropertiesPanel()
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

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorSceneTreeViewTab::CreateWidget()
{
    mVectorSceneTreeView = SNew( SOdysseyPainterEditorVectorSceneTreeView );
    mDetailsView = CreateObjectPropertiesPanel();

    return SNew(SSplitter)
          .Orientation( EOrientation::Orient_Vertical )
          +SSplitter::Slot()
          [
              mVectorSceneTreeView.ToSharedRef()
          ]
          +SSplitter::Slot()
          [
              mDetailsView.ToSharedRef()
          ];
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::OnRefresh( FOdysseyVectorGroupPaint* iScene )
{
    Update( iScene );
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::UpdateSceneTreeView( FOdysseyVectorGroupPaint* iScene )
{
    mVectorSceneTreeView.Get()->Update( iScene );
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::Update( FOdysseyVectorGroupPaint* iScene )
{
    UpdateSceneTreeView( iScene );
    UpdateObjectPropertiesPanel( iScene );
}

FString
FOdysseyPainterEditorVectorSceneTreeViewTab::GetReferencerName() const
{
    return "FOdysseyPainterEditorVectorSceneTreeViewTab";
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Prevent these UObjects from being destroyed by garbage collection
	Collector.AddReferencedObject(mObjectView);
	Collector.AddReferencedObject(mPathView);
	Collector.AddReferencedObject(mGroupPaintView);
	//Collector.AddReferencedObject(mDetailsView);
}

#undef LOCTEXT_NAMESPACE
