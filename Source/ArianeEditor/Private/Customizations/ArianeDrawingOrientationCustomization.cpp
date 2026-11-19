// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeDrawingOrientationCustomization.h"

// Ariane Editor headers
#include "ArianeEditorModule.h"
#include "LayerStack/ArianeLayerVectorEnums.h"
#include "Styles/ArianeEditorStyle.h"
// Unreal Editor headers
#include "Widgets/Input/SSegmentedControl.h"
#include "DetailWidgetRow.h"

TSharedRef<IPropertyTypeCustomization>
FArianeDrawingOrientationCustomization::MakeInstance()
{
    return MakeShared<FArianeDrawingOrientationCustomization>();
}

void
FArianeDrawingOrientationCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    HeaderRow
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(200.0f)
        [
            SNew(SSegmentedControl<EArianeDrawingOrientation>)
                .Value_Lambda([PropertyHandle]()
                    {
                        uint8 Value = 0;
                        PropertyHandle->GetValue(Value);

                        return static_cast<EArianeDrawingOrientation>(Value);
                    })
                .SupportsEmptySelection(false)
                .SupportsMultiSelection(false)

                .OnValueChanged_Lambda(
                    [PropertyHandle](EArianeDrawingOrientation NewValue)
                    {
                        PropertyHandle->SetValue(
                            static_cast<uint8>(NewValue)
                        );
                    })

                + SSegmentedControl<EArianeDrawingOrientation>::Slot(
                    EArianeDrawingOrientation::View)
                [
                    SNew(SImage)
                        .Image(FArianeEditorStyle::Get().GetBrush("ArianeEditor.DrawingOrientation.View20"))
                ]

                + SSegmentedControl<EArianeDrawingOrientation>::Slot(
                    EArianeDrawingOrientation::XY)
                [
                    SNew(SImage)
                        .Image(FArianeEditorStyle::Get().GetBrush("ArianeEditor.DrawingOrientation.LayerXY20"))
                ]

                + SSegmentedControl<EArianeDrawingOrientation>::Slot(
                    EArianeDrawingOrientation::YZ)
                [
                    SNew(SImage)
                        .Image(FArianeEditorStyle::Get().GetBrush("ArianeEditor.DrawingOrientation.LayerYZ20"))
                ]

                + SSegmentedControl<EArianeDrawingOrientation>::Slot(
                    EArianeDrawingOrientation::ZX)
                [
                    SNew(SImage)
                        .Image(FArianeEditorStyle::Get().GetBrush("ArianeEditor.DrawingOrientation.LayerZX20"))
                ]
        ];
}

void FArianeDrawingOrientationCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{

}
