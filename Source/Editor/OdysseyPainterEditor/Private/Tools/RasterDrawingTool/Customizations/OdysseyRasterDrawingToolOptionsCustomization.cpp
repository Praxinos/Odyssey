// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyRasterDrawingToolOptionsCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "Tools/RasterDrawingTool/OdysseyRasterDrawingTool.h"

#define LOCTEXT_NAMESPACE "OdysseyRasterDrawingToolOptionsCustomization"

FOdysseyRasterDrawingToolOptionsCustomization::FOdysseyRasterDrawingToolOptionsCustomization()
{
	mPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FOdysseyRasterDrawingToolOptionsCustomization::OnObjectPostEditChange);
}

FOdysseyRasterDrawingToolOptionsCustomization::~FOdysseyRasterDrawingToolOptionsCustomization()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(mPropertyChangedHandle);
}

UOdysseyRasterDrawingTool*
FOdysseyRasterDrawingToolOptionsCustomization::GetRasterDrawingTool()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    return Cast<UOdysseyRasterDrawingTool>(objects[0]);
}

void
FOdysseyRasterDrawingToolOptionsCustomization::HideAllProperties()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    for (const FProperty* property : TFieldRange<FProperty>(objects[0]->GetClass()))
    {
        mBuilder->HideProperty(property->GetFName());
    }
}

void
FOdysseyRasterDrawingToolOptionsCustomization::AddObjectPropertyToCategory(IDetailCategoryBuilder& iCategory, UObject* iObject, FName iPropertyName)
{
    TArray<UObject*> objects; //contains only one object
    objects.Add(iObject);
    
    TSharedPtr<IPropertyHandle> handle = mBuilder->AddObjectPropertyData(objects, iPropertyName);
    if (handle.IsValid())
        iCategory.AddProperty(handle.ToSharedRef());
}

void
FOdysseyRasterDrawingToolOptionsCustomization::AddObjectToCategoryInline(IDetailCategoryBuilder& iCategory, UObject* iObject)
{
    TArray<UObject*> objects; //contains only one object
    objects.Add(iObject);

    for (const FProperty* property : TFieldRange<FProperty>(iObject->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = mBuilder->AddObjectPropertyData(objects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;

        iCategory.AddProperty(propertyHandle.ToSharedRef());
    }
}

void
FOdysseyRasterDrawingToolOptionsCustomization::AddSelectedShapeInstance(IDetailCategoryBuilder& iCategory)
{
    UObject* shapeInstance = mTool->GetSelectedShapeInstance();
    if (!shapeInstance)
        return;

    AddObjectPropertyToCategory(iCategory, shapeInstance, "Step");
    AddObjectPropertyToCategory(iCategory, shapeInstance, "AdaptativeStep");
    AddObjectPropertyToCategory(iCategory, shapeInstance, "InterpolationType");

    TArray<UObject*> objects; //contains only one object
    objects.Add(shapeInstance);

    TMap<FName, IDetailGroup*> groups;

    //Do properties without category metadata
    for (const FProperty* property : TFieldRange<FProperty>(shapeInstance->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = mBuilder->AddObjectPropertyData(objects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;

        if (property->GetFName() == "Step" || property->GetFName() == "AdaptativeStep" || property->GetFName() == "InterpolationType")
            continue;
            
        iCategory.AddProperty(propertyHandle.ToSharedRef());
    }
}

void
FOdysseyRasterDrawingToolOptionsCustomization::AddBlendParameters()
{
    IDetailCategoryBuilder& blendingCategory = mBuilder->EditCategory("BlendingCategory", LOCTEXT("ShapeCategory", "Blending"), ECategoryPriority::Default);
    TSharedPtr<IPropertyHandle> blendingParametersHandle = mBuilder->GetProperty("BlendParameters");

    uint32 numChildren = 0;
    blendingParametersHandle->GetNumChildren(numChildren);

    for (uint32 i = 0; i < numChildren; i++)
    {
        TSharedPtr<IPropertyHandle> propertyHandle = blendingParametersHandle->GetChildHandle(i);
        blendingCategory.AddProperty(propertyHandle);
    }
}

void
FOdysseyRasterDrawingToolOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    mBuilder = &iBuilder;
    mTool = GetRasterDrawingTool();

    HideAllProperties();

    //Categories
    IDetailCategoryBuilder& shapeCategory = mBuilder->EditCategory("ShapeCategory", LOCTEXT("ShapeCategory", "Shape"), ECategoryPriority::Default);

    //Add Shape Options
    AddSelectedShapeInstance(shapeCategory);
    AddBlendParameters();
}

void 
FOdysseyRasterDrawingToolOptionsCustomization::OnObjectPostEditChange( UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent )
{
	if (mTool != iObject)
        return;

    FName PropertyName = iPropertyChangedEvent.GetPropertyName();
    if (PropertyName == "SelectedShapeInstance" && iPropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
    {
        //Save ExpansionState before refreshing for consistency
        mBuilder->ForceRefreshDetails();

        // Defer the update 1 frame to ensure that we don't end up in a recursive loop adding bindings to the OnObjectPropertyChanged delegate that is currently being triggered
        // (since the bindings are added in FMovieSceneCaptureCustomization::CustomizeDetails)
        //TSharedRef<IPropertyUtilities> propertyUtilities = mBuilder->GetPropertyUtilities();
        //propertyUtilities->EnqueueDeferredAction(FSimpleDelegate::CreateLambda([mBuilder]{ mBuilder->GetPropertyUtilities()->ForceRefresh(); }));
    }
}

#undef LOCTEXT_NAMESPACE
