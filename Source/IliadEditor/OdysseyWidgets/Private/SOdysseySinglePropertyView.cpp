// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseySinglePropertyView.h"

#include "ISinglePropertyView.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "Widgets/Layout/SBox.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

void
SOdysseySinglePropertyView::Construct(const  FArguments&  InArgs, UObject* iObject, const FName& iPropertyName, const FSinglePropertyParams& iParams)
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    mPropertyView = propertyEditorModule.CreateSingleProperty(iObject, iPropertyName, iParams);
    if (!mPropertyView)
        return;

    TSharedPtr<class IPropertyHandle> handle = mPropertyView->GetPropertyHandle();
    if (!handle)
        return;

    if (InArgs._OnOverridePropertyHandle.IsBound())
    {
        handle = InArgs._OnOverridePropertyHandle.Execute(handle);
        if (!handle)
            return;
    }

    TSharedRef<SWidget> nameWidget = handle->CreatePropertyNameWidget();
    TSharedRef<SWidget> valueWidget = handle->CreatePropertyValueWidget(false);

    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(0.f, 0.f, InArgs._InnerPadding, 0.f)
        [
            nameWidget
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SBox)
            .WidthOverride(InArgs._ValueWidthOverride)
            [
                valueWidget
            ]
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
