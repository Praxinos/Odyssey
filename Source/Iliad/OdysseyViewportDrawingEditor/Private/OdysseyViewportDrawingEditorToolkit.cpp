// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyViewportDrawingEditorToolkit.h"
#include "OdysseyStyleSet.h"
#include "OdysseyEditorTab.h"
#include "Widgets/SOdysseyViewportDrawingEditorMasterTab.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "TextureEditor/OdysseyTextureEditorExtension.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

FOdysseyViewportDrawingEditorToolkit::FOdysseyViewportDrawingEditorToolkit(TSharedRef<FOdysseyPainterEditor> iEditor, FEdMode* iEdMode)
    : FOdysseyModeToolkit(iEditor)
    , mEdMode(iEdMode)
{
    TSharedRef<FOdysseyTextureEditorExtension> textureExtension = MakeShared<FOdysseyTextureEditorExtension>(&iEditor.Get());
    TSharedRef<FOdysseyAnimationEditorExtension> animationExtension = MakeShared<FOdysseyAnimationEditorExtension>(&iEditor.Get());
    mViewportDrawingExtension = MakeShared<FOdysseyViewportDrawingEditorExtension>(&iEditor.Get());

    iEditor->AddExtension(textureExtension);
    iEditor->AddExtension(animationExtension);
    iEditor->AddExtension(mViewportDrawingExtension.ToSharedRef());
}

FName
FOdysseyViewportDrawingEditorToolkit::GetToolkitFName() const
{
    return FName("OdysseyMeshPaintMode");
}

FText
FOdysseyViewportDrawingEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT("editor.name", "Odyssey Mesh Paint");
}

void
FOdysseyViewportDrawingEditorToolkit::GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const
{
    ioPaletteNames.Add( FName( "Odyssey Panels Manager" ));
}

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorToolkit::GetInlineContent() const
{
    return SNew(SOdysseyViewportDrawingEditorMasterTab, mViewportDrawingExtension.Get());
    //TODO: Create the widget in ViewportDrawingEditorToolkit
    //return mEditor->GetGUI()->GetWidget();
    //return SNullWidget::NullWidget;
}

FEdMode*
FOdysseyViewportDrawingEditorToolkit::GetEditorMode() const
{
    return mEdMode;
}

TSharedRef<FOdysseyViewportDrawingEditorExtension>
FOdysseyViewportDrawingEditorToolkit::GetViewportDrawingExtension() const
{
    return mViewportDrawingExtension.ToSharedRef();
}

void
FOdysseyViewportDrawingEditorToolkit::BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder )
{
    const TArray<TSharedPtr<FOdysseyEditorTab>>& tabs = mEditor->GetTabs();
    for (TSharedPtr<FOdysseyEditorTab> tab : tabs)
    {
        FFormatNamedArguments Args;
        Args.Add("TabName", tab->GetName());

        FText description = FText::Format(
            LOCTEXT("master-tab.display-tab.tooltip", "Display {TabName}"),
            Args
        );

        ioToolbarBuilder.AddToolBarButton(
            FUIAction(FExecuteAction::CreateSP( tab.ToSharedRef(), &FOdysseyEditorTab::Open) ),
            NAME_None,
            tab->GetName(),
            description,
            tab->GetIcon()
        );
    }
}

#undef LOCTEXT_NAMESPACE
