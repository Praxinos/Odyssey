// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyDrawingToolBrushSelectorCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"
#include "ObjectEditorUtils.h"
#include "Brush/SOdysseyBrushSelector.h"

#define LOCTEXT_NAMESPACE "OdysseyDrawingToolBrushSelectorCustomization"

FOdysseyDrawingToolBrushSelectorCustomization::FOdysseyDrawingToolBrushSelectorCustomization()
{
	mPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FOdysseyDrawingToolBrushSelectorCustomization::OnObjectPostEditChange);
}

FOdysseyDrawingToolBrushSelectorCustomization::~FOdysseyDrawingToolBrushSelectorCustomization()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(mPropertyChangedHandle);
}

UOdysseyDrawingTool*
FOdysseyDrawingToolBrushSelectorCustomization::GetDrawingTool()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    return Cast<UOdysseyDrawingTool>(objects[0]);
}

void
FOdysseyDrawingToolBrushSelectorCustomization::HideAllProperties()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    for (const FProperty* property : TFieldRange<FProperty>(objects[0]->GetClass()))
    {
        mBuilder->HideProperty(property->GetFName());
    }
}

void
FOdysseyDrawingToolBrushSelectorCustomization::AddObjectPropertyToCategory(IDetailCategoryBuilder& iCategory, UObject* iObject, FName iPropertyName)
{
    TArray<UObject*> objects; //contains only one object
    objects.Add(iObject);
    
    TSharedPtr<IPropertyHandle> handle = mBuilder->AddObjectPropertyData(objects, iPropertyName);
    if (handle.IsValid())
        iCategory.AddProperty(handle.ToSharedRef());
}

void
FOdysseyDrawingToolBrushSelectorCustomization::AddObjectToCategoryInline(IDetailCategoryBuilder& iCategory, UObject* iObject)
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
FOdysseyDrawingToolBrushSelectorCustomization::AddBrushInstance(IDetailCategoryBuilder& iCategory)
{
    UObject* brushInstance = mTool->GetBrushInstance();
    if (!brushInstance)
        return;

    TArray<UObject*> objects; //contains only one object
    objects.Add(brushInstance);

    TMap<FName, IDetailGroup*> groups;

    //Do properties without category metadata
    for (const FProperty* property : TFieldRange<FProperty>(brushInstance->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = mBuilder->AddObjectPropertyData(objects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;

        // hide blueprint properties that should not be displayed
        if (property->HasAnyPropertyFlags(CPF_DisableEditOnInstance))
            continue;

        if (propertyHandle->HasMetaData("Category"))
        {
            //By default if there is no category specified by UPROPERTY, the category is set to Class->GetName()
            //That's why we check if it's the case, to determine if the category is explicitly specified or not
            //(Does not work with blueprint classes)
            FName groupName = propertyHandle->GetDefaultCategoryName();
            if (groupName != brushInstance->GetClass()->GetFName())
                continue;
        }

        bool bShouldAutoExpand = property->GetFName() == "BlendParameters";
        iCategory.AddProperty(propertyHandle.ToSharedRef()).ShouldAutoExpand(bShouldAutoExpand);;
    }

    //Do properties with category metadata
    for (const FProperty* property : TFieldRange<FProperty>(brushInstance->GetClass()))
    {
        TSharedPtr<IPropertyHandle> propertyHandle = mBuilder->AddObjectPropertyData(objects, property->GetFName());
        if (!propertyHandle.IsValid())
            continue;

        // hide blueprint properties that should not be displayed
        if (property->HasAnyPropertyFlags(CPF_DisableEditOnInstance))
            continue;

        //make group
        if (!propertyHandle->HasMetaData("Category"))
            continue;

        FName groupName = propertyHandle->GetDefaultCategoryName();

        //By default if there is no category specified by UPROPERTY, the category is set to Class->GetName()
        //That's why we check if it's the case, to determine if the category is explicitly specified or not
        //(Does not work with blueprint classes)
        if (groupName == brushInstance->GetClass()->GetFName())
            continue;

        FText groupText = propertyHandle->GetDefaultCategoryText();
        IDetailGroup** foundGroup = groups.Find(groupName);
        if (!foundGroup)
            foundGroup = &groups.Add(groupName, &iCategory.AddGroup(groupName, groupText, false, true));

        bool bShouldAutoExpand = property->GetFName() == "BlendParameters";

        (*foundGroup)->AddPropertyRow(propertyHandle.ToSharedRef()).ShouldAutoExpand(bShouldAutoExpand);
    }
}

void
FOdysseyDrawingToolBrushSelectorCustomization::AddBrushSelector(IDetailCategoryBuilder& iCategory)
{
    iCategory.AddCustomRow(LOCTEXT("BrushSelector", "Brush"), false)
    [
        SNew(SOdysseyBrushSelector)
        .Brush_Raw(this, &FOdysseyDrawingToolBrushSelectorCustomization::GetBrush)
        .OnBrushChanged(this, &FOdysseyDrawingToolBrushSelectorCustomization::OnBrushChanged)
    ];
}

void
FOdysseyDrawingToolBrushSelectorCustomization::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    mBuilder = &iBuilder;
    mTool = GetDrawingTool();

    HideAllProperties();

    //Categories
    IDetailCategoryBuilder& globalsCategory = iBuilder.EditCategory("Globals", LOCTEXT("GlobalsCategory", "Globals"), ECategoryPriority::Default);
    IDetailCategoryBuilder& brushParametersCategory = iBuilder.EditCategory("BrushParameters", LOCTEXT("BrushParametersCategory", "Brush Parameters"), ECategoryPriority::Default);

    AddBrushSelector(globalsCategory);
    
    AddObjectPropertyToCategory(globalsCategory, mTool->GetBrushOptions(), "Size");
    AddObjectPropertyToCategory(globalsCategory, mTool->GetBrushOptions(), "Flow");
    globalsCategory.AddProperty("BlendParameters").ShouldAutoExpand(true);

    AddBrushInstance(brushParametersCategory);
}

void
FOdysseyDrawingToolBrushSelectorCustomization::OnBrushChanged(UOdysseyBrush* iBrush)
{
    FObjectEditorUtils::SetPropertyValue(mTool, "Brush", iBrush);
}

UOdysseyBrush*
FOdysseyDrawingToolBrushSelectorCustomization::GetBrush() const
{
    return mTool->GetBrush();
}

void 
FOdysseyDrawingToolBrushSelectorCustomization::OnObjectPostEditChange( UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent )
{
	if (mTool != iObject)
        return;

    FName PropertyName = iPropertyChangedEvent.GetPropertyName();
    if (PropertyName == "BrushInstance" && iPropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
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
