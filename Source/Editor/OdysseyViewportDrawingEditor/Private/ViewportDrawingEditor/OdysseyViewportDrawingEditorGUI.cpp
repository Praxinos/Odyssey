// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorGUI.h"

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

FOdysseyViewportDrawingEditorGUI::FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditorExtension* iExtension)
	: mExtension( iExtension )
    , mCommandList(MakeShareable(new FUICommandList()))
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    iToolkit->GetToolkitCommands()->Append(mCommandList.ToSharedRef());
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Layout

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorGUI::CreateWidget()
{
    return SNullWidget::NullWidget;
    /*
    FMargin StandardPadding(6.f, 3.f);

    return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
	        SNew(SVerticalBox)	
            + SVerticalBox::Slot()
            .Padding(StandardPadding)
	        .AutoHeight()
	        [
                mMasterTab->Widget()->AsShared()
            ]
        ]; */
}

#undef LOCTEXT_NAMESPACE
