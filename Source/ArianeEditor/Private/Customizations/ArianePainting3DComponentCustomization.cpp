// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePainting3DComponentCustomization.h"
#include "ArianePainting3DComponent.h"
// Ariane Editor headers
#include "ArianeEditorModule.h"
#include "ArianeEditorViewportEdMode.h"
// Unreal Editor headers
#include "DetailWidgetRow.h"
#include "EditorModeManager.h"

TSharedRef<IDetailCustomization>
FArianePainting3DComponentCustomization::MakeInstance()
{
    return MakeShared<FArianePainting3DComponentCustomization>();
}

void
FArianePainting3DComponentCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Painting3D", FText::GetEmpty(), ECategoryPriority::Important);

    Category.AddCustomRow(FText::FromString("Action Row"))
    .ValueContent()
    [
        SNew(SButton)
        .Text(FText::FromString("Edit Drawing"))
        .OnClicked_Lambda([&DetailBuilder]() -> FReply {
            FEditorModeTools& ModeTools = GLevelEditorModeTools();

            ModeTools.ActivateMode( FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId );

            return FReply::Handled();
        })
    ];
}
