// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrushCustomization.h"

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "OdysseyBrushAssetBase.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushCustomization"

class FOdysseyBrushDetails : public IDetailCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShared<FOdysseyBrushDetails>();
    }

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface

private:
    void CustomizeOverrides(IDetailLayoutBuilder& iBuilder);
    void CustomizeBrushOptions(IDetailLayoutBuilder& iBuilder);
};

void
FOdysseyBrushDetails::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    CustomizeOverrides(iBuilder);
    CustomizeBrushOptions(iBuilder);
}

void
FOdysseyBrushDetails::CustomizeBrushOptions(IDetailLayoutBuilder& iBuilder)
{
    TSharedRef<IPropertyHandle> brushOptionsHandle = iBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyBrushAssetBase, BrushOptions));
    if (!iBuilder.IsPropertyVisible(brushOptionsHandle))
        return;

    IDetailCategoryBuilder& globalsCategory = iBuilder.EditCategory("Globals", LOCTEXT("GlobalsCategory", "Globals"), ECategoryPriority::TypeSpecific);
    iBuilder.HideProperty("BrushOptions");   

    TArray< TWeakObjectPtr<UObject> > objects;
    iBuilder.GetObjectsBeingCustomized( objects );

    UOdysseyBrushAssetBase* brushInstance = Cast<UOdysseyBrushAssetBase>(objects[0]);

    TArray<UObject*> brushOptionsObjects; //contains only one object
    brushOptionsObjects.Add(brushInstance->GetBrushOptions());

    TSharedPtr<IPropertyHandle> sizeHandle = iBuilder.AddObjectPropertyData(brushOptionsObjects, "Size");
    if (sizeHandle.IsValid())
        globalsCategory.AddProperty(sizeHandle.ToSharedRef());

    TSharedPtr<IPropertyHandle> flowHandle = iBuilder.AddObjectPropertyData(brushOptionsObjects, "Flow");
    if (flowHandle.IsValid())
        globalsCategory.AddProperty(flowHandle.ToSharedRef());
}


void
FOdysseyBrushDetails::CustomizeOverrides(IDetailLayoutBuilder& iBuilder)
{
    TSharedRef<IPropertyHandle> overridesHandle = iBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyBrushAssetBase, Overrides));
    if (!iBuilder.IsPropertyVisible(overridesHandle))
        return;

    iBuilder.HideProperty("Overrides");

    IDetailCategoryBuilder& overridesCategory = iBuilder.EditCategory("Overrides", LOCTEXT("OverridesCategory", "Overrides"));

    TArray< TWeakObjectPtr<UObject> > objects;
    iBuilder.GetObjectsBeingCustomized( objects );

    UOdysseyBrushAssetBase* brushInstance = Cast<UOdysseyBrushAssetBase>(objects[0]);

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
FOdysseyBrushCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyBrushAssetBase::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyBrushDetails::MakeInstance));
}

#undef LOCTEXT_NAMESPACE
