// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorHUDTab.h"

#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorHUDTab::~FOdysseyPainterEditorHUDTab()
{
}

FOdysseyPainterEditorHUDTab::FOdysseyPainterEditorHUDTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_HUD"),
                            LOCTEXT( "OdysseyPainterEditorHUDTab", "HUD" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.HUD16" ))
    , mEditor(iEditor)
    , mHUD(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorHUDTab::CreateWidget()
{
    return SNullWidget::NullWidget;
/*
	if (!mHUD)
    {
        UOdysseyHUDLine* line = NewObject<UOdysseyHUDLine>(GetTransientPackage(), NAME_None, RF_Transient);
        mHUD = MakeShareable(line);
    }

    return mHUD->CreateWidget();*/
}

void
FOdysseyPainterEditorHUDTab::BindShortcuts(FBaseToolkit* iToolkit)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public Getters

TSharedPtr<UOdysseyHUDLine>
FOdysseyPainterEditorHUDTab::GetHUD()
{
    return mHUD;
}


#undef LOCTEXT_NAMESPACE
