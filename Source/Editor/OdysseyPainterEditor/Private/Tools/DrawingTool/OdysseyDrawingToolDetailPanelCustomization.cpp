// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyDrawingToolDetailPanelCustomization.h"

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"

#define LOCTEXT_NAMESPACE "OdysseyDrawingToolDetailPanelCustomization"

class FOdysseyDrawingToolDetailCustomization : public IDetailCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShared<FOdysseyDrawingToolDetailCustomization>();
    }

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface
};

void
FOdysseyDrawingToolDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    IDetailCategoryBuilder& shapeCategory = iBuilder.EditCategory("Shape", LOCTEXT("ShapeCategory", "Shape"), ECategoryPriority::Default);
    IDetailCategoryBuilder& brushCategory = iBuilder.EditCategory("Brush", LOCTEXT("BrushCategory", "Brush"), ECategoryPriority::Default);

    iBuilder.HideProperty("SelectedShape");
    iBuilder.HideProperty("SelectedShapeInstance");
    iBuilder.HideProperty("BrushInstance");

    shapeCategory.AddProperty("SelectedShape");

    TArray< TWeakObjectPtr<UObject> > objects;
    iBuilder.GetObjectsBeingCustomized( objects );

    UOdysseyDrawingTool* drawingTool = Cast<UOdysseyDrawingTool>(objects[0]);

    //shape
    TSharedRef<IPropertyHandle> shapeHandle = iBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyDrawingTool, SelectedShapeInstance));
    if (!iBuilder.IsPropertyVisible(shapeHandle))
        return;

    UObject* shapeObject = drawingTool->SelectedShapeInstance;
    if (!shapeObject)
        return;

    TArray<UObject*> shapeObjects; //contains only one object
    shapeObjects.Add(shapeObject);

    for (const FProperty* property : TFieldRange<FProperty>(shapeObject->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = iBuilder.AddObjectPropertyData(shapeObjects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;
        
        if (!iBuilder.IsPropertyVisible(propertyHandle.ToSharedRef()))
            continue;

        shapeCategory.AddProperty(propertyHandle.ToSharedRef());
    }

    //Brush
    TSharedRef<IPropertyHandle> brushHandle = iBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyDrawingTool, BrushInstance));
    if (!iBuilder.IsPropertyVisible(brushHandle))
        return;

    UObject* brushObject = drawingTool->BrushInstance;
    if (!brushObject)
        return;

    TArray<UObject*> brushObjects; //contains only one object
    brushObjects.Add(brushObject);

    for (const FProperty* property : TFieldRange<FProperty>(brushObject->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = iBuilder.AddObjectPropertyData(brushObjects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;

        if (!iBuilder.IsPropertyVisible(propertyHandle.ToSharedRef()))
            continue;

        brushCategory.AddProperty(propertyHandle.ToSharedRef());
    }
}

void
FOdysseyDrawingToolDetailPanelCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyDrawingTool::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyDrawingToolDetailCustomization::MakeInstance));
}

#undef LOCTEXT_NAMESPACE
