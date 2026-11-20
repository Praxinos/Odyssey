// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuidesTab.h"
#include "Widgets/Guides/SOdysseyPainterEditorGuidesPanel.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

const FName& FOdysseyPainterEditorGuidesTab::StaticId()
{
    static const FName Id(TEXT("OdysseyPainterEditor_Guides"));
    return Id;
}

FOdysseyPainterEditorGuidesTab::FOdysseyPainterEditorGuidesTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT("guides-tab.name", "Guides"), FSlateIcon("OdysseyStyle", "PainterEditor.DetailsTab"))
    , mEditor(iEditor)
{
}

const FName& FOdysseyPainterEditorGuidesTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget> FOdysseyPainterEditorGuidesTab::CreateWidget()
{
    return SNew(SOdysseyPainterEditorGuidesPanel)
               .Editor(mEditor);
}

#undef LOCTEXT_NAMESPACE
