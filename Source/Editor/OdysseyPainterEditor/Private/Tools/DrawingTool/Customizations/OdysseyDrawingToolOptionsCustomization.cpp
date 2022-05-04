// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

    TArray<UObject*> objects; //contains only one object
    objects.Add(shapeInstance);

    TMap<FName, IDetailGroup*> groups;

    //Do properties without category metadata
    for (const FProperty* property : TFieldRange<FProperty>(shapeInstance->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = mBuilder->AddObjectPropertyData(objects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;

        if (propertyHandle->HasMetaData("Category"))
        {
            //By default if there is no category specified by UPROPERTY, the category is set to Class->GetName()
            //That's why we check if it's the case, to determine if the category is explicitly specified or not
            //(Does not work with blueprint classes)
            FName groupName = propertyHandle->GetDefaultCategoryName();
            if (groupName != shapeInstance->GetClass()->GetFName())
                continue;
        }

        bool bShouldAutoExpand = property->GetFName() == "SmoothingOptions";
        iCategory.AddProperty(propertyHandle.ToSharedRef()).ShouldAutoExpand(bShouldAutoExpand);
    }

    //Do properties with category metadata
    for (const FProperty* property : TFieldRange<FProperty>(shapeInstance->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = mBuilder->AddObjectPropertyData(objects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;

        //make group
        if (!propertyHandle->HasMetaData("Category"))
            continue;

        FName groupName = propertyHandle->GetDefaultCategoryName();

        //By default if there is no category specified by UPROPERTY, the category is set to Class->GetName()
        //That's why we check if it's the case, to determine if the category is explicitly specified or not
        //(Does not work with blueprint classes)
        if (groupName == shapeInstance->GetClass()->GetFName())
            continue;
            
        FText groupText = propertyHandle->GetDefaultCategoryText();
        IDetailGroup** foundGroup = groups.Find(groupName);
        if (!foundGroup)
            foundGroup = &groups.Add(groupName, &iCategory.AddGroup(groupName, groupText, false, true));

        bool bShouldAutoExpand = property->GetFName() == "SmoothingOptions";
        (*foundGroup)->AddPropertyRow(propertyHandle.ToSharedRef()).ShouldAutoExpand(bShouldAutoExpand);
    }
}

void
FOdysseyDrawingToolOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    mBuilder = &iBuilder;
    mTool = GetDrawingTool();

    HideAllProperties();

    //Categories
    IDetailCategoryBuilder& shapeCategory = mBuilder->EditCategory("Shape", LOCTEXT("ShapeCategory", "Shape"), ECategoryPriority::Default);

    //shapeCategory.AddProperty("SelectedShape");
    AddObjectPropertyToCategory(shapeCategory, mTool->GetBrushOptions(), "Step");
    AddObjectPropertyToCategory(shapeCategory, mTool->GetBrushOptions(), "AdaptativeStep");

    //Add Shape Options
    AddSelectedShapeInstance(shapeCategory);
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
