// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorBrushCustomization.h"

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "OdysseyBrushAssetBase.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorBrushCustomization"

class FOdysseyPainterEditorBrushDetails : public IDetailCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShared<FOdysseyPainterEditorBrushDetails>();
    }

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface
};

void
FOdysseyPainterEditorBrushDetails::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    IDetailCategoryBuilder& overridesCategory = iBuilder.EditCategory("Overrides", FText::FromString("Overrides"));

    TArray< TWeakObjectPtr<UObject> > objects;
    iBuilder.GetObjectsBeingCustomized( objects );

    UOdysseyBrushAssetBase* brushInstance = Cast<UOdysseyBrushAssetBase>(objects[0]);

    TSharedRef<IPropertyHandle> overridesHandle = iBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyBrushAssetBase, Overrides));
    if (!iBuilder.IsPropertyVisible(overridesHandle))
        return;

    for (auto overrideElement : brushInstance->Overrides)
    {
        UObject* overrideObject = overrideElement.Value;
        if (!overrideObject)
            continue;

        FName categoryName = overrideObject->GetClass()->GetFName();
        FText categoryText = overrideObject->GetClass()->GetDisplayNameText();
        IDetailGroup& group = overridesCategory.AddGroup(categoryName, categoryText, false, true);


        TArray<UObject*> overrideObjects; //contains only one object
        overrideObjects.Add(overrideObject);

        for (const FProperty* property : TFieldRange<FProperty>(overrideObject->GetClass()))
        {
            TSharedPtr<IPropertyHandle> propertyHandle = iBuilder.AddObjectPropertyData(overrideObjects, property->GetFName());
            if (!propertyHandle.IsValid())
                continue;
            group.AddPropertyRow(propertyHandle.ToSharedRef());
        }
    }
}

void
FOdysseyPainterEditorBrushCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyBrushAssetBase::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyPainterEditorBrushDetails::MakeInstance));
}

#undef LOCTEXT_NAMESPACE
