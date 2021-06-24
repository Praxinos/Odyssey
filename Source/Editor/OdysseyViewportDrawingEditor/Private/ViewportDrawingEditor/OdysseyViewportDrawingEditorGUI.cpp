// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorGUI.h"

#include "OdysseyViewportDrawingEditorMasterTab.h"
#include "Widgets/Layout/SExpandableArea.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorGUI::~FOdysseyViewportDrawingEditorGUI()
{
}

FOdysseyViewportDrawingEditorGUI::FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditor* iEditor) :
	FOdysseyTexture2DEditorGUI(iEditor),
	mEditor( iEditor ),
    mCommandList(MakeShareable(new FUICommandList()))
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditorGUI::CreateTabs()
{
	FOdysseyTexture2DEditorGUI::CreateTabs();

    mMasterTab = MakeShareable(new FOdysseyViewportDrawingEditorMasterTab(mEditor));
    mMasterTab->Init();
}

TSharedRef<FTabManager::FSplitter>
FOdysseyViewportDrawingEditorGUI::CreateMainSection()
{
	return FOdysseyTexture2DEditorGUI::CreateMainSection();
}

void
FOdysseyViewportDrawingEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyTexture2DEditorGUI::BindShortcuts(iToolkit);
    iToolkit->GetToolkitCommands()->Append(mCommandList.ToSharedRef());
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyViewportDrawingEditorGUI::GetLayoutName()
{
	return "OdysseyViewportDrawingEditor_Layout";
}


TSharedPtr<FOdysseyViewportDrawingEditorMasterTab>&
FOdysseyViewportDrawingEditorGUI::GetMasterTab()
{
    return mMasterTab;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Layout

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorGUI::CreateWidget()
{
    FMargin StandardPadding(6.f, 3.f);

    return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
	        SNew(SVerticalBox)	
            + SVerticalBox::Slot()
            .Padding(StandardPadding)
	        .AutoHeight()
	        [
                GetMasterTab()->Widget()->AsShared()
            ]
        ];
}

#undef LOCTEXT_NAMESPACE
