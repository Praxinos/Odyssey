// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorRasterDrawingToolCustomization.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditorSettings.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "ISinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorRasterDrawingTool*
FOdysseyPainterEditorRasterDrawingToolCustomization::GetTool( const IDetailLayoutBuilder& iDetailBuilder )
{
    TArray<TWeakObjectPtr<UObject>> objects;

    iDetailBuilder.GetObjectsBeingCustomized( objects );

    return Cast<UOdysseyPainterEditorRasterDrawingTool>( objects[0].Get() );
}

void
FOdysseyPainterEditorRasterDrawingToolCustomization::CustomizeDetails( IDetailLayoutBuilder& iDetailBuilder )
{
    FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    IDetailCategoryBuilder& categoryBuilder = iDetailBuilder.EditCategory(
        TEXT("Brush"),
        FText::GetEmpty(),
        ECategoryPriority::Important
    );

    mTool = GetTool(iDetailBuilder);

    //Header
    FSinglePropertyParams brushPropertyParams;
    brushPropertyParams.NamePlacement = EPropertyNamePlacement::Hidden;
    brushPropertyParams.bHideResetToDefault = true;

    TSharedRef<IPropertyHandle> brushHandle = iDetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Brush));
    TSharedRef<IPropertyHandle> brushInstanceHandle = iDetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BrushInstance));

    FIsResetToDefaultVisible brushIsResetToDefaultVisible = FIsResetToDefaultVisible::CreateLambda(
        [tool = mTool](TSharedPtr<IPropertyHandle> iHandle)
        {
            UOdysseyPainterEditorSettings* settings = UOdysseyPainterEditorSettings::Get();
            UOdysseyBrush* brush = settings->BrushDefaults.DefaultBrush.LoadSynchronous();
            return tool->GetBrush() != brush;
        }
    );

    FResetToDefaultHandler brushOnResetToDefaultClicked = FResetToDefaultHandler::CreateLambda(
        [tool = mTool](TSharedPtr<IPropertyHandle> iHandle)
        {
            UOdysseyPainterEditorSettings* settings = UOdysseyPainterEditorSettings::Get();
            UOdysseyBrush* brush = settings->BrushDefaults.DefaultBrush.LoadSynchronous();
            return tool->SetBrush(brush);
        }
    );

    FResetToDefaultOverride resetToDefaultOverride = FResetToDefaultOverride::Create(brushIsResetToDefaultVisible, brushOnResetToDefaultClicked, true);

    IDetailPropertyRow* brushInstanceRow = iDetailBuilder.EditDefaultProperty(brushInstanceHandle);
    brushInstanceRow->ShouldAutoExpand(true);

    FDetailWidgetRow& widgetRow = brushInstanceRow->CustomWidget(true);
    widgetRow.OverrideResetToDefault(resetToDefaultOverride);
    widgetRow.ShouldAutoExpand(true);
    widgetRow.WholeRowContent()
    [
        propertyModule.CreateSingleProperty(mTool, "Brush", brushPropertyParams).ToSharedRef()
    ];
}

#undef LOCTEXT_NAMESPACE
