// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyBrushCustomization.h"

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "Modules/ModuleManager.h"
#include "OdysseyBrushAssetBase.h"

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
};

void
SortCategories(const TMap<FName, IDetailCategoryBuilder*>& iAllCategoryMap)
{
    for (const TPair<FName, IDetailCategoryBuilder*>& Pair : iAllCategoryMap )
    {
        int32 SortOrder = Pair.Value->GetSortOrder();
        const FName& CategoryName = Pair.Key;

        if (CategoryName == TEXT("Common") )
        {
            SortOrder = 1;
        }
        else
        {
            SortOrder += 2; //make sure sort order is at least 2
        }

        Pair.Value->SetSortOrder(SortOrder);
    }
}

void
FOdysseyBrushDetails::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    iBuilder.SortCategories(&SortCategories);
}

void
FOdysseyBrushCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyBrushAssetBase::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyBrushDetails::MakeInstance));
}
