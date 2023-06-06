// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorSelectedVectorObjectTab.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorSelectedVectorObjectTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorSelectedVectorObjectTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorSelectedVectorObjectTab::~FOdysseyPainterEditorSelectedVectorObjectTab()
{

}

FOdysseyPainterEditorSelectedVectorObjectTab::FOdysseyPainterEditorSelectedVectorObjectTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(TEXT("OdysseyPainterEditor_SelectedVectorObject"),
                            LOCTEXT( "OdysseyPainterEditorSelectedVectorObjectTab", "Vector Selection" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{
    mObjectView = NewObject<UOdysseyPainterEditorVectorObjectView>();
    mPathView = NewObject<UOdysseyPainterEditorVectorPathView>();
    mEllipseView = NewObject<UOdysseyPainterEditorVectorEllipseView>();
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
	Collector.AddReferencedObject(mEllipseView);
	Collector.AddReferencedObject(mGroupPaintView);
}

FString
FOdysseyPainterEditorSelectedVectorObjectTab::GetReferencerName() const
{
    return "FOdysseyPainterEditorSelectedVectorObjectTab";
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

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

UOdysseyPainterEditorVectorEllipseView*
FOdysseyPainterEditorSelectedVectorObjectTab::GetEllipseView()
{
    return mEllipseView;
}

UOdysseyPainterEditorVectorGroupPaintView*
FOdysseyPainterEditorSelectedVectorObjectTab::GetGroupPaintView()
{
    return mGroupPaintView;
}

void
FOdysseyPainterEditorSelectedVectorObjectTab::Update( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    if( selectedObject && ( iScene->GetSelectedObjectList().size() == 1 ) )
    {
        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            mPathView->Update( selectedObject );
            mDetailsView->SetObject( mPathView );
        }
        else
        if( selectedObject->GetClass() == FOdysseyVectorEllipse::StaticClass() )
        {
            mEllipseView->Update( selectedObject );
            mDetailsView->SetObject( mEllipseView );
        }
        else
        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            mGroupPaintView->Update( selectedObject );
            mDetailsView->SetObject( mGroupPaintView );
        }
        else
        {
            // default
            mObjectView->Update( selectedObject );
            mDetailsView->SetObject( mObjectView );
        }
    }
    else
    {
        mDetailsView->SetObject(nullptr);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh( FOdysseyVectorScene* iScene )
{
    Update( iScene );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

#undef LOCTEXT_NAMESPACE
