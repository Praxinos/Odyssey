// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorSelectedVectorObjectTab.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorSelectedVectorObjectTab"

const FName&
FOdysseyPainterEditorSelectedVectorObjectTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_SelectedVectorObject"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorSelectedVectorObjectTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorSelectedVectorObjectTab::~FOdysseyPainterEditorSelectedVectorObjectTab()
{

}

FOdysseyPainterEditorSelectedVectorObjectTab::FOdysseyPainterEditorSelectedVectorObjectTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "OdysseyPainterEditorSelectedVectorObjectTab", "Vector Selection" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{
    mObjectView = NewObject<UOdysseyPainterEditorVectorObjectView>();
    mPathView = NewObject<UOdysseyPainterEditorVectorPathView>();
    mGroupPaintView = NewObject<UOdysseyPainterEditorVectorGroupPaintView>();
/*
    mEditor->GetVectorObjectPickTool()->mSelectionChanged.AddRaw(this, &FOdysseyPainterEditorSelectedVectorObjectTab::OnSelectionChanged );
    mEditor->GetVectorPathDrawingTool()->mSelectionChanged.AddRaw(this, &FOdysseyPainterEditorSelectedVectorObjectTab::OnSelectionChanged );
    mEditor->GetVectorPrimitiveDrawingTool()->mSelectionChanged.AddRaw(this, &FOdysseyPainterEditorSelectedVectorObjectTab::OnSelectionChanged );
*/
}

void
FOdysseyPainterEditorSelectedVectorObjectTab::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Prevent these UObjects from being destroyed by garbage collection
	Collector.AddReferencedObject(mObjectView);
	Collector.AddReferencedObject(mPathView);
	Collector.AddReferencedObject(mGroupPaintView);
}

FString
FOdysseyPainterEditorSelectedVectorObjectTab::GetReferencerName() const
{
    return "FOdysseyPainterEditorSelectedVectorObjectTab";
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorSelectedVectorObjectTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorSelectedVectorObjectTab::CreateWidget()
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    
    mDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    mDetailsView->SetObject(nullptr);

    return mDetailsView;
}

void
FOdysseyPainterEditorSelectedVectorObjectTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorSelectedVectorObjectTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyPainterEditorVectorObjectView*
FOdysseyPainterEditorSelectedVectorObjectTab::GetObjectView()
{
    return mObjectView;
}

UOdysseyPainterEditorVectorPathView*
FOdysseyPainterEditorSelectedVectorObjectTab::GetPathView()
{
    return mPathView;
}

UOdysseyPainterEditorVectorGroupPaintView*
FOdysseyPainterEditorSelectedVectorObjectTab::GetGroupPaintView()
{
    return mGroupPaintView;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorSelectedVectorObjectTab::Update( FOdysseyVectorScene* iScene )
{
    // defaults to scene
    std::list<FOdysseyVectorObject*> sceneAsList { iScene };
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
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
         || ( objectClass == FOdysseyVectorScene::StaticClass() ) )
        {
            mGroupPaintView->Update( iScene, focusedObjectList );
            mDetailsView->SetObject( mGroupPaintView );
        }

        if( objectClass == FOdysseyVectorObject::StaticClass() )
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


#undef LOCTEXT_NAMESPACE
