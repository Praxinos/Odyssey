// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyBrushCustomization.h"

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
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
    void CustomizeBrushOptions(IDetailLayoutBuilder& iBuilder);
};

void
FOdysseyBrushDetails::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
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
FOdysseyBrushCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyBrushAssetBase::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyBrushDetails::MakeInstance));
}

#undef LOCTEXT_NAMESPACE
