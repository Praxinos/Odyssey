// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorGUI.h"

#include "OdysseyViewportDrawingEditorMasterTab.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorGUI::~FOdysseyViewportDrawingEditorGUI()
{
}

FOdysseyViewportDrawingEditorGUI::FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditor* iEditor) :
	FOdysseyTextureEditorGUI(iEditor),
	mEditor( iEditor ),
    mCommandList(MakeShareable(new FUICommandList()))
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditorGUI::CreateTabs()
{
	FOdysseyTextureEditorGUI::CreateTabs();

    mMasterTab = MakeShareable(new FOdysseyViewportDrawingEditorMasterTab(mEditor));
    mMasterTab->Init();
}

TSharedRef<FTabManager::FSplitter>
FOdysseyViewportDrawingEditorGUI::CreateMainSection()
{
	return FOdysseyTextureEditorGUI::CreateMainSection();
}

void
FOdysseyViewportDrawingEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyTextureEditorGUI::BindShortcuts(iToolkit);
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
