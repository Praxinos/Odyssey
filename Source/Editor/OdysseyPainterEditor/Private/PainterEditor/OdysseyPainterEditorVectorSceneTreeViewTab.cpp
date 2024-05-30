// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVector.h"
#include "HUD/OdysseyVectorHUD.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "OdysseyPainterEditorVectorObjectView.h"
#include "OdysseyPainterEditorVectorPathView.h"
#include "OdysseyPainterEditorVectorGroupPaintView.h"
#include "OdysseyPainterEditorVectorTagInbetweenerView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
    : FOdysseyEditorTab( LOCTEXT( "vector-scene-tree-view-tab.name", "Vector Scene Tree View" ),
                         FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{
    mObjectView = NewObject<UOdysseyPainterEditorVectorObjectView>();
    mPathView = NewObject<UOdysseyPainterEditorVectorPathView>();
    mGroupPaintView = NewObject<UOdysseyPainterEditorVectorGroupPaintView>();
    mTagInbetweenerView = NewObject<UOdysseyPainterEditorVectorTagInbetweenerView>();
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
    mDetailsView->SetObject( nullptr );

    if( iScene )
    {
        mScene = iScene;

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            FOdysseyVectorObject* selectedObject = iScene->GetEngine()->GetLastSelectedObject();

            if( selectedObject )
            {
                FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                    mTagInbetweenerView->Update( mEditor, inbetweenerTag );
                    mDetailsView->SetObject( mTagInbetweenerView );
                }
            }
        }
        else
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
                    mPathView->Update( mEditor, iScene, focusedObjectList );
                    mDetailsView->SetObject( mPathView );
                }

                if( ( objectClass == FOdysseyVectorGroupPaint::StaticClass() )
                 || ( objectClass == FOdysseyVectorGroupPaint::StaticClass() ) )
                {
                    mGroupPaintView->Update( mEditor, iScene, focusedObjectList );
                    mDetailsView->SetObject( mGroupPaintView );
                }

                if( ( objectClass == FOdysseyVectorObject::StaticClass() )
                 || ( objectClass == FOdysseyVectorGroup::StaticClass() ) )
                {
                    // default
                    mObjectView->Update( mEditor, iScene, focusedObjectList );
                    mDetailsView->SetObject( mObjectView );
                }
            }
        }
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
    mVectorSceneTreeView = SNew( SOdysseyPainterEditorVectorSceneTreeView, mEditor );
    mDetailsView = CreateObjectPropertiesPanel();

    return SNew(SWidgetSwitcher)
        .WidgetIndex(this, &FOdysseyPainterEditorVectorSceneTreeViewTab::WidgetIndex)
        +SWidgetSwitcher::Slot()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("vector-scene-tree-view-tab.no-scene-text", "This tab is only available when editing a vector scene."))
            .AutoWrapText(true)
        ]
        +SWidgetSwitcher::Slot()
        [
            SNew(SSplitter)
            .Orientation( EOrientation::Orient_Vertical )
            +SSplitter::Slot()
            [
                mVectorSceneTreeView.ToSharedRef()
            ]
            +SSplitter::Slot()
            [
                mDetailsView.ToSharedRef()
            ]
        ];
        
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::OnRefresh( FOdysseyVectorGroupPaint* iScene )
{
    mScene = iScene;
    Update( iScene );
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::UpdateSceneTreeView( FOdysseyVectorGroupPaint* iScene )
{
    mScene = iScene;
    mVectorSceneTreeView.Get()->Update( iScene );
}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::Update( FOdysseyVectorGroupPaint* iScene )
{
    mScene = iScene;
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
	Collector.AddReferencedObject(mTagInbetweenerView);
	//Collector.AddReferencedObject(mDetailsView);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

int
FOdysseyPainterEditorVectorSceneTreeViewTab::WidgetIndex() const
{
    if (mScene)
        return 1;
    return 0;
}

#undef LOCTEXT_NAMESPACE
