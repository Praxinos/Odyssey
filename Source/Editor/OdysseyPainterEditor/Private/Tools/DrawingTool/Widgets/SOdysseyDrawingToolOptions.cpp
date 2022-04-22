// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/DrawingTool/Widgets/SOdysseyDrawingToolOptions.h"

#include "ObjectEditorUtils.h"
#include "Widgets/SOdysseyShapeSelector.h"

#define LOCTEXT_NAMESPACE "SOdysseyDrawingToolOptions"

/////////////////////////////////////////////////////
// SOdysseyDrawingToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyDrawingToolOptions::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;

    this->ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyShapeSelector)
            .SelectedShape(this, &SOdysseyDrawingToolOptions::GetSelectedShape)
            .OnShapeSelected(this, &SOdysseyDrawingToolOptions::OnShapeSelected)
        ]
        + SVerticalBox::Slot()
        [
            SAssignNew(mShapeSlot, SBorder)
            [
                SNullWidget::NullWidget
            ]
        ]
    ];

    UpdateShapeSlot();
}

void
SOdysseyDrawingToolOptions::OnShapeSelected(EOdysseyShape iSelectedShape)
{
    FObjectEditorUtils::SetPropertyValue(mTool, "SelectedShape", iSelectedShape);
    UpdateShapeSlot();
}

EOdysseyShape
SOdysseyDrawingToolOptions::GetSelectedShape() const
{
    return mTool->GetSelectedShape();
}

void
SOdysseyDrawingToolOptions::UpdateShapeSlot()
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true, nullptr);
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
    TSharedRef<IDetailsView> details_view = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    details_view->SetIsPropertyVisibleDelegate(
        FIsPropertyVisible::CreateLambda(
            [](const FPropertyAndParent& iPropertyAndParent) -> bool
            {
                return !iPropertyAndParent.Property.HasAnyPropertyFlags(CPF_DisableEditOnInstance); //Brush Overrides
            }
        )
    );

    details_view->SetObject(mTool->GetSelectedShapeInstance());
    mShapeSlot->SetContent(details_view);
}

#undef LOCTEXT_NAMESPACE

