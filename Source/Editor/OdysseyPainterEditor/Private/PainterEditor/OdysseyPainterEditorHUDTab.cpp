// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorHUDTab.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyHUDLine.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDHandle.h"

#include <ULIS>

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorHUDTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_HUD");
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorHUDTab::~FOdysseyPainterEditorHUDTab()
{
}

FOdysseyPainterEditorHUDTab::FOdysseyPainterEditorHUDTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(LOCTEXT( "hud-tab.name", "HUD" ), FSlateIcon( "OdysseyStyle", "PainterEditor.HUD16" ))
    , mEditor(iEditor)
{
}

void FOdysseyPainterEditorHUDTab::Init()
{
    if (mEditor)
        mEditor->OnSelectedToolChanged().AddSP(SharedThis<FOdysseyPainterEditorHUDTab>(this), &FOdysseyPainterEditorHUDTab::OnSelectedToolChanged);

    FOdysseyEditorTab::Init();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorHUDTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorHUDTab::CreateWidget()
{   
    return SNullWidget::NullWidget;
}

void
FOdysseyPainterEditorHUDTab::RefreshWidgetContent()
{
    UE_LOG(LogTemp, Display, TEXT("CHANGED WIDGET"));
}

void
FOdysseyPainterEditorHUDTab::OnSelectedToolChanged()
{
    RefreshWidgetContent();
}

void
FOdysseyPainterEditorHUDTab::BindShortcuts(FBaseToolkit* iToolkit)
{
}

#undef LOCTEXT_NAMESPACE
