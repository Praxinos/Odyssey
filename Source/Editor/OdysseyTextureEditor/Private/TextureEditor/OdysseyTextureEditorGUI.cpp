// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor) :
	FOdysseyPainterEditorGUI(iEditor),
	mEditor( iEditor )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditorGUI::CreateTabs()
{
	FOdysseyPainterEditorGUI::CreateTabs();

	//ADD NEW TABS
	ODYSSEY_ADD_TAB(mLayerStackTab, FOdysseyTextureEditorLayerStackTab, mEditor);
	ODYSSEY_ADD_TAB(mTextureDetailsTab, FOdysseyTextureEditorTextureDetailsTab, mEditor);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateRightSection()
{
	return FOdysseyPainterEditorGUI::CreateRightSection()
		// LayerStack + Notes
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mLayerStackTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.35f)
		)
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mTextureDetailsTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.15f)
		);
}

void
FOdysseyTextureEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyPainterEditorGUI::BindShortcuts(iToolkit);

    //---

    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyTextureEditorCommands& textureEditorCommands = FOdysseyTextureEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyTextureEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(textureEditorCommands.ResetView, ResetView )
    MAP_ACTION(textureEditorCommands.GroupPaint, GroupPaint )
    MAP_ACTION(textureEditorCommands.Group, Group )
    MAP_ACTION(textureEditorCommands.Ungroup, Ungroup )
    MAP_ACTION(textureEditorCommands.BringForward, BringForward )
    MAP_ACTION(textureEditorCommands.SendBackward, SendBackward )
    MAP_ACTION(textureEditorCommands.ConvertToPath, ConvertToPath )
    MAP_ACTION(textureEditorCommands.RemoveSelectedObjects, RemoveSelectedObjects )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Menu and Toolbar

void
FOdysseyTextureEditorGUI::ExtendMenuAbout( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyPainterEditorGUI::ExtendMenuAbout( iOwner, iMenuName );

    //---

    UToolMenu* menu = UToolMenus::Get()->FindMenu(*(iMenuName.ToString() + FString(".Iliad")));

    //Adding entries in our menu
    FToolMenuSection& aboutSection = menu->AddSection("About ILIAD", LOCTEXT("OdysseyPainter", "ILIAD"));
    {
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().ResetView
            , LOCTEXT("ResetView", "ResetView")
            , LOCTEXT("ResetView", "ResetView")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().GroupPaint
            , LOCTEXT("GroupPaint", "GroupPaint")
            , LOCTEXT("GroupPaint", "GroupPaint")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().Group
            , LOCTEXT("Group", "Group")
            , LOCTEXT("Group", "Group")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().Ungroup
            , LOCTEXT("Ungroup", "Ungroup")
            , LOCTEXT("Ungroup", "Ungroup")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().BringForward
            , LOCTEXT("BringForward", "Bring forward")
            , LOCTEXT("BringForward", "Bring forward")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().SendBackward
            , LOCTEXT("SendBackward", "Send backward")
            , LOCTEXT("SendBackward", "Send backward")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().ConvertToPath
            , LOCTEXT("ConvertToPath", "Convert to path")
            , LOCTEXT("ConvertToPath", "Convert to path")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().RemoveSelectedObjects
            ,LOCTEXT("RemoveSelectedObjects","RemoveSelectedObjects")
            ,LOCTEXT("RemoveSelectedObjects","RemoveSelectedObjects")
            ,FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
            ,NAME_None);
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyTextureEditorGUI::GetLayoutName()
{
	return "OdysseyTextureEditor_Layout";
}

TSharedPtr<FOdysseyTextureEditorLayerStackTab>&
FOdysseyTextureEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}

TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>&
FOdysseyTextureEditorGUI::GetTextureDetailsTab()
{
	return mTextureDetailsTab;
}

void
FOdysseyTextureEditorGUI::RemoveSelectedObjects()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer )
    {
        if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
        {
            UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

            currentVectorLayer->GetScene()->RemoveSelectedObjects();
            currentVectorLayer->GetScene()->Update( 0 );

            currentVectorLayer->RenderImageChanged( false );
        }
    }
}

void
FOdysseyTextureEditorGUI::ConvertToPath()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer )
    {
        if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
        {
            UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
            // work on a copy to be able to remove the object from selection while iterating
            std::list<FOdysseyVectorObject*> selectedObjectList = vectorScene->GetSelectedObjectList();

            for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
            {
                FOdysseyVectorObject* object = (*it);
                FOdysseyVectorEllipse* circle = static_cast<FOdysseyVectorEllipse*>(object);

                if( circle )
                {
                    FOdysseyVectorPathCubic* cubicPath = circle->Convert();

                    vectorScene->Unselect( circle );
                    vectorScene->RemoveChild( circle );
                    vectorScene->AppendChild( cubicPath );
                    vectorScene->Select( cubicPath );

                    cubicPath->UpdateMatrix();
                    cubicPath->Invalidate();
                }
            }

            vectorScene->Update( 0 );

            vectorObjectTab.Get()->Update( vectorScene );
        }
    }
}

void
FOdysseyTextureEditorGUI::BringForward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer )
    {
        if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
        {
            UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            std::list<FOdysseyVectorObject*>& selectedObjectList = currentVectorLayer->GetScene()->GetSelectedObjectList();

            for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
            {
                FOdysseyVectorObject* object = (*it);

                object->MoveFront();
            }

            currentVectorLayer->RenderImageChanged( false );
        }
    }
}

void
FOdysseyTextureEditorGUI::SendBackward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer )
    {
        if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
        {
            UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            std::list<FOdysseyVectorObject*>& selectedObjectList = currentVectorLayer->GetScene()->GetSelectedObjectList();

            for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
            {
                FOdysseyVectorObject* object = (*it);

                object->MoveBack();
            }

            currentVectorLayer->RenderImageChanged( false );
        }
    }
}

void
FOdysseyTextureEditorGUI::Ungroup()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer )
    {
        if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
        {
            UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
            FOdysseyVectorObject* selectedObject = vectorScene->GetLastSelected();
            FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>(selectedObject);

            if( group )
            {
                // we work on a copy of the list to be able to delete children while iterating
                std::list<FOdysseyVectorObject*> childrenList = group->GetChildrenList();

                for( std::list<FOdysseyVectorObject*>::iterator it = childrenList.begin(); it != childrenList.end(); ++it )
                {
                    FOdysseyVectorObject* child = (*it);

                    group->RemoveChild( child );
                    child->SwitchSpace( *group->GetParent() );
                    group->GetParent()->AppendChild( child );
                }

                group->GetParent()->RemoveChild( group );

                vectorScene->ClearSelection();
                vectorScene->UpdateMatrix();
                vectorScene->Update( 0 );

                currentVectorLayer->RenderImageChanged( false );
            }

            vectorObjectTab.Get()->Update( vectorScene );
        }
    }
}

void
FOdysseyTextureEditorGUI::ResetView()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        currentVectorLayer->GetScene()->ResetTransform();
        currentVectorLayer->GetScene()->UpdateMatrix();

        currentVectorLayer->RenderImageChanged( false );
    }
}

void
FOdysseyTextureEditorGUI::Group()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorGroup* group = vectorScene->GroupSelectedObjects();

        vectorScene->ClearSelection();
        vectorScene->Select( group );

        currentVectorLayer->RenderImageChanged( false );

        vectorObjectTab.Get()->Update( vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::GroupPaint()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        std::list<FOdysseyVectorObject*>& selectObjectList = vectorScene->GetSelectedObjectList();

        if( selectObjectList.size() )
        {
            FOdysseyVectorGroupPaint* paintGroup = new FOdysseyVectorGroupPaint( "Paint Group" );
            std::vector<FOdysseyVectorPath*> pathArray;

            vectorScene->AppendChild( paintGroup );
            paintGroup->UpdateMatrix();

            for( std::list<FOdysseyVectorObject*>::iterator it = selectObjectList.begin(); it != selectObjectList.end(); ++it )
            {
                FOdysseyVectorObject* selectedObject = (*it);

                if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
                {
                    FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);
                        
                    for( std::list<FOdysseyVectorObject*>::iterator cit = selectedPaintGroup->GetChildrenList().begin(); cit != selectedPaintGroup->GetChildrenList().end(); ++cit )
                    {
                        FOdysseyVectorObject* childObject = (*cit);

                        if( childObject->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
                        {
                            FOdysseyVectorPathCubic* childCubicPath = static_cast<FOdysseyVectorPathCubic*>( childObject );

                            pathArray.push_back( childCubicPath );
                        }
                    }

                    selectedPaintGroup->GetParent()->RemoveChild( selectedPaintGroup );

                    selectedPaintGroup->CopyBuckets( paintGroup );
                }

                if( selectedObject->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
                {
                    FOdysseyVectorPathCubic* selectedCubicPath = static_cast<FOdysseyVectorPathCubic*>( selectedObject );

                    pathArray.push_back( selectedCubicPath );
                }
            }

            for( int i = 0; i < pathArray.size(); i++ )
            {
                pathArray[i]->GetParent()->RemoveChild( pathArray[i] );
/*                    pathArray[i]->SwitchSpace( *paintGroup );*/

                paintGroup->AppendChild( pathArray[i] );
/*
                pathArray[i]->InvalidateAllSegments();

                pathArray[i]->ResetTransform();
                pathArray[i]->UpdateMatrix();
*/
            }

            // first update to update paths' segments.
            vectorScene->Update( 0 );

            //paintGroup->Invalidate();

            vectorScene->ClearSelection();
            vectorScene->Select( paintGroup );
            //currentVectorLayer->GetScene()->Update( 0 );

            // second update to update paintgroup
            currentVectorLayer->RenderImageChanged( false );
        }

        vectorObjectTab.Get()->Update( vectorScene );
    }
}

#undef LOCTEXT_NAMESPACE
