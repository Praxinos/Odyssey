// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyDrawingToolOptionsCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"

#define LOCTEXT_NAMESPACE "OdysseyDrawingToolOptionsCustomization"

FOdysseyDrawingToolOptionsCustomization::FOdysseyDrawingToolOptionsCustomization()
{
	mPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FOdysseyDrawingToolOptionsCustomization::OnObjectPostEditChange);
}

FOdysseyDrawingToolOptionsCustomization::~FOdysseyDrawingToolOptionsCustomization()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(mPropertyChangedHandle);
}

UOdysseyDrawingTool*
FOdysseyDrawingToolOptionsCustomization::GetDrawingTool()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    return Cast<UOdysseyDrawingTool>(objects[0]);
}

void
FOdysseyDrawingToolOptionsCustomization::HideAllProperties()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    for (const FProperty* property : TFieldRange<FProperty>(objects[0]->GetClass()))
    {
        mBuilder->HideProperty(property->GetFName());
    }
}

void
FOdysseyDrawingToolOptionsCustomization::AddObjectPropertyToCategory(IDetailCategoryBuilder& iCategory, UObject* iObject, FName iPropertyName)
{
    TArray<UObject*> objects; //contains only one object
    objects.Add(iObject);
    
    TSharedPtr<IPropertyHandle> handle = mBuilder->AddObjectPropertyData(objects, iPropertyName);
    if (handle.IsValid())
        iCategory.AddProperty(handle.ToSharedRef());
}

void
FOdysseyDrawingToolOptionsCustomization::AddObjectToCategoryInline(IDetailCategoryBuilder& iCategory, UObject* iObject)
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
FOdysseyDrawingToolOptionsCustomization::AddSelectedShapeInstance(IDetailCategoryBuilder& iCategory)
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
FOdysseyDrawingToolOptionsCustomization::AddBlendParameters()
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
FOdysseyDrawingToolOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    mBuilder = &iBuilder;
    mTool = GetDrawingTool();

    HideAllProperties();

    //Categories
    IDetailCategoryBuilder& shapeCategory = mBuilder->EditCategory("ShapeCategory", LOCTEXT("ShapeCategory", "Shape"), ECategoryPriority::Default);

    //Add Shape Options
    AddSelectedShapeInstance(shapeCategory);
    AddBlendParameters();
}

void 
FOdysseyDrawingToolOptionsCustomization::OnObjectPostEditChange( UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent )
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
