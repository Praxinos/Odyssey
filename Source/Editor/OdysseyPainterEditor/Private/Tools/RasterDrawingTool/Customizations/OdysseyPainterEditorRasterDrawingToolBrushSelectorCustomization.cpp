// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "ObjectEditorUtils.h"
#include "Brush/SOdysseyBrushSelector.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization"

FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization()
{
	mPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::OnObjectPostEditChange);
}

FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::~FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(mPropertyChangedHandle);
}

UOdysseyPainterEditorRasterDrawingTool*
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::GetPainterEditorRasterDrawingTool()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    return Cast<UOdysseyPainterEditorRasterDrawingTool>(objects[0]);
}

void
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::HideAllProperties()
{
    TArray< TWeakObjectPtr<UObject> > objects;
    mBuilder->GetObjectsBeingCustomized(objects);
    for (const FProperty* property : TFieldRange<FProperty>(objects[0]->GetClass()))
    {
        mBuilder->HideProperty(property->GetFName());
    }
}

void
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::AddObjectPropertyToCategory(IDetailCategoryBuilder& iCategory, UObject* iObject, FName iPropertyName)
{
    TArray<UObject*> objects; //contains only one object
    objects.Add(iObject);
    
    TSharedPtr<IPropertyHandle> handle = mBuilder->AddObjectPropertyData(objects, iPropertyName);
    if (handle.IsValid())
        iCategory.AddProperty(handle.ToSharedRef());
}

void
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::AddObjectToCategoryInline(IDetailCategoryBuilder& iCategory, UObject* iObject)
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
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::AddBrushInstance(IDetailCategoryBuilder& iCategory)
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

        //By default if there is no category specified by UPROPERTY, the category is set to Default
        if (propertyHandle->HasMetaData("Category") && propertyHandle->GetDefaultCategoryName() != "Default")
            continue;

        iCategory.AddProperty(propertyHandle.ToSharedRef());
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

        FName categoryName = propertyHandle->GetDefaultCategoryName();
        FText categoryText = propertyHandle->GetDefaultCategoryText();

        //By default if there is no category specified by UPROPERTY, the category is set to Default
        if (!propertyHandle->HasMetaData("Category") || categoryName == "Default")
            continue;

        //Categories
        IDetailCategoryBuilder& category = mBuilder->EditCategory(categoryName, categoryText, ECategoryPriority::Default);
        category.AddProperty(propertyHandle);
    }
}

void
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::AddBrushSelector(IDetailCategoryBuilder& iCategory)
{
    iCategory.AddCustomRow(LOCTEXT("BrushSelector", "Brush"), false)
    [
        SNew(SOdysseyBrushSelector)
        .Brush_Raw(this, &FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::GetBrush)
        .OnBrushChanged(this, &FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::OnBrushChanged)
    ];
}

void
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::CustomizeDetails(IDetailLayoutBuilder& iBuilder)
{
    mBuilder = &iBuilder;
    mTool = GetPainterEditorRasterDrawingTool();

    HideAllProperties();

    //Categories
    IDetailCategoryBuilder& noCategory = iBuilder.EditCategory("NoCategory", LOCTEXT("noCategory", "NoCategory"), ECategoryPriority::Default);

    AddBrushSelector(noCategory);
    
    AddObjectPropertyToCategory(noCategory, mTool->GetBrushOptions(), "Size");
    AddObjectPropertyToCategory(noCategory, mTool->GetBrushOptions(), "Flow");

    AddBrushInstance(noCategory);
}

void
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::OnBrushChanged(UOdysseyBrush* iBrush)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTool, "Brush", iBrush);
}

UOdysseyBrush*
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::GetBrush() const
{
    return mTool->GetBrush();
}

void 
FOdysseyPainterEditorRasterDrawingToolBrushSelectorCustomization::OnObjectPostEditChange( UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent )
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
