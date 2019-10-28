// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OdysseyBrushDetailsCustomization.h"
#include "UObject/StructOnScope.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Layout/SSpacer.h"
#include "DetailWidgetRow.h"
#include "Engine/UserDefinedStruct.h"
#include "Misc/MessageDialog.h"
#include "UObject/UObjectIterator.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Engine.h"
#include "EdMode.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EdGraph/EdGraphNode_Documentation.h"
#include "Layout/WidgetPath.h"
#include "SlateOptMacros.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "EdGraphNode_Comment.h"
#include "Components/ChildActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet2/ComponentEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_OdysseyBrush.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Variable.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_Tunnel.h"
#include "K2Node_Composite.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionTerminator.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_MathExpression.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "ScopedTransaction.h"
#include "PropertyRestriction.h"
#include "OdysseyBrushEditorModes.h"
#include "BlueprintEditorSettings.h"
#include "DetailLayoutBuilder.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"
#include "IDetailsView.h"
#include "Widgets/Colors/SColorPicker.h"
#include "SOdysseyInspector.h"
#include "SOdysseySCSEditor.h"
#include "SPinTypeSelector.h"
#include "NodeFactory.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#include "Modules/ModuleManager.h"
#include "ISequencerModule.h"
#include "AnimatedPropertyKey.h"

#include "PropertyCustomizationHelpers.h"

#include "Kismet2/BlueprintEditorUtils.h"

#include "ObjectEditorUtils.h"

#include "Editor/SceneOutliner/Private/SSocketChooser.h"

#include "IDocumentationPage.h"
#include "IDocumentation.h"
#include "Widgets/Input/STextComboBox.h"

#include "UObject/TextProperty.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushDetailsCustomization"

namespace OdysseyBrushDocumentationDetailDefs
{
    /** Minimum size of the details title panel */
    static const float DetailsTitleMinWidth = 125.f;
    /** Maximum size of the details title panel */
    static const float DetailsTitleMaxWidth = 300.f;
    /** magic number retrieved from SGraphNodeComment::GetWrapAt() */
    static const float DetailsTitleWrapPadding = 32.0f;
};

void FOdysseyBrushDetails::AddEventsCategory(IDetailLayoutBuilder& DetailBuilder, UProperty* VariableProperty)
{
    UBlueprint* OdysseyBrushObj = GetBlueprintObj();
    check(OdysseyBrushObj);

    if ( UObjectProperty* ComponentProperty = Cast<UObjectProperty>(VariableProperty) )
    {
        UClass* PropertyClass = ComponentProperty->PropertyClass;

        // Check for Ed Graph vars that can generate events
        if ( PropertyClass && OdysseyBrushObj->AllowsDynamicBinding() )
        {
            if ( FBlueprintEditorUtils::CanClassGenerateEvents(PropertyClass) )
            {
                for ( TFieldIterator<UMulticastDelegateProperty> PropertyIt(PropertyClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt )
                {
                    UProperty* Property = *PropertyIt;

                    FName PropertyName = ComponentProperty->GetFName();

                    // Check for multicast delegates that we can safely assign
                    if ( !Property->HasAnyPropertyFlags(CPF_Parm) && Property->HasAllPropertyFlags(CPF_BlueprintAssignable) )
                    {
                        FName EventName = Property->GetFName();
                        FText EventText = Property->GetDisplayNameText();

                        IDetailCategoryBuilder& EventCategory = DetailBuilder.EditCategory(TEXT("Events"), LOCTEXT("Events", "Events"), ECategoryPriority::Uncommon);

                        EventCategory.AddCustomRow(EventText)
                        .NameContent()
                        [
                            SNew(SHorizontalBox)
                            .ToolTipText(Property->GetToolTipText())

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            .VAlign(VAlign_Center)
                            .Padding(0, 0, 5, 0)
                            [
                                SNew(SImage)
                                .Image(FEditorStyle::GetBrush("GraphEditor.Event_16x"))
                            ]

                            + SHorizontalBox::Slot()
                            .VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .Font(IDetailLayoutBuilder::GetDetailFont())
                                .Text(EventText)
                            ]
                        ]
                        .ValueContent()
                        .MinDesiredWidth(150)
                        .MaxDesiredWidth(200)
                        [
                            SNew(SButton)
                            .ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
                            .HAlign(HAlign_Center)
                            .OnClicked(this, &FOdysseyBrushVarActionDetails::HandleAddOrViewEventForVariable, EventName, PropertyName, MakeWeakObjectPtr(PropertyClass))
                            .ForegroundColor(FSlateColor::UseForeground())
                            [
                                SNew(SWidgetSwitcher)
                                .WidgetIndex(this, &FOdysseyBrushVarActionDetails::HandleAddOrViewIndexForButton, EventName, PropertyName)

                                + SWidgetSwitcher::Slot()
                                [
                                    SNew(STextBlock)
                                    .Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
                                    .Text(LOCTEXT("ViewEvent", "View"))
                                ]

                                + SWidgetSwitcher::Slot()
                                [
                                    SNew(SImage)
                                    .Image(FEditorStyle::GetBrush("Plus"))
                                ]
                            ]
                        ];
                    }
                }
            }
        }
    }
}

FReply FOdysseyBrushDetails::HandleAddOrViewEventForVariable(const FName EventName, FName PropertyName, TWeakObjectPtr<UClass> PropertyClass)
{
    UBlueprint* OdysseyBrushObj = GetBlueprintObj();

    // Find the corresponding variable property in the OdysseyBrush
    UObjectProperty* VariableProperty = FindField<UObjectProperty>(OdysseyBrushObj->SkeletonGeneratedClass, PropertyName);

    if ( VariableProperty )
    {
        if ( !FKismetEditorUtilities::FindBoundEventForComponent(OdysseyBrushObj, EventName, VariableProperty->GetFName()) )
        {
            FKismetEditorUtilities::CreateNewBoundEventForClass(PropertyClass.Get(), EventName, OdysseyBrushObj, VariableProperty);
        }
        else
        {
            const UK2Node_ComponentBoundEvent* ExistingNode = FKismetEditorUtilities::FindBoundEventForComponent(OdysseyBrushObj, EventName, VariableProperty->GetFName());
            if ( ExistingNode )
            {
                FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ExistingNode);
            }
        }
    }

    return FReply::Handled();
}

int32 FOdysseyBrushDetails::HandleAddOrViewIndexForButton(const FName EventName, FName PropertyName) const
{
    UBlueprint* OdysseyBrushObj = GetBlueprintObj();

    if ( FKismetEditorUtilities::FindBoundEventForComponent(OdysseyBrushObj, EventName, PropertyName) )
    {
        return 0; // View
    }

    return 1; // Add
}

FOdysseyBrushVarActionDetails::~FOdysseyBrushVarActionDetails()
{
    if(MyOdysseyBrush.IsValid())
    {
        // Remove the callback delegate we registered for
        TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditor = MyOdysseyBrush.Pin()->GetBlueprintEditor();
        if( OdysseyBrushEditor.IsValid() )
        {
            OdysseyBrushEditor.Pin()->OnRefresh().RemoveAll(this);
        }
    }
}

// UProperty Detail Customization
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FOdysseyBrushVarActionDetails::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
    DetailLayout.GetObjectsBeingCustomized(ObjectsBeingEdited);

    CachedVariableProperty = SelectionAsProperty();

    if(!CachedVariableProperty.IsValid())
    {
        return;
    }

    CachedVariableName = GetVariableName();

    TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditor = MyOdysseyBrush.Pin()->GetBlueprintEditor();
    if( OdysseyBrushEditor.IsValid() )
    {
        OdysseyBrushEditor.Pin()->OnRefresh().AddSP(this, &FOdysseyBrushVarActionDetails::OnPostEditorRefresh);
    }


    UProperty* VariableProperty = CachedVariableProperty.Get();

    // Cache the OdysseyBrush which owns this VariableProperty
    if (UBlueprintGeneratedClass* GeneratedClass = Cast<UBlueprintGeneratedClass>(VariableProperty->GetOwnerClass()))
    {
        PropertyOwnerOdysseyBrush = Cast<UBlueprint>(GeneratedClass->ClassGeneratedBy);
    }

    const UEdGraphSchema_OdysseyBrush* Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

    IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Variable", LOCTEXT("VariableDetailsCategory", "Variable"));
    const FSlateFontInfo DetailFontInfo = IDetailLayoutBuilder::GetDetailFont();

    const FString DocLink = TEXT("Shared/Editors/OdysseyBrushEditor/VariableDetails");

    TSharedPtr<SToolTip> VarNameTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarNameTooltip", "The name of the variable."), NULL, DocLink, TEXT("VariableName"));

    Category.AddCustomRow( LOCTEXT("OdysseyBrushVarActionDetails_VariableNameLabel", "Variable Name") )
    .NameContent()
    [
        SNew(STextBlock)
        .Text(LOCTEXT("OdysseyBrushVarActionDetails_VariableNameLabel", "Variable Name"))
        .ToolTip(VarNameTooltip)
        .Font(IDetailLayoutBuilder::GetDetailFont())
    ]
    .ValueContent()
    .MaxDesiredWidth(250.0f)
    [
        SAssignNew(VarNameEditableTextBox, SEditableTextBox)
        .Text(this, &FOdysseyBrushVarActionDetails::OnGetVarName)
        .ToolTip(VarNameTooltip)
        .OnTextChanged(this, &FOdysseyBrushVarActionDetails::OnVarNameChanged)
        .OnTextCommitted(this, &FOdysseyBrushVarActionDetails::OnVarNameCommitted)
        .IsReadOnly(this, &FOdysseyBrushVarActionDetails::GetVariableNameChangeEnabled)
        .Font(IDetailLayoutBuilder::GetDetailFont())
    ];

    TSharedPtr<SToolTip> VarTypeTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarTypeTooltip", "The type of the variable."), NULL, DocLink, TEXT("VariableType"));

    Category.AddCustomRow( LOCTEXT("VariableTypeLabel", "Variable Type") )
    .NameContent()
    [
        SNew(STextBlock)
        .Text( LOCTEXT("VariableTypeLabel", "Variable Type") )
        .ToolTip(VarTypeTooltip)
        .Font(DetailFontInfo)
    ]
    .ValueContent()
    .MaxDesiredWidth(980.f)
    [
        SNew(SPinTypeSelector, FGetPinTypeTree::CreateUObject(Schema, &UEdGraphSchema_OdysseyBrush::GetVariableTypeTree))
        .TargetPinType(this, &FOdysseyBrushVarActionDetails::OnGetVarType)
        .OnPinTypeChanged(this, &FOdysseyBrushVarActionDetails::OnVarTypeChanged)
        .IsEnabled(this, &FOdysseyBrushVarActionDetails::GetVariableTypeChangeEnabled)
        .Schema(Schema)
        .TypeTreeFilter(ETypeTreeFilter::None)
        .Font( DetailFontInfo )
        .ToolTip(VarTypeTooltip)
    ];

    TSharedPtr<SToolTip> EditableTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarEditableTooltip", "Whether this variable is publicly editable on instances of this OdysseyBrush."), NULL, DocLink, TEXT("Editable"));

    Category.AddCustomRow( LOCTEXT("IsVariableEditableLabel", "Instance Editable") )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ShowEditableCheckboxVisibilty))
    .NameContent()
    [
        SNew(STextBlock)
        .Text( LOCTEXT("IsVariableEditableLabel", "Instance Editable") )
        .ToolTip(EditableTooltip)
        .Font( IDetailLayoutBuilder::GetDetailFont() )
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked( this, &FOdysseyBrushVarActionDetails::OnEditableCheckboxState )
        .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnEditableChanged )
        .IsEnabled(IsVariableInOdysseyBrush())
        .ToolTip(EditableTooltip)
    ];

    /*
    TSharedPtr<SToolTip> ReadOnlyTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarReadOnlyTooltip", "Whether this variable can be set by OdysseyBrush nodes or if it is read-only."), NULL, DocLink, TEXT("ReadOnly"));

    Category.AddCustomRow(LOCTEXT("IsVariableReadOnlyLabel", "OdysseyBrush Read Only"))
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ShowReadOnlyCheckboxVisibilty))
    .NameContent()
    [
        SNew(STextBlock)
        .Text(LOCTEXT("IsVariableReadOnlyLabel", "OdysseyBrush Read Only"))
        .ToolTip(ReadOnlyTooltip)
        .Font(IDetailLayoutBuilder::GetDetailFont())
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked(this, &FOdysseyBrushVarActionDetails::OnReadyOnlyCheckboxState)
        .OnCheckStateChanged(this, &FOdysseyBrushVarActionDetails::OnReadyOnlyChanged)
        .IsEnabled(IsVariableInOdysseyBrush())
        .ToolTip(ReadOnlyTooltip)
    ];
    */

    TSharedPtr<SToolTip> ToolTipTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarToolTipTooltip", "Extra information about this variable, shown when cursor is over it."), NULL, DocLink, TEXT("Tooltip"));

    Category.AddCustomRow( LOCTEXT("IsVariableToolTipLabel", "Tooltip") )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::IsTooltipEditVisible))
    .NameContent()
    [
        SNew(STextBlock)
        .Text( LOCTEXT("IsVariableToolTipLabel", "Tooltip") )
        .ToolTip(ToolTipTooltip)
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SNew(SEditableTextBox)
        .Text( this, &FOdysseyBrushVarActionDetails::OnGetTooltipText )
        .ToolTip(ToolTipTooltip)
        .OnTextCommitted( this, &FOdysseyBrushVarActionDetails::OnTooltipTextCommitted, CachedVariableName )
        .IsEnabled(IsVariableInOdysseyBrush())
        .Font( DetailFontInfo )
    ];

    /*
    TSharedPtr<SToolTip> Widget3DTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariableWidget3D_Tooltip", "When true, allows the user to tweak the vector variable by using a 3D transform widget in the viewport (usable when varible is public/enabled)."), NULL, DocLink, TEXT("Widget3D"));

    Category.AddCustomRow( LOCTEXT("VariableWidget3D_Prompt", "Show 3D Widget") )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::Show3DWidgetVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .ToolTip(Widget3DTooltip)
        .Text(LOCTEXT("VariableWidget3D_Prompt", "Show 3D Widget"))
        .Font( DetailFontInfo )
        .IsEnabled(Is3DWidgetEnabled())
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked( this, &FOdysseyBrushVarActionDetails::OnCreateWidgetCheckboxState )
        .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnCreateWidgetChanged )
        .IsEnabled(Is3DWidgetEnabled() && IsVariableInOdysseyBrush())
        .ToolTip(Widget3DTooltip)
    ];
    */

    /*
    TSharedPtr<SToolTip> ExposeOnSpawnTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariableExposeToSpawn_Tooltip", "Should this variable be exposed as a pin when spawning this OdysseyBrush?"), NULL, DocLink, TEXT("ExposeOnSpawn"));

    Category.AddCustomRow( LOCTEXT("VariableExposeToSpawnLabel", "Expose on Spawn") )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ExposeOnSpawnVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .ToolTip(ExposeOnSpawnTooltip)
        .Text( LOCTEXT("VariableExposeToSpawnLabel", "Expose on Spawn") )
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked( this, &FOdysseyBrushVarActionDetails::OnGetExposedToSpawnCheckboxState )
        .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnExposedToSpawnChanged )
        .IsEnabled(IsVariableInOdysseyBrush())
        .ToolTip(ExposeOnSpawnTooltip)
    ];
    */

    /*
    TSharedPtr<SToolTip> PrivateTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariablePrivate_Tooltip", "Should this variable be private (derived blueprints cannot modify it)?"), NULL, DocLink, TEXT("Private"));

    Category.AddCustomRow(LOCTEXT("VariablePrivate", "Private"))
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ExposePrivateVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .ToolTip(PrivateTooltip)
        .Text( LOCTEXT("VariablePrivate", "Private") )
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked( this, &FOdysseyBrushVarActionDetails::OnGetPrivateCheckboxState )
        .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnPrivateChanged )
        .IsEnabled(IsVariableInOdysseyBrush())
        .ToolTip(PrivateTooltip)
    ];
    */

    /*
    TSharedPtr<SToolTip> ExposeToCinematicsTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariableExposeToCinematics_Tooltip", "Should this variable be exposed for Matinee or Sequencer to modify?"), NULL, DocLink, TEXT("ExposeToCinematics"));

    Category.AddCustomRow( LOCTEXT("VariableExposeToCinematics", "Expose to Cinematics") )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ExposeToCinematicsVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .ToolTip(ExposeToCinematicsTooltip)
        .Text( LOCTEXT("VariableExposeToCinematics", "Expose to Cinematics") )
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked( this, &FOdysseyBrushVarActionDetails::OnGetExposedToCinematicsCheckboxState )
        .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnExposedToCinematicsChanged )
        .IsEnabled(IsVariableInOdysseyBrush())
        .ToolTip(ExposeToCinematicsTooltip)
    ];
    */

    /*
    FText LocalisedTooltip;
    if (IsConfigCheckBoxEnabled())
    {
        // Build the property specific config variable tool tip
        FFormatNamedArguments ConfigTooltipArgs;
        if (UClass* OwnerClass = VariableProperty->GetOwnerClass())
        {
            OwnerClass = OwnerClass->GetAuthoritativeClass();
            ConfigTooltipArgs.Add(TEXT("ConfigPath"), FText::FromString(OwnerClass->GetConfigName()));
            ConfigTooltipArgs.Add(TEXT("ConfigSection"), FText::FromString(OwnerClass->GetPathName()));
        }
        LocalisedTooltip = FText::Format(LOCTEXT("VariableExposeToConfig_Tooltip", "Should this variable read its default value from a config file if it is present?\r\n\r\nThis is used for customising variable default values and behavior between different projects and configurations.\r\n\r\nConfig file [{ConfigPath}]\r\nConfig section [{ConfigSection}]"), ConfigTooltipArgs);
    }
    else if (IsVariableInOdysseyBrush())
    {
        // mimics the error that UHT would throw
        LocalisedTooltip = LOCTEXT("ObjectVariableConfig_Tooltip", "Not allowed to use 'config' with object variables");
    }
    TSharedPtr<SToolTip> ExposeToConfigTooltip = IDocumentation::Get()->CreateToolTip(LocalisedTooltip, NULL, DocLink, TEXT("ExposeToConfig"));

    Category.AddCustomRow( LOCTEXT("VariableExposeToConfig", "Config Variable"), true )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ExposeConfigVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .ToolTip( ExposeToConfigTooltip )
        .Text( LOCTEXT("ExposeToConfigLabel", "Config Variable") )
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .ToolTip( ExposeToConfigTooltip )
        .IsChecked( this, &FOdysseyBrushVarActionDetails::OnGetConfigVariableCheckboxState )
        .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnSetConfigVariableState )
        .IsEnabled(this, &FOdysseyBrushVarActionDetails::IsConfigCheckBoxEnabled)
    ];
    */
    PopulateCategories(MyOdysseyBrush.Pin().Get(), CategorySource);
    TSharedPtr<SComboButton> NewComboButton;
    TSharedPtr<SListView<TSharedPtr<FText>>> NewListView;

    TSharedPtr<SToolTip> CategoryTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("EditCategoryName_Tooltip", "The category of the variable; editing this will place the variable into another category or create a new one."), NULL, DocLink, TEXT("Category"));

    Category.AddCustomRow( LOCTEXT("CategoryLabel", "Category") )
        .Visibility(GetPropertyOwnerOdysseyBrush()? EVisibility::Visible : EVisibility::Hidden)
    .NameContent()
    [
        SNew(STextBlock)
        .Text( LOCTEXT("CategoryLabel", "Category") )
        .ToolTip(CategoryTooltip)
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SAssignNew(NewComboButton, SComboButton)
        .ContentPadding(FMargin(0,0,5,0))
        .IsEnabled(this, &FOdysseyBrushVarActionDetails::GetVariableCategoryChangeEnabled)
        .ToolTip(CategoryTooltip)
        .ButtonContent()
        [
            SNew(SBorder)
            .BorderImage( FEditorStyle::GetBrush("NoBorder") )
            .Padding(FMargin(0, 0, 5, 0))
            [
                SNew(SEditableTextBox)
                    .Text(this, &FOdysseyBrushVarActionDetails::OnGetCategoryText)
                    .OnTextCommitted(this, &FOdysseyBrushVarActionDetails::OnCategoryTextCommitted, CachedVariableName )
                    .ToolTip(CategoryTooltip)
                    .SelectAllTextWhenFocused(true)
                    .RevertTextOnEscape(true)
                    .Font( DetailFontInfo )
            ]
        ]
        .MenuContent()
        [
            SNew(SVerticalBox)
            +SVerticalBox::Slot()
            .AutoHeight()
            .MaxHeight(400.0f)
            [
                SAssignNew(NewListView, SListView<TSharedPtr<FText>>)
                    .ListItemsSource(&CategorySource)
                    .OnGenerateRow(this, &FOdysseyBrushVarActionDetails::MakeCategoryViewWidget)
                    .OnSelectionChanged(this, &FOdysseyBrushVarActionDetails::OnCategorySelectionChanged)
            ]
        ]
    ];

    CategoryComboButton = NewComboButton;
    CategoryListView = NewListView;

    TSharedPtr<SToolTip> SliderRangeTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("SliderRange_Tooltip", "Allows setting the minimum and maximum values for the UI slider for this variable."), NULL, DocLink, TEXT("SliderRange"));

    FName UIMin = TEXT("UIMin");
    FName UIMax = TEXT("UIMax");
    Category.AddCustomRow( LOCTEXT("SliderRangeLabel", "Slider Range") )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::RangeVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .Text( LOCTEXT("SliderRangeLabel", "Slider Range") )
        .ToolTip(SliderRangeTooltip)
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SNew(SHorizontalBox)
        .ToolTip(SliderRangeTooltip)
        +SHorizontalBox::Slot()
        .FillWidth(1)
        [
            SNew(SEditableTextBox)
            .Text(this, &FOdysseyBrushVarActionDetails::OnGetMetaKeyValue, UIMin)
            .OnTextCommitted(this, &FOdysseyBrushVarActionDetails::OnMetaKeyValueChanged, UIMin)
            .IsEnabled(IsVariableInOdysseyBrush())
            .Font( DetailFontInfo )
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( STextBlock )
            .Text( LOCTEXT("Min .. Max Separator", " .. ") )
            .Font(DetailFontInfo)
        ]
        +SHorizontalBox::Slot()
        .FillWidth(1)
        [
            SNew(SEditableTextBox)
            .Text(this, &FOdysseyBrushVarActionDetails::OnGetMetaKeyValue, UIMax)
            .OnTextCommitted(this, &FOdysseyBrushVarActionDetails::OnMetaKeyValueChanged, UIMax)
            .IsEnabled(IsVariableInOdysseyBrush())
            .Font( DetailFontInfo )
        ]
    ];

    TSharedPtr<SToolTip> ValueRangeTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("ValueRangeLabel_Tooltip", "The range of values allowed by this variable. Values outside of this will be clamped to the range."), NULL, DocLink, TEXT("ValueRange"));

    FName ClampMin = TEXT("ClampMin");
    FName ClampMax = TEXT("ClampMax");
    Category.AddCustomRow(LOCTEXT("ValueRangeLabel", "Value Range"))
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::RangeVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .Text(LOCTEXT("ValueRangeLabel", "Value Range"))
        .ToolTipText(LOCTEXT("ValueRangeLabel_Tooltip", "The range of values allowed by this variable. Values outside of this will be clamped to the range."))
        .ToolTip(ValueRangeTooltip)
        .Font(DetailFontInfo)
    ]
    .ValueContent()
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(1)
        [
            SNew(SEditableTextBox)
            .Text(this, &FOdysseyBrushVarActionDetails::OnGetMetaKeyValue, ClampMin)
            .OnTextCommitted(this, &FOdysseyBrushVarActionDetails::OnMetaKeyValueChanged, ClampMin)
            .IsEnabled(IsVariableInOdysseyBrush())
            .Font(DetailFontInfo)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("Min .. Max Separator", " .. "))
            .Font(DetailFontInfo)
        ]
        + SHorizontalBox::Slot()
        .FillWidth(1)
        [
            SNew(SEditableTextBox)
            .Text(this, &FOdysseyBrushVarActionDetails::OnGetMetaKeyValue, ClampMax)
            .OnTextCommitted(this, &FOdysseyBrushVarActionDetails::OnMetaKeyValueChanged, ClampMax)
            .IsEnabled(IsVariableInOdysseyBrush())
            .Font(DetailFontInfo)
        ]
    ];

    /*
    TSharedPtr<SToolTip> BitmaskTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarBitmaskTooltip", "Whether or not to treat this variable as a bitmask."), nullptr, DocLink, TEXT("Bitmask"));

    Category.AddCustomRow(LOCTEXT("IsVariableBitmaskLabel", "Bitmask"))
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::BitmaskVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .Text(LOCTEXT("IsVariableBitmaskLabel", "Bitmask"))
        .ToolTip(BitmaskTooltip)
        .Font(IDetailLayoutBuilder::GetDetailFont())
    ]
    .ValueContent()
    [
        SNew(SCheckBox)
        .IsChecked(this, &FOdysseyBrushVarActionDetails::OnBitmaskCheckboxState)
        .OnCheckStateChanged(this, &FOdysseyBrushVarActionDetails::OnBitmaskChanged)
        .IsEnabled(IsVariableInOdysseyBrush())
        .ToolTip(BitmaskTooltip)
    ];
    */

    /*
    BitmaskEnumTypeNames.Empty();
    BitmaskEnumTypeNames.Add(MakeShareable(new FString(LOCTEXT("BitmaskEnumTypeName_None", "None").ToString())));
    for (TObjectIterator<UEnum> EnumIt; EnumIt; ++EnumIt)
    {
        UEnum* CurrentEnum = *EnumIt;
        if (UEdGraphSchema_OdysseyBrush::IsAllowableBlueprintVariableType(CurrentEnum) && CurrentEnum->HasMetaData(TEXT("Bitflags")))
        {
            BitmaskEnumTypeNames.Add(MakeShareable(new FString(CurrentEnum->GetFName().ToString())));
        }
    }

    TSharedPtr<SToolTip> BitmaskEnumTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarBitmaskEnumTooltip", "If this is a bitmask, choose an optional enumeration type for the flags. Note that changing this will also reset the default value."), nullptr, DocLink, TEXT("Bitmask Flags"));

    Category.AddCustomRow(LOCTEXT("BitmaskEnumLabel", "Bitmask Enum"))
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::BitmaskVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .Text(LOCTEXT("BitmaskEnumLabel", "Bitmask Enum"))
        .ToolTip(BitmaskEnumTooltip)
        .Font(IDetailLayoutBuilder::GetDetailFont())
    ]
    .ValueContent()
    [
        SNew(STextComboBox)
        .OptionsSource(&BitmaskEnumTypeNames)
        .InitiallySelectedItem(GetBitmaskEnumTypeName())
        .OnSelectionChanged(this, &FOdysseyBrushVarActionDetails::OnBitmaskEnumTypeChanged)
        .IsEnabled(IsVariableInOdysseyBrush() && OnBitmaskCheckboxState() == ECheckBoxState::Checked)
    ];
    */

    /*
    ReplicationOptions.Empty();
    ReplicationOptions.Add(MakeShareable(new FString("None")));
    ReplicationOptions.Add(MakeShareable(new FString("Replicated")));
    ReplicationOptions.Add(MakeShareable(new FString("RepNotify")));

    TSharedPtr<SToolTip> ReplicationTooltip = IDocumentation::Get()->CreateToolTip( TAttribute<FText>::Create( TAttribute<FText>::FGetter::CreateRaw( this, &FOdysseyBrushVarActionDetails::ReplicationTooltip ) ), NULL, DocLink, TEXT("Replication"));

    Category.AddCustomRow( LOCTEXT("VariableReplicationLabel", "Replication") )
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ReplicationVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .ToolTip(ReplicationTooltip)
        .Text( LOCTEXT("VariableReplicationLabel", "Replication") )
        .Font( DetailFontInfo )
    ]
    .ValueContent()
    [
        SNew(STextComboBox)
        .OptionsSource( &ReplicationOptions )
        .InitiallySelectedItem(GetVariableReplicationType())
        .OnSelectionChanged( this, &FOdysseyBrushVarActionDetails::OnChangeReplication )
        .IsEnabled(this, &FOdysseyBrushVarActionDetails::ReplicationEnabled)
        .ToolTip(ReplicationTooltip)
    ];

    ReplicationConditionEnumTypeNames.Empty();
    UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ELifetimeCondition"), true);
    check(Enum);

    for (int32 i = 0; i < Enum->NumEnums(); i++)
    {
        if (!Enum->HasMetaData(TEXT("Hidden"), i))
        {
            ReplicationConditionEnumTypeNames.Add(MakeShareable(new FString(Enum->GetDisplayNameTextByIndex(i).ToString())));
        }
    }

    Category.AddCustomRow(LOCTEXT("VariableReplicationConditionsLabel", "Replication Condition"))
    .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::ReplicationVisibility))
    .NameContent()
    [
        SNew(STextBlock)
        .ToolTip(ReplicationTooltip)
        .Text(LOCTEXT("VariableReplicationConditionsLabel", "Replication Condition"))
        .Font(DetailFontInfo)
    ]
    .ValueContent()
    [
        SNew(STextComboBox)
        .OptionsSource(&ReplicationConditionEnumTypeNames)
        .InitiallySelectedItem(GetVariableReplicationCondition())
        .OnSelectionChanged(this, &FOdysseyBrushVarActionDetails::OnChangeReplicationCondition)
        .IsEnabled(this, &FOdysseyBrushVarActionDetails::ReplicationConditionEnabled)
    ];
    */

    UBlueprint* OdysseyBrushObj = GetBlueprintObj();

    // Handle event generation
    if ( FBlueprintEditorUtils::DoesSupportEventGraphs(OdysseyBrushObj) )
    {
        AddEventsCategory(DetailLayout, VariableProperty);
    }

    // Add in default value editing for properties that can be edited, local properties cannot be edited
    if ((OdysseyBrushObj != nullptr) && (OdysseyBrushObj->GeneratedClass != nullptr))
    {
        bool bVariableRenamed = false;
        if (VariableProperty != nullptr && IsVariableInOdysseyBrush())
        {
            // Determine the current property name on the CDO is stale
            if (PropertyOwnerOdysseyBrush.IsValid() && VariableProperty)
            {
                UBlueprint* PropertyOdysseyBrush = PropertyOwnerOdysseyBrush.Get();
                const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(PropertyOdysseyBrush, CachedVariableName);
                if (VarIndex != INDEX_NONE)
                {
                    const FGuid VarGuid = PropertyOdysseyBrush->NewVariables[VarIndex].VarGuid;
                    if (UBlueprintGeneratedClass* AuthoritiveBPGC = Cast<UBlueprintGeneratedClass>(PropertyOdysseyBrush->GeneratedClass))
                    {
                        if (const FName* OldName = AuthoritiveBPGC->PropertyGuids.FindKey(VarGuid))
                        {
                            bVariableRenamed = CachedVariableName != *OldName;
                        }
                    }
                }
            }
            const UProperty* OriginalProperty = nullptr;

            if(!IsALocalVariable(VariableProperty))
            {
                OriginalProperty = FindField<UProperty>(OdysseyBrushObj->GeneratedClass, VariableProperty->GetFName());
            }
            else
            {
                OriginalProperty = VariableProperty;
            }

            if (OriginalProperty == nullptr || bVariableRenamed)
            {
                // Prevent editing the default value of a skeleton property
                VariableProperty = nullptr;
            }
            else if (const UStructProperty* StructProperty = Cast<const UStructProperty>(OriginalProperty))
            {
                // Prevent editing the default value of a stale struct
                const UUserDefinedStruct* BGStruct = Cast<const UUserDefinedStruct>(StructProperty->Struct);
                if (BGStruct && (EUserDefinedStructureStatus::UDSS_UpToDate != BGStruct->Status))
                {
                    VariableProperty = nullptr;
                }
            }
        }

        // Find the class containing the variable
        UClass* VariableClass = (VariableProperty ? VariableProperty->GetTypedOuter<UClass>() : nullptr);

        FText ErrorMessage;
        IDetailCategoryBuilder& DefaultValueCategory = DetailLayout.EditCategory(TEXT("DefaultValueCategory"), LOCTEXT("DefaultValueCategoryHeading", "Default Value"));

        if (VariableProperty == nullptr)
        {
            if (OdysseyBrushObj->Status != BS_UpToDate)
            {
                ErrorMessage = LOCTEXT("VariableMissing_DirtyOdysseyBrush", "Please compile the blueprint");
            }
            else
            {
                ErrorMessage = LOCTEXT("VariableMissing_CleanOdysseyBrush", "Failed to find variable property");
            }
        }

        // Show the error message if something went wrong
        if (!ErrorMessage.IsEmpty())
        {
            DefaultValueCategory.AddCustomRow( ErrorMessage )
            [
                SNew(STextBlock)
                .ToolTipText(ErrorMessage)
                .Text(ErrorMessage)
                .Font(DetailFontInfo)
            ];
        }
        else
        {
            if(IsALocalVariable(VariableProperty))
            {
                UFunction* StructScope = Cast<UFunction>(VariableProperty->GetOuter());
                check(StructScope);

                TSharedPtr<FStructOnScope> StructData = MakeShareable(new FStructOnScope((UFunction*)StructScope));
                UEdGraph* Graph = FBlueprintEditorUtils::FindScopeGraph(GetBlueprintObj(), (UFunction*)StructScope);

                // Find the function entry nodes in the current graph
                TArray<UK2Node_FunctionEntry*> EntryNodes;
                Graph->GetNodesOfClass(EntryNodes);

                // There should always be an entry node in the function graph
                check(EntryNodes.Num() > 0);

                const UStructProperty* PotentialUDSProperty = Cast<const UStructProperty>(VariableProperty);

                UK2Node_FunctionEntry* FuncEntry = EntryNodes[0];
                for (const FBPVariableDescription& LocalVar : FuncEntry->LocalVariables)
                {
                    if(LocalVar.VarName == VariableProperty->GetFName()) //Property->GetFName())
                    {
                        // Only set the default value if there is one
                        if(!LocalVar.DefaultValue.IsEmpty())
                        {
                            FBlueprintEditorUtils::PropertyValueFromString(VariableProperty, LocalVar.DefaultValue, StructData->GetStructMemory());
                        }
                        break;
                    }
                }

                if(OdysseyBrushEditor.IsValid())
                {
                    TSharedPtr< IDetailsView > DetailsView  = OdysseyBrushEditor.Pin()->GetInspector()->GetPropertyView();

                    if(DetailsView.IsValid())
                    {
                        TWeakObjectPtr<UK2Node_EditablePinBase> EntryNode = FuncEntry;
                        DetailsView->OnFinishedChangingProperties().AddSP(this, &FOdysseyBrushVarActionDetails::OnFinishedChangingProperties, StructData, EntryNode);
                    }
                }

                IDetailPropertyRow* Row = DefaultValueCategory.AddExternalStructureProperty(StructData, VariableProperty->GetFName());
            }
            else
            {
                UBlueprint* CurrPropertyOwnerOdysseyBrush = IsVariableInheritedByOdysseyBrush() ? GetBlueprintObj() : GetPropertyOwnerOdysseyBrush();
                UObject* TarGetBlueprintDefaultObject = nullptr;
                if (CurrPropertyOwnerOdysseyBrush && CurrPropertyOwnerOdysseyBrush->GeneratedClass)
                {
                    TarGetBlueprintDefaultObject = CurrPropertyOwnerOdysseyBrush->GeneratedClass->GetDefaultObject();
                }
                else if (UBlueprint* PropertyOwnerBP = GetPropertyOwnerOdysseyBrush())
                {
                    TarGetBlueprintDefaultObject = PropertyOwnerBP->GeneratedClass->GetDefaultObject();
                }
                else if (CachedVariableProperty.IsValid())
                {
                    // Capture the non-BP class CDO so we can show the default value
                    TarGetBlueprintDefaultObject = CachedVariableProperty->GetOwnerClass()->GetDefaultObject();
                }

                if (TarGetBlueprintDefaultObject != nullptr)
                {
                    // Things are in order, show the property and allow it to be edited
                    TArray<UObject*> ObjectList;
                    ObjectList.Add(TarGetBlueprintDefaultObject);
                    IDetailPropertyRow* Row = DefaultValueCategory.AddExternalObjectProperty(ObjectList, VariableProperty->GetFName());
                    if (Row != nullptr)
                    {
                        Row->IsEnabled(IsVariableInheritedByOdysseyBrush());
                    }
                }
            }
        }

        /*
        TSharedPtr<SToolTip> TransientTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariableTransient_Tooltip", "Should this variable not serialize and be zero-filled at load?"), NULL, DocLink, TEXT("Transient"));

        Category.AddCustomRow(LOCTEXT("VariableTransient", "Transient"), true)
            .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::GetTransientVisibility))
            .NameContent()
        [
            SNew(STextBlock)
            .ToolTip(TransientTooltip)
            .Text( LOCTEXT("VariableTransient", "Transient") )
            .Font( DetailFontInfo )
        ]
        .ValueContent()
        [
            SNew(SCheckBox)
            .IsChecked( this, &FOdysseyBrushVarActionDetails::OnGetTransientCheckboxState )
            .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnTransientChanged )
            .IsEnabled(IsVariableInOdysseyBrush())
            .ToolTip(TransientTooltip)
        ];
        */

        /*
        TSharedPtr<SToolTip> SaveGameTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariableSaveGame_Tooltip", "Should this variable be serialized for saved games?"), NULL, DocLink, TEXT("SaveGame"));

        Category.AddCustomRow(LOCTEXT("VariableSaveGame", "SaveGame"), true)
        .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::GetSaveGameVisibility))
        .NameContent()
        [
            SNew(STextBlock)
            .ToolTip(SaveGameTooltip)
            .Text( LOCTEXT("VariableSaveGame", "SaveGame") )
            .Font( DetailFontInfo )
        ]
        .ValueContent()
        [
            SNew(SCheckBox)
            .IsChecked( this, &FOdysseyBrushVarActionDetails::OnGetSaveGameCheckboxState )
            .OnCheckStateChanged( this, &FOdysseyBrushVarActionDetails::OnSaveGameChanged )
            .IsEnabled(IsVariableInOdysseyBrush())
            .ToolTip(SaveGameTooltip)
        ];
        */

        /*
        TSharedPtr<SToolTip> AdvancedDisplayTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariableAdvancedDisplay_Tooltip", "Hide this variable in Class Defaults windows by default"), NULL, DocLink, TEXT("AdvancedDisplay"));

        Category.AddCustomRow(LOCTEXT("VariableAdvancedDisplay", "Advanced Display"), true)
            .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::GetAdvancedDisplayVisibility))
            .NameContent()
            [
                SNew(STextBlock)
                .ToolTip(AdvancedDisplayTooltip)
                .Text(LOCTEXT("VariableAdvancedDisplay", "Advanced Display"))
                .Font(DetailFontInfo)
            ]
        .ValueContent()
            [
                SNew(SCheckBox)
                .IsChecked(this, &FOdysseyBrushVarActionDetails::OnGetAdvancedDisplayCheckboxState)
                .OnCheckStateChanged(this, &FOdysseyBrushVarActionDetails::OnAdvancedDisplayChanged)
                .IsEnabled(IsVariableInOdysseyBrush())
                .ToolTip(AdvancedDisplayTooltip)
            ];
        */

        /*
        TSharedPtr<SToolTip> MultilineTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VariableMultilineTooltip_Tooltip", "Allow the value of this variable to have newlines (use Shift+Enter to add one while editing)"), NULL, DocLink, TEXT("Multiline"));

        Category.AddCustomRow(LOCTEXT("VariableMultilineTooltip", "Multi line"), true)
            .Visibility(TAttribute<EVisibility>(this, &FOdysseyBrushVarActionDetails::GetMultilineVisibility))
            .NameContent()
            [
                SNew(STextBlock)
                .ToolTip(MultilineTooltip)
                .Text(LOCTEXT("VariableMultiline", "Multi line"))
                .Font(DetailFontInfo)
            ]
        .ValueContent()
            [
                SNew(SCheckBox)
                .IsChecked(this, &FOdysseyBrushVarActionDetails::OnGetMultilineCheckboxState)
                .OnCheckStateChanged(this, &FOdysseyBrushVarActionDetails::OnMultilineChanged)
                .IsEnabled(IsVariableInOdysseyBrush())
                .ToolTip(MultilineTooltip)
            ];

        */

        /*
        TSharedPtr<SToolTip> PropertyFlagsTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("DefinedPropertyFlags_Tooltip", "List of defined flags for this property"), NULL, DocLink, TEXT("PropertyFlags"));

        Category.AddCustomRow(LOCTEXT("DefinedPropertyFlags", "Defined Property Flags"), true)
        .WholeRowWidget
        [
            SNew(STextBlock)
            .ToolTip(PropertyFlagsTooltip)
            .Text( LOCTEXT("DefinedPropertyFlags", "Defined Property Flags") )
            .Font( IDetailLayoutBuilder::GetDetailFontBold() )
        ];

        Category.AddCustomRow(FText::GetEmpty(), true)
        .WholeRowWidget
        [
            SAssignNew(PropertyFlagWidget, SListView< TSharedPtr< FString > >)
                .OnGenerateRow(this, &FOdysseyBrushVarActionDetails::OnGenerateWidgetForPropertyList)
                .ListItemsSource(&PropertyFlags)
                .SelectionMode(ESelectionMode::None)
                .ScrollbarVisibility(EVisibility::Collapsed)
                .ToolTip(PropertyFlagsTooltip)
        ];

        RefreshPropertyFlags();
        */
    }

    // See if anything else wants to customize our details
    FOdysseyBrushEditorModule& OdysseyBrushEditorModule = FModuleManager::GetModuleChecked<FOdysseyBrushEditorModule>("OdysseyBrushEditor");
    TArray<TSharedPtr<IDetailCustomization>> Customizations = OdysseyBrushEditorModule.CustomizeVariable(CachedVariableProperty->GetClass(), OdysseyBrushEditor.Pin());
    ExternalDetailCustomizations.Append(Customizations);
    for (TSharedPtr<IDetailCustomization> ExternalDetailCustomization : ExternalDetailCustomizations)
    {
        ExternalDetailCustomization->CustomizeDetails(DetailLayout);
    }
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FOdysseyBrushVarActionDetails::RefreshPropertyFlags()
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if(VariableProperty)
    {
        PropertyFlags.Empty();
        for( const TCHAR* PropertyFlag : ParsePropertyFlags(VariableProperty->PropertyFlags) )
        {
            PropertyFlags.Add(MakeShareable<FString>(new FString(PropertyFlag)));
        }

        PropertyFlagWidget.Pin()->RequestListRefresh();
    }
}

TSharedRef<ITableRow> FOdysseyBrushVarActionDetails::OnGenerateWidgetForPropertyList( TSharedPtr< FString > Item, const TSharedRef<STableViewBase>& OwnerTable )
{
    return SNew(STableRow< TSharedPtr< FString > >, OwnerTable)
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                    .Text(FText::FromString(*Item.Get()))
                    .ToolTipText(FText::FromString(*Item.Get()))
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
            ]

            +SHorizontalBox::Slot()
                .AutoWidth()
            [
                SNew(SCheckBox)
                    .IsChecked(true ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
                    .IsEnabled(false)
            ]
        ];
}

bool FOdysseyBrushVarActionDetails::IsAUserVariable(UProperty* VariableProperty) const
{
    UObjectProperty* VariableObjProp = VariableProperty ? Cast<UObjectProperty>(VariableProperty) : NULL;

    if (VariableObjProp != NULL && VariableObjProp->PropertyClass != NULL)
    {
        return FBlueprintEditorUtils::IsVariableCreatedByBlueprint(GetBlueprintObj(), VariableObjProp);
    }
    return true;
}

bool FOdysseyBrushVarActionDetails::IsASCSVariable(UProperty* VariableProperty) const
{
    UObjectProperty* VariableObjProp = VariableProperty ? Cast<UObjectProperty>(VariableProperty) : NULL;
    if (VariableObjProp != NULL && VariableObjProp->PropertyClass != NULL)
    {
        return (!IsAUserVariable(VariableProperty) && VariableObjProp->PropertyClass->IsChildOf(UActorComponent::StaticClass()));
    }
    return false;
}

bool FOdysseyBrushVarActionDetails::IsAOdysseyBrushVariable(UProperty* VariableProperty) const
{
    UClass* VarSourceClass = VariableProperty ? Cast<UClass>(VariableProperty->GetOuter()) : NULL;
    if(VarSourceClass)
    {
        return (VarSourceClass->ClassGeneratedBy != NULL);
    }
    return false;
}

bool FOdysseyBrushVarActionDetails::IsALocalVariable(UProperty* VariableProperty) const
{
    return VariableProperty && (Cast<UFunction>(VariableProperty->GetOuter()) != NULL);
}

UStruct* FOdysseyBrushVarActionDetails::GetLocalVariableScope(UProperty* VariableProperty) const
{
    if(IsALocalVariable(VariableProperty))
    {
        return Cast<UFunction>(VariableProperty->GetOuter());
    }

    return NULL;
}

bool FOdysseyBrushVarActionDetails::GetVariableNameChangeEnabled() const
{
    bool bIsReadOnly = true;

    UBlueprint* OdysseyBrushObj = GetBlueprintObj();
    check(OdysseyBrushObj != nullptr);

    UProperty* VariableProperty = CachedVariableProperty.Get();
    if(VariableProperty != nullptr && IsVariableInOdysseyBrush())
    {
        if(FBlueprintEditorUtils::FindNewVariableIndex(OdysseyBrushObj, CachedVariableName) != INDEX_NONE)
        {
            bIsReadOnly = false;
        }
        else if(OdysseyBrushObj->FindTimelineTemplateByVariableName(CachedVariableName))
        {
            bIsReadOnly = false;
        }
        else if(IsASCSVariable(VariableProperty) && OdysseyBrushObj->SimpleConstructionScript != nullptr)
        {
            if (USCS_Node* Node = OdysseyBrushObj->SimpleConstructionScript->FindSCSNode(CachedVariableName))
            {
                bIsReadOnly = !FComponentEditorUtils::IsValidVariableNameString(Node->ComponentTemplate, Node->GetVariableName().ToString());
            }
        }
        else if(IsALocalVariable(VariableProperty))
        {
            bIsReadOnly = false;
        }
    }

    return bIsReadOnly;
}

FText FOdysseyBrushVarActionDetails::OnGetVarName() const
{
    return FText::FromName(CachedVariableName);
}

void FOdysseyBrushVarActionDetails::OnVarNameChanged(const FText& InNewText)
{
    bIsVarNameInvalid = true;

    UBlueprint* OdysseyBrushObj = GetBlueprintObj();
    check(OdysseyBrushObj != nullptr);

    UProperty* VariableProperty = CachedVariableProperty.Get();
    if(VariableProperty && IsASCSVariable(VariableProperty) && OdysseyBrushObj->SimpleConstructionScript != nullptr)
    {
        for (USCS_Node* Node : OdysseyBrushObj->SimpleConstructionScript->GetAllNodes())
        {
            if (Node && Node->GetVariableName() == CachedVariableName && !FComponentEditorUtils::IsValidVariableNameString(Node->ComponentTemplate, InNewText.ToString()))
            {
                VarNameEditableTextBox->SetError(LOCTEXT("ComponentVariableRenameFailed_NotValid", "This name is reserved for engine use."));
                return;
            }
        }
    }

    TSharedPtr<INameValidatorInterface> NameValidator = MakeShareable(new FKismetNameValidator(OdysseyBrushObj, CachedVariableName, GetLocalVariableScope(VariableProperty)));

    EValidatorResult ValidatorResult = NameValidator->IsValid(InNewText.ToString());
    if(ValidatorResult == EValidatorResult::AlreadyInUse)
    {
        VarNameEditableTextBox->SetError(FText::Format(LOCTEXT("RenameFailed_InUse", "{0} is in use by another variable or function!"), InNewText));
    }
    else if(ValidatorResult == EValidatorResult::EmptyName)
    {
        VarNameEditableTextBox->SetError(LOCTEXT("RenameFailed_LeftBlank", "Names cannot be left blank!"));
    }
    else if(ValidatorResult == EValidatorResult::TooLong)
    {
        VarNameEditableTextBox->SetError(FText::Format( LOCTEXT("RenameFailed_NameTooLong", "Names must have fewer than {0} characters!"), FText::AsNumber( FKismetNameValidator::GetMaximumNameLength())));
    }
    else if(ValidatorResult == EValidatorResult::LocallyInUse)
    {
        VarNameEditableTextBox->SetError(LOCTEXT("ConflictsWithProperty", "Conflicts with another local variable or function parameter!"));
    }
    else
    {
        bIsVarNameInvalid = false;
        VarNameEditableTextBox->SetError(FText::GetEmpty());
    }
}

void FOdysseyBrushVarActionDetails::OnVarNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit)
{
    if(InTextCommit != ETextCommit::OnCleared && !bIsVarNameInvalid)
    {
        const FScopedTransaction Transaction( LOCTEXT( "RenameVariable", "Rename Variable" ) );

        FName NewVarName = FName(*InNewText.ToString());

        // Double check we're not renaming a timeline disguised as a variable
        bool bIsTimeline = false;

        UProperty* VariableProperty = CachedVariableProperty.Get();
        if (VariableProperty != NULL)
        {
            // Don't allow removal of timeline properties - you need to remove the timeline node for that
            UObjectProperty* ObjProperty = Cast<UObjectProperty>(VariableProperty);
            if(ObjProperty != NULL && ObjProperty->PropertyClass == UTimelineComponent::StaticClass())
            {
                bIsTimeline = true;
            }

            // Rename as a timeline if required
            if (bIsTimeline)
            {
                FBlueprintEditorUtils::RenameTimeline(GetBlueprintObj(), CachedVariableName, NewVarName);
            }
            else if(IsALocalVariable(VariableProperty))
            {
                UFunction* LocalVarScope = Cast<UFunction>(VariableProperty->GetOuter());
                FBlueprintEditorUtils::RenameLocalVariable(GetBlueprintObj(), LocalVarScope, CachedVariableName, NewVarName);
            }
            else
            {
                FBlueprintEditorUtils::RenameMemberVariable(GetBlueprintObj(), CachedVariableName, NewVarName);
            }

            check(MyOdysseyBrush.IsValid());
            MyOdysseyBrush.Pin()->SelectItemByName(NewVarName, ESelectInfo::OnMouseClick);
        }
    }

    bIsVarNameInvalid = false;
    VarNameEditableTextBox->SetError(FText::GetEmpty());
}

bool FOdysseyBrushVarActionDetails::GetVariableTypeChangeEnabled() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if(VariableProperty && IsVariableInOdysseyBrush())
    {
        if (!IsALocalVariable(VariableProperty))
        {
            if(GetBlueprintObj()->SkeletonGeneratedClass->GetAuthoritativeClass() != VariableProperty->GetOwnerClass()->GetAuthoritativeClass())
            {
                return false;
            }
            // If the variable belongs to this class and cannot be found in the member variable list, it is not editable (it may be a component)
            if (FBlueprintEditorUtils::FindNewVariableIndex(GetBlueprintObj(), CachedVariableName) == INDEX_NONE)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool FOdysseyBrushVarActionDetails::GetVariableCategoryChangeEnabled() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if(VariableProperty && IsVariableInOdysseyBrush())
    {
        if(UClass* VarSourceClass = Cast<UClass>(VariableProperty->GetOuter()))
        {
            // If the variable's source class is the same as the current blueprint's class then it was created in this blueprint and it's category can be changed.
            return VarSourceClass == GetBlueprintObj()->SkeletonGeneratedClass;
        }
        else if(IsALocalVariable(VariableProperty))
        {
            return true;
        }
    }

    return false;
}

FEdGraphPinType FOdysseyBrushVarActionDetails::OnGetVarType() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
        FEdGraphPinType Type;
        K2Schema->ConvertPropertyToPinType(VariableProperty, Type);
        return Type;
    }
    return FEdGraphPinType();
}

void FOdysseyBrushVarActionDetails::OnVarTypeChanged(const FEdGraphPinType& NewPinType)
{
    if (FBlueprintEditorUtils::IsPinTypeValid(NewPinType))
    {
        FName VarName = CachedVariableName;

        if (VarName != NAME_None)
        {
            // Set the MyBP tab's last pin type used as this, for adding lots of variables of the same type
            MyOdysseyBrush.Pin()->GetLastPinTypeUsed() = NewPinType;

            UProperty* VariableProperty = CachedVariableProperty.Get();
            if(VariableProperty)
            {
                if(IsALocalVariable(VariableProperty))
                {
                    FBlueprintEditorUtils::ChangeLocalVariableType(GetBlueprintObj(), GetLocalVariableScope(VariableProperty), VarName, NewPinType);
                }
                else
                {
                    FBlueprintEditorUtils::ChangeMemberVariableType(GetBlueprintObj(), VarName, NewPinType);
                }
            }
        }
    }
}

FText FOdysseyBrushVarActionDetails::OnGetTooltipText() const
{
    FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        if ( UBlueprint* OwnerOdysseyBrush = GetPropertyOwnerOdysseyBrush() )
        {
            FString Result;
            FBlueprintEditorUtils::GetBlueprintVariableMetaData(GetPropertyOwnerOdysseyBrush(), VarName, GetLocalVariableScope(CachedVariableProperty.Get()), TEXT("tooltip"), Result);
            return FText::FromString(Result);
        }
    }
    return FText();
}

void FOdysseyBrushVarActionDetails::OnTooltipTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit, FName VarName)
{
    FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), VarName, GetLocalVariableScope(CachedVariableProperty.Get()), TEXT("tooltip"), NewText.ToString() );
}

void FOdysseyBrushVarActionDetails::PopulateCategories(SMyOdysseyBrush* MyOdysseyBrush, TArray<TSharedPtr<FText>>& CategorySource)
{
    // Used to compare found categories to prevent double adds
    TArray<FString> CategoryNameList;

    TArray<FName> VisibleVariables;
    bool bShowUserVarsOnly = MyOdysseyBrush->ShowUserVarsOnly();
    UBlueprint* OdysseyBrush = MyOdysseyBrush->GetBlueprintObj();
    check(OdysseyBrush != NULL);
    if (OdysseyBrush->SkeletonGeneratedClass == NULL)
    {
        UE_LOG(LogBlueprint, Error, TEXT("OdysseyBrush %s has NULL SkeletonGeneratedClass in FOdysseyBrushVarActionDetails::PopulateCategories().  Cannot Populate Categories."), *GetNameSafe(OdysseyBrush));
        return;
    }

    check(OdysseyBrush->SkeletonGeneratedClass != NULL);
    EFieldIteratorFlags::SuperClassFlags SuperClassFlag = EFieldIteratorFlags::ExcludeSuper;
    if(!bShowUserVarsOnly)
    {
        SuperClassFlag = EFieldIteratorFlags::IncludeSuper;
    }

    for (TFieldIterator<UProperty> PropertyIt(OdysseyBrush->SkeletonGeneratedClass, SuperClassFlag); PropertyIt; ++PropertyIt)
    {
        UProperty* Property = *PropertyIt;

        if ((!Property->HasAnyPropertyFlags(CPF_Parm) && Property->HasAllPropertyFlags(CPF_BlueprintVisible)))
        {
            VisibleVariables.Add(Property->GetFName());
        }
    }

    CategorySource.Reset();
    CategorySource.Add(MakeShareable(new FText(LOCTEXT("Default", "Default"))));
    for (int32 i = 0; i < VisibleVariables.Num(); ++i)
    {
        FText Category = FBlueprintEditorUtils::GetBlueprintVariableCategory(OdysseyBrush, VisibleVariables[i], nullptr);
        if (!Category.IsEmpty() && !Category.EqualTo(FText::FromString(OdysseyBrush->GetName())))
        {
            bool bNewCategory = true;
            for (int32 j = 0; j < CategorySource.Num() && bNewCategory; ++j)
            {
                bNewCategory &= !CategorySource[j].Get()->EqualTo(Category);
            }
            if (bNewCategory)
            {
                CategorySource.Add(MakeShareable(new FText(Category)));
            }
        }
    }

    // Search through all function graphs for entry nodes to search for local variables to pull their categories
    for (UEdGraph* FunctionGraph : OdysseyBrush->FunctionGraphs)
    {
        if(UFunction* Function = OdysseyBrush->SkeletonGeneratedClass->FindFunctionByName(FunctionGraph->GetFName()))
        {
            FText FunctionCategory = Function->GetMetaDataText(FBlueprintMetadata::MD_FunctionCategory, TEXT("UObjectCategory"), Function->GetFullGroupName(false));

            if(!FunctionCategory.IsEmpty())
            {
                bool bNewCategory = true;
                for (int32 j = 0; j < CategorySource.Num() && bNewCategory; ++j)
                {
                    bNewCategory &= !CategorySource[j].Get()->EqualTo(FunctionCategory);
                }

                if(bNewCategory)
                {
                    CategorySource.Add(MakeShareable(new FText(FunctionCategory)));
                }
            }
        }

        UK2Node_EditablePinBase* EntryNode = FBlueprintEditorUtils::GetEntryNode(FunctionGraph);
        if (UK2Node_FunctionEntry* FunctionEntryNode = Cast<UK2Node_FunctionEntry>(EntryNode))
        {
            for (FBPVariableDescription& Variable : FunctionEntryNode->LocalVariables)
            {
                bool bNewCategory = true;
                for (int32 j = 0; j < CategorySource.Num() && bNewCategory; ++j)
                {
                    bNewCategory &= !CategorySource[j].Get()->EqualTo(Variable.Category);
                }
                if (bNewCategory)
                {
                    CategorySource.Add(MakeShareable(new FText(Variable.Category)));
                }
            }
        }
    }

    for (UEdGraph* MacroGraph : OdysseyBrush->MacroGraphs)
    {
        UK2Node_EditablePinBase* EntryNode = FBlueprintEditorUtils::GetEntryNode(MacroGraph);
        if (UK2Node_Tunnel* TypedEntryNode = ExactCast<UK2Node_Tunnel>(EntryNode))
        {
            if (!TypedEntryNode->MetaData.Category.IsEmpty())
            {
                bool bNewCategory = true;
                for (int32 j = 0; j < CategorySource.Num() && bNewCategory; ++j)
                {
                    bNewCategory &= !CategorySource[j].Get()->EqualTo(TypedEntryNode->MetaData.Category);
                }
                if (bNewCategory)
                {
                    CategorySource.Add(MakeShareable(new FText(TypedEntryNode->MetaData.Category)));
                }
            }
        }
    }

    // Pull categories from overridable functions
    for (TFieldIterator<UFunction> FunctionIt(OdysseyBrush->ParentClass, EFieldIteratorFlags::IncludeSuper); FunctionIt; ++FunctionIt)
    {
        const UFunction* Function = *FunctionIt;
        const FName FunctionName = Function->GetFName();

        if (UEdGraphSchema_OdysseyBrush::CanKismetOverrideFunction(Function) && !UEdGraphSchema_OdysseyBrush::FunctionCanBePlacedAsEvent(Function))
        {
            FText FunctionCategory = Function->GetMetaDataText(FBlueprintMetadata::MD_FunctionCategory, TEXT("UObjectCategory"), Function->GetFullGroupName(false));

            if (!FunctionCategory.IsEmpty())
            {
                bool bNewCategory = true;
                for (int32 j = 0; j < CategorySource.Num() && bNewCategory; ++j)
                {
                    bNewCategory &= !CategorySource[j].Get()->EqualTo(FunctionCategory);
                }

                if (bNewCategory)
                {
                    CategorySource.Add(MakeShareable(new FText(FunctionCategory)));
                }
            }
        }
    }
}

UProperty* FOdysseyBrushVarActionDetails::CustomizedObjectAsProperty() const
{
    if(ObjectsBeingEdited.Num() == 1)
    {
        return Cast<UProperty>(ObjectsBeingEdited[0].Get());
    }

    return nullptr;
}

UK2Node_Variable* FOdysseyBrushVarActionDetails::EdGraphSelectionAsVar() const
{
    TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditor = MyOdysseyBrush.Pin()->GetBlueprintEditor();

    if( OdysseyBrushEditor.IsValid() )
    {
        /** Get the currently selected set of nodes */
        TSet<UObject*> Objects = OdysseyBrushEditor.Pin()->GetSelectedNodes();

        if (Objects.Num() == 1)
        {
            TSet<UObject*>::TIterator Iter(Objects);
            UObject* Object = *Iter;

            if (Object && Object->IsA<UK2Node_Variable>())
            {
                return Cast<UK2Node_Variable>(Object);
            }
        }
    }
    return NULL;
}

UProperty* FOdysseyBrushVarActionDetails::SelectionAsProperty() const
{
    FEdGraphSchemaAction_K2Var* VarAction = MyOdysseyBrushSelectionAsVar();
    if(VarAction)
    {
        return VarAction->GetProperty();
    }
    FEdGraphSchemaAction_K2LocalVar* LocalVarAction = MyOdysseyBrushSelectionAsLocalVar();
    if(LocalVarAction)
    {
        return LocalVarAction->GetProperty();
    }
    UK2Node_Variable* GraphVar = EdGraphSelectionAsVar();
    if(GraphVar)
    {
        return GraphVar->GetPropertyForVariable();
    }
    UProperty* Property = CustomizedObjectAsProperty();
    if(Property)
    {
        return Property;
    }
    return NULL;
}

FName FOdysseyBrushVarActionDetails::GetVariableName() const
{
    FEdGraphSchemaAction_K2Var* VarAction = MyOdysseyBrushSelectionAsVar();
    if(VarAction)
    {
        return VarAction->GetVariableName();
    }
    FEdGraphSchemaAction_K2LocalVar* LocalVarAction = MyOdysseyBrushSelectionAsLocalVar();
    if(LocalVarAction)
    {
        return LocalVarAction->GetVariableName();
    }
    UK2Node_Variable* GraphVar = EdGraphSelectionAsVar();
    if(GraphVar)
    {
        return GraphVar->GetVarName();
    }
    UProperty* Property = CustomizedObjectAsProperty();
    if(Property)
    {
        return Property->GetFName();
    }
    return NAME_None;
}

FText FOdysseyBrushVarActionDetails::OnGetCategoryText() const
{
    FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        if ( UBlueprint* OwnerOdysseyBrush = GetPropertyOwnerOdysseyBrush() )
        {
            FText Category = FBlueprintEditorUtils::GetBlueprintVariableCategory(OwnerOdysseyBrush, VarName, GetLocalVariableScope(CachedVariableProperty.Get()));

            // Older blueprints will have their name as the default category and whenever it is the same as the default category, display localized text
            if ( Category.EqualTo(FText::FromString(OwnerOdysseyBrush->GetName())) || Category.EqualTo(UEdGraphSchema_OdysseyBrush::VR_DefaultCategory) )
            {
                return UEdGraphSchema_OdysseyBrush::VR_DefaultCategory;
            }
            else
            {
                return Category;
            }
        }

        return FText::FromName(VarName);
    }
    return FText();
}

void FOdysseyBrushVarActionDetails::OnCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit, FName VarName)
{
    if (InTextCommit == ETextCommit::OnEnter || InTextCommit == ETextCommit::OnUserMovedFocus)
    {
        // Remove excess whitespace and prevent categories with just spaces
        FText CategoryName = FText::TrimPrecedingAndTrailing(NewText);

        FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), VarName, GetLocalVariableScope(CachedVariableProperty.Get()), CategoryName);
        check(MyOdysseyBrush.IsValid());
        PopulateCategories(MyOdysseyBrush.Pin().Get(), CategorySource);
        MyOdysseyBrush.Pin()->ExpandCategory(CategoryName);
    }
}

TSharedRef< ITableRow > FOdysseyBrushVarActionDetails::MakeCategoryViewWidget( TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable )
{
    return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
        [
            SNew(STextBlock) .Text(*Item.Get())
        ];
}

void FOdysseyBrushVarActionDetails::OnCategorySelectionChanged( TSharedPtr<FText> ProposedSelection, ESelectInfo::Type /*SelectInfo*/ )
{
    FName VarName = CachedVariableName;
    if (ProposedSelection.IsValid() && VarName != NAME_None)
    {
        FText NewCategory = *ProposedSelection.Get();

        FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), VarName, GetLocalVariableScope(CachedVariableProperty.Get()), NewCategory );
        CategoryListView.Pin()->ClearSelection();
        CategoryComboButton.Pin()->SetIsOpen(false);
        MyOdysseyBrush.Pin()->ExpandCategory(NewCategory);
    }
}

EVisibility FOdysseyBrushVarActionDetails::ShowEditableCheckboxVisibilty() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty && GetPropertyOwnerOdysseyBrush())
    {
        if (IsAOdysseyBrushVariable(VariableProperty) && IsAUserVariable(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnEditableCheckboxState() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        return VariableProperty->HasAnyPropertyFlags(CPF_DisableEditOnInstance) ? ECheckBoxState::Unchecked : ECheckBoxState::Checked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnEditableChanged(ECheckBoxState InNewState)
{
    FName VarName = CachedVariableName;

    // Toggle the flag on the blueprint's version of the variable description, based on state
    const bool bVariableIsExposed = InNewState == ECheckBoxState::Checked;

    UBlueprint* OdysseyBrushObj = MyOdysseyBrush.Pin()->GetBlueprintObj();
    FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(OdysseyBrushObj, VarName, !bVariableIsExposed);
}

EVisibility FOdysseyBrushVarActionDetails::ShowReadOnlyCheckboxVisibilty() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty && GetPropertyOwnerOdysseyBrush())
    {
        if (IsAOdysseyBrushVariable(VariableProperty) && IsAUserVariable(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnReadyOnlyCheckboxState() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        return VariableProperty->HasAnyPropertyFlags(CPF_BlueprintReadOnly) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnReadyOnlyChanged(ECheckBoxState InNewState)
{
    FName VarName = CachedVariableName;

    // Toggle the flag on the blueprint's version of the variable description, based on state
    const bool bVariableIsReadOnly = InNewState == ECheckBoxState::Checked;

    UBlueprint* OdysseyBrushObj = MyOdysseyBrush.Pin()->GetBlueprintObj();
    FBlueprintEditorUtils::SetBlueprintPropertyReadOnlyFlag(OdysseyBrushObj, VarName, bVariableIsReadOnly);
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnCreateWidgetCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        bool bMakingWidget = FEdMode::ShouldCreateWidgetForProperty(Property);

        return bMakingWidget ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnCreateWidgetChanged(ECheckBoxState InNewState)
{
    const FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        if (InNewState == ECheckBoxState::Checked)
        {
            FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), VarName, GetLocalVariableScope(CachedVariableProperty.Get()), FEdMode::MD_MakeEditWidget, TEXT("true"));
        }
        else
        {
            FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(GetBlueprintObj(), VarName, GetLocalVariableScope(CachedVariableProperty.Get()), FEdMode::MD_MakeEditWidget);
        }
    }
}

EVisibility FOdysseyBrushVarActionDetails::Show3DWidgetVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty && GetPropertyOwnerOdysseyBrush())
    {
        if (IsAOdysseyBrushVariable(VariableProperty) && FEdMode::CanCreateWidgetForProperty(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

bool FOdysseyBrushVarActionDetails::Is3DWidgetEnabled()
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        return ( VariableProperty && !VariableProperty->HasAnyPropertyFlags(CPF_DisableEditOnInstance) ) ;
    }
    return false;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetExposedToSpawnCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return (Property && Property->GetBoolMetaData(FBlueprintMetadata::MD_ExposeOnSpawn) != false) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnExposedToSpawnChanged(ECheckBoxState InNewState)
{
    const FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        const bool bExposeOnSpawn = (InNewState == ECheckBoxState::Checked);
        if(bExposeOnSpawn)
        {
            FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), VarName, NULL, FBlueprintMetadata::MD_ExposeOnSpawn, TEXT("true"));
        }
        else
        {
            FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(GetBlueprintObj(), VarName, NULL, FBlueprintMetadata::MD_ExposeOnSpawn);
        }
    }
}

EVisibility FOdysseyBrushVarActionDetails::ExposeOnSpawnVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty && GetPropertyOwnerOdysseyBrush())
    {
        const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
        FEdGraphPinType VariablePinType;
        K2Schema->ConvertPropertyToPinType(VariableProperty, VariablePinType);

        const bool bShowPrivacySetting = IsAOdysseyBrushVariable(VariableProperty) && IsAUserVariable(VariableProperty);
        if (bShowPrivacySetting && (K2Schema->FindSetVariableByNameFunction(VariablePinType) != NULL))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetPrivateCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return (Property && Property->GetBoolMetaData(FBlueprintMetadata::MD_Private) != false) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnPrivateChanged(ECheckBoxState InNewState)
{
    const FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        const bool bExposeOnSpawn = (InNewState == ECheckBoxState::Checked);
        if(bExposeOnSpawn)
        {
            FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), VarName, NULL, FBlueprintMetadata::MD_Private, TEXT("true"));
        }
        else
        {
            FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(GetBlueprintObj(), VarName, NULL, FBlueprintMetadata::MD_Private);
        }
    }
}

EVisibility FOdysseyBrushVarActionDetails::ExposePrivateVisibility() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property && GetPropertyOwnerOdysseyBrush())
    {
        if (IsAOdysseyBrushVariable(Property) && IsAUserVariable(Property))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetExposedToCinematicsCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return Property && Property->HasAnyPropertyFlags(CPF_Interp) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnExposedToCinematicsChanged(ECheckBoxState InNewState)
{
    // Toggle the flag on the blueprint's version of the variable description, based on state
    const bool bExposeToCinematics = (InNewState == ECheckBoxState::Checked);

    const FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        FBlueprintEditorUtils::SetInterpFlag(GetBlueprintObj(), VarName, bExposeToCinematics);
    }
}

EVisibility FOdysseyBrushVarActionDetails::ExposeToCinematicsVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty && !IsALocalVariable(VariableProperty))
    {
        const bool bIsInteger = VariableProperty->IsA(UIntProperty::StaticClass());
        const bool bIsByte = VariableProperty->IsA(UByteProperty::StaticClass());
        const bool bIsEnum = VariableProperty->IsA(UEnumProperty::StaticClass());
        const bool bIsFloat = VariableProperty->IsA(UFloatProperty::StaticClass());
        const bool bIsBool = VariableProperty->IsA(UBoolProperty::StaticClass());
        const bool bIsStr = VariableProperty->IsA(UStrProperty::StaticClass());
        const bool bIsVectorStruct = VariableProperty->IsA(UStructProperty::StaticClass()) && Cast<UStructProperty>(VariableProperty)->Struct->GetFName() == NAME_Vector;
        const bool bIsTransformStruct = VariableProperty->IsA(UStructProperty::StaticClass()) && Cast<UStructProperty>(VariableProperty)->Struct->GetFName() == NAME_Transform;
        const bool bIsColorStruct = VariableProperty->IsA(UStructProperty::StaticClass()) && Cast<UStructProperty>(VariableProperty)->Struct->GetFName() == NAME_Color;
        const bool bIsLinearColorStruct = VariableProperty->IsA(UStructProperty::StaticClass()) && Cast<UStructProperty>(VariableProperty)->Struct->GetFName() == NAME_LinearColor;
        const bool bIsActorProperty = VariableProperty->IsA(UObjectProperty::StaticClass()) && Cast<UObjectProperty>(VariableProperty)->PropertyClass->IsChildOf(AActor::StaticClass());

        if (bIsInteger || bIsByte || bIsEnum || bIsFloat || bIsBool || bIsStr || bIsVectorStruct || bIsTransformStruct || bIsColorStruct || bIsLinearColorStruct || bIsActorProperty)
        {
            return EVisibility::Visible;
        }
        else
        {
            ISequencerModule* SequencerModule = FModuleManager::Get().GetModulePtr<ISequencerModule>("Sequencer");
            //ODYSSEY: PATCH
            //if (SequencerModule->CanAnimateProperty(FAnimatedPropertyKey::FromProperty(VariableProperty)))
            if (SequencerModule->CanAnimateProperty(VariableProperty))
            {
                return EVisibility::Visible;
            }
            else if (UObjectProperty* ObjectProperty = Cast<UObjectProperty>(VariableProperty))
            {
                UClass* ClassType = ObjectProperty->PropertyClass ? ObjectProperty->PropertyClass->GetSuperClass() : nullptr;

                //ODYSSEY: PATCH
                /*
                while (ClassType)
                {
                    if (SequencerModule->CanAnimateProperty(FAnimatedPropertyKey::FromObjectType(ClassType)))
                    {
                        return EVisibility::Visible;
                    }
                    ClassType = ClassType->GetSuperClass();
                }
                */
                return  EVisibility::Hidden;
            }
        }
    }
    return EVisibility::Collapsed;
}

TSharedPtr<FString> FOdysseyBrushVarActionDetails::GetVariableReplicationCondition() const
{
    ELifetimeCondition VariableRepCondition = COND_None;

    const UProperty* const Property = CachedVariableProperty.Get();

    if (Property)
    {
        VariableRepCondition = Property->GetBlueprintReplicationCondition();
    }

    return ReplicationConditionEnumTypeNames[(uint8)VariableRepCondition];
}

void FOdysseyBrushVarActionDetails::OnChangeReplicationCondition(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
    int32 NewSelection;
    const bool bFound = ReplicationConditionEnumTypeNames.Find(ItemSelected, NewSelection);
    check(bFound && NewSelection != INDEX_NONE);

    const ELifetimeCondition NewRepCondition = (ELifetimeCondition)NewSelection;

    UBlueprint* const OdysseyBrushObj = GetBlueprintObj();
    const FName VarName = CachedVariableName;

    if (OdysseyBrushObj && VarName != NAME_None)
    {
        const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(OdysseyBrushObj, VarName);

        if (VarIndex != INDEX_NONE)
        {
            OdysseyBrushObj->NewVariables[VarIndex].ReplicationCondition = NewRepCondition;

            FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(OdysseyBrushObj);
        }
    }

}

bool FOdysseyBrushVarActionDetails::ReplicationConditionEnabled() const
{
    const UProperty* const VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        const uint64 *PropFlagPtr = FBlueprintEditorUtils::GetBlueprintVariablePropertyFlags(GetBlueprintObj(), VariableProperty->GetFName());
        uint64 PropFlags = 0;

        if (PropFlagPtr != nullptr)
        {
            PropFlags = *PropFlagPtr;
            return (PropFlags & CPF_Net) > 0;

        }
    }

    return false;
}

bool FOdysseyBrushVarActionDetails::ReplicationEnabled() const
{
    // Update FOdysseyBrushVarActionDetails::ReplicationTooltip if you alter this function
    // shat users can understand why replication settins are disabled!
    bool bVariableCanBeReplicated = true;
    const UProperty* const VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        // sets and maps cannot yet be replicated:
        bVariableCanBeReplicated = Cast<USetProperty>(VariableProperty) == nullptr && Cast<UMapProperty>(VariableProperty) == nullptr;
    }
    return bVariableCanBeReplicated && IsVariableInOdysseyBrush();
}

FText FOdysseyBrushVarActionDetails::ReplicationTooltip() const
{
    if(ReplicationEnabled())
    {
        return LOCTEXT("VariableReplicate_Tooltip", "Should this Variable be replicated over the network?");
    }
    else
    {
        return LOCTEXT("VariableReplicateDisabled_Tooltip", "Set and Map properties cannot be replicated");
    }
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetConfigVariableCheckboxState() const
{
    UBlueprint* OdysseyBrushObj = GetPropertyOwnerOdysseyBrush();
    const FName VarName = CachedVariableName;
    ECheckBoxState CheckboxValue = ECheckBoxState::Unchecked;

    if( OdysseyBrushObj && VarName != NAME_None )
    {
        const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex( OdysseyBrushObj, VarName );

        if( VarIndex != INDEX_NONE && OdysseyBrushObj->NewVariables[ VarIndex ].PropertyFlags & CPF_Config )
        {
            CheckboxValue = ECheckBoxState::Checked;
        }
    }
    return CheckboxValue;
}

void FOdysseyBrushVarActionDetails::OnSetConfigVariableState( ECheckBoxState InNewState )
{
    UBlueprint* OdysseyBrushObj = GetBlueprintObj();
    const FName VarName = CachedVariableName;

    if( OdysseyBrushObj && VarName != NAME_None )
    {
        const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex( OdysseyBrushObj, VarName );

        if( VarIndex != INDEX_NONE )
        {
            if( InNewState == ECheckBoxState::Checked )
            {
                OdysseyBrushObj->NewVariables[ VarIndex ].PropertyFlags |= CPF_Config;
            }
            else
            {
                OdysseyBrushObj->NewVariables[ VarIndex ].PropertyFlags &= ~CPF_Config;
            }
            FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified( OdysseyBrushObj );
        }
    }
}

EVisibility FOdysseyBrushVarActionDetails::ExposeConfigVisibility() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        if (IsAOdysseyBrushVariable(Property) && IsAUserVariable(Property))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

bool FOdysseyBrushVarActionDetails::IsConfigCheckBoxEnabled() const
{
    bool bEnabled = IsVariableInOdysseyBrush();
    if (bEnabled && CachedVariableProperty.IsValid())
    {
        if (UProperty* VariableProperty = CachedVariableProperty.Get())
        {
            // meant to match up with UHT's FPropertyBase::IsObject(), which it uses to block object properties from being marked with CPF_Config
            bEnabled = VariableProperty->IsA<UClassProperty>() || VariableProperty->IsA<USoftClassProperty>() || VariableProperty->IsA<USoftObjectProperty>() ||
                (!VariableProperty->IsA<UObjectPropertyBase>() && !VariableProperty->IsA<UInterfaceProperty>());
        }
    }
    return bEnabled;
}

FText FOdysseyBrushVarActionDetails::OnGetMetaKeyValue(FName Key) const
{
    FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        if ( UBlueprint* OdysseyBrushObj = GetPropertyOwnerOdysseyBrush() )
        {
            FString Result;
            FBlueprintEditorUtils::GetBlueprintVariableMetaData(OdysseyBrushObj, VarName, GetLocalVariableScope(CachedVariableProperty.Get()), Key, /*out*/ Result);

            return FText::FromString(Result);
        }
    }
    return FText();
}

void FOdysseyBrushVarActionDetails::OnMetaKeyValueChanged(const FText& NewMinValue, ETextCommit::Type CommitInfo, FName Key)
{
    FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        if ((CommitInfo == ETextCommit::OnEnter) || (CommitInfo == ETextCommit::OnUserMovedFocus))
        {
            FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), VarName, GetLocalVariableScope(CachedVariableProperty.Get()), Key, NewMinValue.ToString());
        }
    }
}

EVisibility FOdysseyBrushVarActionDetails::RangeVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        const bool bIsInteger = VariableProperty->IsA(UIntProperty::StaticClass());
        const bool bIsNonEnumByte = (VariableProperty->IsA(UByteProperty::StaticClass()) && Cast<const UByteProperty>(VariableProperty)->Enum == NULL);
        const bool bIsFloat = VariableProperty->IsA(UFloatProperty::StaticClass());

        if (IsAOdysseyBrushVariable(VariableProperty) && (bIsInteger || bIsNonEnumByte || bIsFloat))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

EVisibility FOdysseyBrushVarActionDetails::BitmaskVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty && IsAOdysseyBrushVariable(VariableProperty) && VariableProperty->IsA(UIntProperty::StaticClass()))
    {
        return EVisibility::Visible;
    }

    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnBitmaskCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return (Property && Property->HasMetaData(FBlueprintMetadata::MD_Bitmask)) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnBitmaskChanged(ECheckBoxState InNewState)
{
    const FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        UBlueprint* LocalOdysseyBrush = GetBlueprintObj();

        const bool bIsBitmask = (InNewState == ECheckBoxState::Checked);
        if (bIsBitmask)
        {
            FBlueprintEditorUtils::SetBlueprintVariableMetaData(LocalOdysseyBrush, VarName, nullptr, FBlueprintMetadata::MD_Bitmask, TEXT(""));
        }
        else
        {
            FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(LocalOdysseyBrush, VarName, nullptr, FBlueprintMetadata::MD_Bitmask);
        }

        // Reset default value
        if (LocalOdysseyBrush->GeneratedClass)
        {
            UObject* CDO = LocalOdysseyBrush->GeneratedClass->GetDefaultObject(false);
            UProperty* VarProperty = FindField<UProperty>(LocalOdysseyBrush->GeneratedClass, VarName);

            if (CDO != nullptr && VarProperty != nullptr)
            {
                VarProperty->InitializeValue_InContainer(CDO);
            }
        }

        TArray<UK2Node_Variable*> VariableNodes;
        FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_Variable>(GetBlueprintObj(), VariableNodes);

        for (TArray<UK2Node_Variable*>::TConstIterator NodeIt(VariableNodes); NodeIt; ++NodeIt)
        {
            UK2Node_Variable* CurrentNode = *NodeIt;
            if (VarName == CurrentNode->GetVarName())
            {
                CurrentNode->ReconstructNode();
            }
        }
    }
}

TSharedPtr<FString> FOdysseyBrushVarActionDetails::GetBitmaskEnumTypeName() const
{
    TSharedPtr<FString> Result;
    const FName VarName = CachedVariableName;

    if (BitmaskEnumTypeNames.Num() > 0 && VarName != NAME_None)
    {
        Result = BitmaskEnumTypeNames[0];

        FString OutValue;
        FBlueprintEditorUtils::GetBlueprintVariableMetaData(GetBlueprintObj(), VarName, nullptr, FBlueprintMetadata::MD_BitmaskEnum, OutValue);

        for (int32 i = 1; i < BitmaskEnumTypeNames.Num(); ++i)
        {
            if (OutValue == *BitmaskEnumTypeNames[i])
            {
                Result = BitmaskEnumTypeNames[i];
                break;
            }
        }
    }

    return Result;
}

void FOdysseyBrushVarActionDetails::OnBitmaskEnumTypeChanged(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
    const FName VarName = CachedVariableName;
    if (VarName != NAME_None)
    {
        UBlueprint* LocalOdysseyBrush = GetBlueprintObj();

        if (ItemSelected == BitmaskEnumTypeNames[0])
        {
            FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(LocalOdysseyBrush, VarName, nullptr, FBlueprintMetadata::MD_BitmaskEnum);
        }
        else if(ItemSelected.IsValid())
        {
            FBlueprintEditorUtils::SetBlueprintVariableMetaData(LocalOdysseyBrush, VarName, nullptr, FBlueprintMetadata::MD_BitmaskEnum, *ItemSelected);
        }

        // Reset default value
        if (LocalOdysseyBrush->GeneratedClass)
        {
            UObject* CDO = LocalOdysseyBrush->GeneratedClass->GetDefaultObject(false);
            UProperty* VarProperty = FindField<UProperty>(LocalOdysseyBrush->GeneratedClass, VarName);

            if (CDO != nullptr && VarProperty != nullptr)
            {
                VarProperty->InitializeValue_InContainer(CDO);
            }
        }

        TArray<UK2Node_Variable*> VariableNodes;
        FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_Variable>(GetBlueprintObj(), VariableNodes);

        for (TArray<UK2Node_Variable*>::TConstIterator NodeIt(VariableNodes); NodeIt; ++NodeIt)
        {
            UK2Node_Variable* CurrentNode = *NodeIt;
            if (VarName == CurrentNode->GetVarName())
            {
                CurrentNode->ReconstructNode();
            }
        }
    }
}

TSharedPtr<FString> FOdysseyBrushVarActionDetails::GetVariableReplicationType() const
{
    EVariableReplication::Type VariableReplication = EVariableReplication::None;

    uint64 PropFlags = 0;
    UProperty* VariableProperty = CachedVariableProperty.Get();

    if (VariableProperty && (IsVariableInOdysseyBrush() || IsVariableInheritedByOdysseyBrush()))
    {
        UBlueprint* OdysseyBrushObj = GetPropertyOwnerOdysseyBrush();
        if (OdysseyBrushObj != nullptr)
        {
            uint64 *PropFlagPtr = FBlueprintEditorUtils::GetBlueprintVariablePropertyFlags(OdysseyBrushObj, VariableProperty->GetFName());

            if (PropFlagPtr != NULL)
            {
                PropFlags = *PropFlagPtr;
                bool IsReplicated = (PropFlags & CPF_Net) > 0;
                bool bHasRepNotify = FBlueprintEditorUtils::GetBlueprintVariableRepNotifyFunc(OdysseyBrushObj, VariableProperty->GetFName()) != NAME_None;
                if (bHasRepNotify)
                {
                    // Verify they actually have a valid rep notify function still
                    UClass* GenClass = GetPropertyOwnerOdysseyBrush()->SkeletonGeneratedClass;
                    UFunction* OnRepFunc = GenClass->FindFunctionByName(FBlueprintEditorUtils::GetBlueprintVariableRepNotifyFunc(OdysseyBrushObj, VariableProperty->GetFName()));
                    if (OnRepFunc == NULL || OnRepFunc->NumParms != 0 || OnRepFunc->GetReturnProperty() != NULL)
                    {
                        bHasRepNotify = false;
                        ReplicationOnRepFuncChanged(FName(NAME_None).ToString());
                    }
                }

                VariableReplication = !IsReplicated ? EVariableReplication::None :
                    bHasRepNotify ? EVariableReplication::RepNotify : EVariableReplication::Replicated;
            }
        }
    }

    return ReplicationOptions[(int32)VariableReplication];
}

void FOdysseyBrushVarActionDetails::OnChangeReplication(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
    int32 NewSelection;
    bool bFound = ReplicationOptions.Find(ItemSelected, NewSelection);
    check(bFound && NewSelection != INDEX_NONE);

    EVariableReplication::Type VariableReplication = (EVariableReplication::Type)NewSelection;

    UProperty* VariableProperty = CachedVariableProperty.Get();

    UBlueprint* const OdysseyBrushObj = GetBlueprintObj();
    const FName VarName = CachedVariableName;
    int32 VarIndex = INDEX_NONE;
    if (OdysseyBrushObj && VarName != NAME_None)
    {
        VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(OdysseyBrushObj, VarName);
    }

    if (VariableProperty)
    {
        uint64 *PropFlagPtr = FBlueprintEditorUtils::GetBlueprintVariablePropertyFlags(GetBlueprintObj(), VariableProperty->GetFName());
        if (PropFlagPtr != NULL)
        {
            switch(VariableReplication)
            {
            case EVariableReplication::None:
                *PropFlagPtr &= ~CPF_Net;
                ReplicationOnRepFuncChanged(FName(NAME_None).ToString());

                //set replication condition to none:
                if (VarIndex != INDEX_NONE)
                {
                    OdysseyBrushObj->NewVariables[VarIndex].ReplicationCondition = COND_None;
                }

                break;

            case EVariableReplication::Replicated:
                *PropFlagPtr |= CPF_Net;
                ReplicationOnRepFuncChanged(FName(NAME_None).ToString());
                break;

            case EVariableReplication::RepNotify:
                *PropFlagPtr |= CPF_Net;
                FString NewFuncName = FString::Printf(TEXT("OnRep_%s"), *VariableProperty->GetName());
                UEdGraph* FuncGraph = FindObject<UEdGraph>(OdysseyBrushObj, *NewFuncName);
                if (!FuncGraph)
                {
                    FuncGraph = FBlueprintEditorUtils::CreateNewGraph(OdysseyBrushObj, FName(*NewFuncName), UEdGraph::StaticClass(), UEdGraphSchema_OdysseyBrush::StaticClass());
                    FBlueprintEditorUtils::AddFunctionGraph<UClass>(OdysseyBrushObj, FuncGraph, false, NULL);
                }

                if (FuncGraph)
                {
                    ReplicationOnRepFuncChanged(NewFuncName);
                }
                break;
            }

            FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(OdysseyBrushObj);
        }
    }
}

void FOdysseyBrushVarActionDetails::ReplicationOnRepFuncChanged(const FString& NewOnRepFunc) const
{
    FName NewFuncName = FName(*NewOnRepFunc);
    UProperty* VariableProperty = CachedVariableProperty.Get();

    if (VariableProperty)
    {
        FBlueprintEditorUtils::SetBlueprintVariableRepNotifyFunc(GetBlueprintObj(), VariableProperty->GetFName(), NewFuncName);
        uint64 *PropFlagPtr = FBlueprintEditorUtils::GetBlueprintVariablePropertyFlags(GetBlueprintObj(), VariableProperty->GetFName());
        if (PropFlagPtr != NULL)
        {
            if (NewFuncName != NAME_None)
            {
                *PropFlagPtr |= CPF_RepNotify;
                *PropFlagPtr |= CPF_Net;
            }
            else
            {
                *PropFlagPtr &= ~CPF_RepNotify;
            }
        }
    }
}

EVisibility FOdysseyBrushVarActionDetails::ReplicationVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if(VariableProperty)
    {
        if (IsAUserVariable(VariableProperty) && IsAOdysseyBrushVariable(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

TSharedRef<SWidget> FOdysseyBrushVarActionDetails::BuildEventsMenuForVariable() const
{
    if( MyOdysseyBrush.IsValid() )
    {
        TSharedPtr<SMyOdysseyBrush> MyOdysseyBrushPtr = MyOdysseyBrush.Pin();
        FEdGraphSchemaAction_K2Var* Variable = MyOdysseyBrushPtr->SelectionAsVar();
        UObjectProperty* ComponentProperty = Variable ? Cast<UObjectProperty>(Variable->GetProperty()) : NULL;
        TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = MyOdysseyBrushPtr->GetBlueprintEditor();
        if( OdysseyBrushEditorPtr.IsValid() && ComponentProperty )
        {
            TSharedPtr<SSCSEditor> Editor =  OdysseyBrushEditorPtr.Pin()->GetSCSEditor();
            FMenuBuilder MenuBuilder( true, NULL );
            Editor->BuildMenuEventsSection( MenuBuilder, OdysseyBrushEditorPtr.Pin()->GetBlueprintObj(), ComponentProperty->PropertyClass,
                                            FCanExecuteAction::CreateSP(OdysseyBrushEditorPtr.Pin().Get(), &FOdysseyBrushEditor::InEditingMode),
                                            FOdysseyGetSelectedObjectsDelegate::CreateSP(MyOdysseyBrushPtr.Get(), &SMyOdysseyBrush::GetSelectedItemsForContextMenu));
            return MenuBuilder.MakeWidget();
        }
    }
    return SNullWidget::NullWidget;
}

void FOdysseyBrushVarActionDetails::OnPostEditorRefresh()
{
    CachedVariableProperty = SelectionAsProperty();
    CachedVariableName = GetVariableName();
}

EVisibility FOdysseyBrushVarActionDetails::GetTransientVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        if (IsAOdysseyBrushVariable(VariableProperty) && IsAUserVariable(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetTransientCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return (Property && Property->HasAnyPropertyFlags(CPF_Transient)) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnTransientChanged(ECheckBoxState InNewState)
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        const bool bTransientFlag = (InNewState == ECheckBoxState::Checked);
        FBlueprintEditorUtils::SetVariableTransientFlag(GetBlueprintObj(), Property->GetFName(), bTransientFlag);
    }
}

EVisibility FOdysseyBrushVarActionDetails::GetSaveGameVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        if (IsAOdysseyBrushVariable(VariableProperty) && IsAUserVariable(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetSaveGameCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return (Property && Property->HasAnyPropertyFlags(CPF_SaveGame)) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnSaveGameChanged(ECheckBoxState InNewState)
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        const bool bSaveGameFlag = (InNewState == ECheckBoxState::Checked);
        FBlueprintEditorUtils::SetVariableSaveGameFlag(GetBlueprintObj(), Property->GetFName(), bSaveGameFlag);
    }
}

EVisibility FOdysseyBrushVarActionDetails::GetAdvancedDisplayVisibility() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        if (IsAOdysseyBrushVariable(VariableProperty) && IsAUserVariable(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetAdvancedDisplayCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return (Property && Property->HasAnyPropertyFlags(CPF_AdvancedDisplay)) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnAdvancedDisplayChanged(ECheckBoxState InNewState)
{
    if (UProperty* Property = CachedVariableProperty.Get())
    {
        const bool bAdvancedFlag = (InNewState == ECheckBoxState::Checked);
        FBlueprintEditorUtils::SetVariableAdvancedDisplayFlag(GetBlueprintObj(), Property->GetFName(), bAdvancedFlag);
    }
}

EVisibility FOdysseyBrushVarActionDetails::GetMultilineVisibility() const
{
    if (UProperty* VariableProperty = CachedVariableProperty.Get())
    {
        if (IsAOdysseyBrushVariable(VariableProperty))
        {
            if (VariableProperty->IsA(UTextProperty::StaticClass()) || VariableProperty->IsA(UStrProperty::StaticClass()))
            {
                return EVisibility::Visible;
            }
        }
    }
    return EVisibility::Collapsed;
}

ECheckBoxState FOdysseyBrushVarActionDetails::OnGetMultilineCheckboxState() const
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        return (Property && Property->GetBoolMetaData(TEXT("MultiLine"))) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

void FOdysseyBrushVarActionDetails::OnMultilineChanged(ECheckBoxState InNewState)
{
    UProperty* Property = CachedVariableProperty.Get();
    if (Property)
    {
        const bool bMultiline = (InNewState == ECheckBoxState::Checked);
        FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), Property->GetFName(), GetLocalVariableScope(CachedVariableProperty.Get()), TEXT("MultiLine"), bMultiline ? TEXT("true") : TEXT("false"));
    }
}

EVisibility FOdysseyBrushVarActionDetails::IsTooltipEditVisible() const
{
    UProperty* VariableProperty = CachedVariableProperty.Get();
    if (VariableProperty)
    {
        if ((IsAOdysseyBrushVariable(VariableProperty) && IsAUserVariable(VariableProperty)) || IsALocalVariable(VariableProperty))
        {
            return EVisibility::Visible;
        }
    }
    return EVisibility::Collapsed;
}

void FOdysseyBrushVarActionDetails::OnFinishedChangingProperties(const FPropertyChangedEvent& InPropertyChangedEvent, TSharedPtr<FStructOnScope> InStructData, TWeakObjectPtr<UK2Node_EditablePinBase> InEntryNode)
{
    if( !InPropertyChangedEvent.MemberProperty ||
        !InPropertyChangedEvent.MemberProperty->GetOwnerStruct() ||
        !InPropertyChangedEvent.MemberProperty->GetOwnerStruct()->IsA<UFunction>())
    {
        return;
    }

    // Find the top level property that was modified within the UFunction
    const UProperty* DirectProperty = InPropertyChangedEvent.MemberProperty;
    while (!Cast<const UFunction>(DirectProperty->GetOuter()))
    {
        DirectProperty = CastChecked<const UProperty>(DirectProperty->GetOuter());
    }

    FString DefaultValueString;
    bool bDefaultValueSet = false;

    if (InStructData.IsValid())
    {
        bDefaultValueSet = FBlueprintEditorUtils::PropertyValueToString(DirectProperty, InStructData->GetStructMemory(), DefaultValueString);

        if(bDefaultValueSet)
        {
            UK2Node_FunctionEntry* FuncEntry = Cast<UK2Node_FunctionEntry>(InEntryNode.Get());

            // Search out the correct local variable in the Function Entry Node and set the default value
            for (FBPVariableDescription& LocalVar : FuncEntry->LocalVariables)
            {
                if (LocalVar.VarName == DirectProperty->GetFName() && LocalVar.DefaultValue != DefaultValueString)
                {
                    const FScopedTransaction Transaction(LOCTEXT("ChangeDefaults", "Change Defaults"));

                    FuncEntry->Modify();
                    GetBlueprintObj()->Modify();
                    LocalVar.DefaultValue = DefaultValueString;
                    FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprintObj());
                    break;
                }
            }
        }
    }
}

bool FOdysseyBrushVarActionDetails::IsVariableInheritedByOdysseyBrush() const
{
    UClass* PropertyOwnerClass = nullptr;
    if (UBlueprint* PropertyOwnerBP = GetPropertyOwnerOdysseyBrush())
    {
        PropertyOwnerClass = PropertyOwnerBP->SkeletonGeneratedClass;
    }
    else if (CachedVariableProperty.IsValid())
    {
        PropertyOwnerClass = CachedVariableProperty->GetOwnerClass();
    }
    return GetBlueprintObj()->SkeletonGeneratedClass->IsChildOf(PropertyOwnerClass);
}

static FDetailWidgetRow& AddRow( TArray<TSharedRef<FDetailWidgetRow> >& OutChildRows )
{
    TSharedRef<FDetailWidgetRow> NewRow( new FDetailWidgetRow );
    OutChildRows.Add( NewRow );

    return *NewRow;
}

void FOdysseyBrushGraphArgumentGroupLayout::SetOnRebuildChildren( FSimpleDelegate InOnRegenerateChildren )
{
    GraphActionDetailsPtr.Pin()->SetRefreshDelegate(InOnRegenerateChildren, TargetNode == GraphActionDetailsPtr.Pin()->GetFunctionEntryNode().Get());
}

void FOdysseyBrushGraphArgumentGroupLayout::GenerateChildContent( IDetailChildrenBuilder& ChildrenBuilder )
{
    bool WasContentAdded = false;
    if(TargetNode.IsValid())
    {
        TArray<TSharedPtr<FUserPinInfo>> Pins = TargetNode->UserDefinedPins;

        if(Pins.Num() > 0)
        {
            bool bIsInputNode = TargetNode == GraphActionDetailsPtr.Pin()->GetFunctionEntryNode().Get();
            for (int32 i = 0; i < Pins.Num(); ++i)
            {
                TSharedRef<class FOdysseyBrushGraphArgumentLayout> OdysseyBrushArgumentLayout = MakeShareable(new FOdysseyBrushGraphArgumentLayout(
                    TWeakPtr<FUserPinInfo>(Pins[i]),
                    TargetNode.Get(),
                    GraphActionDetailsPtr,
                    FName(*(bIsInputNode ? FString::Printf(TEXT("InputArgument%i"), i) : FString(TEXT("OutputArgument%i"), i))),
                    bIsInputNode));
                ChildrenBuilder.AddCustomBuilder(OdysseyBrushArgumentLayout);
                WasContentAdded = true;
            }
        }
    }
    if (!WasContentAdded)
    {
        // Add a text widget to let the user know to hit the + icon to add parameters.
        ChildrenBuilder.AddCustomRow(FText::GetEmpty()).WholeRowContent()
            .MaxDesiredWidth(980.f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                    .VAlign(VAlign_Center)
                    .Padding(0.0f, 0.0f, 4.0f, 0.0f)
                    .AutoWidth()
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("NoArgumentsAddedForOdysseyBrush", "Please press the + icon above to add parameters"))
                        .Font(IDetailLayoutBuilder::GetDetailFont())
                    ]
            ];
    }
}

// Internal
static bool ShouldAllowWildcard(UK2Node_EditablePinBase* TargetNode)
{
    // allow wildcards for tunnel nodes in macro graphs
    if ( TargetNode->IsA(UK2Node_Tunnel::StaticClass()) )
    {
        const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
        return ( K2Schema->GetGraphType( TargetNode->GetGraph() ) == GT_Macro );
    }

    return false;
}

void FOdysseyBrushGraphArgumentLayout::GenerateHeaderRowContent( FDetailWidgetRow& NodeRow )
{
    const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

    ETypeTreeFilter TypeTreeFilter = ETypeTreeFilter::None;
    if (TargetNode->CanModifyExecutionWires())
    {
        TypeTreeFilter |= ETypeTreeFilter::AllowExec;
    }

    if (ShouldAllowWildcard(TargetNode))
    {
        TypeTreeFilter |= ETypeTreeFilter::AllowWildcard;
    }

    NodeRow
    .NameContent()
    [
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .FillWidth(1)
        .VAlign(VAlign_Center)
        [
            SAssignNew(ArgumentNameWidget, SEditableTextBox)
                .Text( this, &FOdysseyBrushGraphArgumentLayout::OnGetArgNameText )
                .OnTextChanged(this, &FOdysseyBrushGraphArgumentLayout::OnArgNameChange)
                .OnTextCommitted(this, &FOdysseyBrushGraphArgumentLayout::OnArgNameTextCommitted)
                .ToolTipText(this, &FOdysseyBrushGraphArgumentLayout::OnGetArgToolTipText)
                .Font( IDetailLayoutBuilder::GetDetailFont() )
                .IsEnabled(!ShouldPinBeReadOnly())
        ]
    ]
    .ValueContent()
    .MaxDesiredWidth(980.f)
    [
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
        .AutoWidth()
        [
            SNew(SPinTypeSelector, FGetPinTypeTree::CreateUObject(K2Schema, &UEdGraphSchema_OdysseyBrush::GetVariableTypeTree))
                .TargetPinType(this, &FOdysseyBrushGraphArgumentLayout::OnGetPinInfo)
                .OnPinTypePreChanged(this, &FOdysseyBrushGraphArgumentLayout::OnPrePinInfoChange)
                .OnPinTypeChanged(this, &FOdysseyBrushGraphArgumentLayout::PinInfoChanged)
                .Schema(K2Schema)
                .TypeTreeFilter(TypeTreeFilter)
                .bAllowArrays(!ShouldPinBeReadOnly())
                .IsEnabled(!ShouldPinBeReadOnly(true))
                .Font( IDetailLayoutBuilder::GetDetailFont() )
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ContentPadding(0)
            .IsEnabled(!IsPinEditingReadOnly())
            .OnClicked(this, &FOdysseyBrushGraphArgumentLayout::OnArgMoveUp)
            .ToolTipText(LOCTEXT("FunctionArgDetailsArgMoveUpTooltip", "Move this parameter up in the list."))
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush("OdysseyBrushEditor.Details.ArgUpButton"))
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(2, 0)
        [
            SNew(SButton)
            .ContentPadding(0)
            .IsEnabled(!IsPinEditingReadOnly())
            .OnClicked(this, &FOdysseyBrushGraphArgumentLayout::OnArgMoveDown)
            .ToolTipText(LOCTEXT("FunctionArgDetailsArgMoveDownTooltip", "Move this parameter down in the list."))
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush("OdysseyBrushEditor.Details.ArgDownButton"))
            ]
        ]
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Center)
        .Padding(10, 0, 0, 0)
        .AutoWidth()
        [
            PropertyCustomizationHelpers::MakeClearButton(FSimpleDelegate::CreateSP(this, &FOdysseyBrushGraphArgumentLayout::OnRemoveClicked), LOCTEXT("FunctionArgDetailsClearTooltip", "Remove this parameter."), !IsPinEditingReadOnly())
        ]

    ];
}

void FOdysseyBrushGraphArgumentLayout::GenerateChildContent( IDetailChildrenBuilder& ChildrenBuilder )
{
    if (bHasDefaultValue)
    {
        UEdGraphPin* FoundPin = GetPin();
        if (FoundPin)
        {
            // Certain types are outlawed at the compiler level
            const bool bTypeWithNoDefaults = (FoundPin->PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Object) || (FoundPin->PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Class) || (FoundPin->PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Interface) || UEdGraphSchema_OdysseyBrush::IsExecPin(*FoundPin);

            if (!FoundPin->PinType.bIsReference && !bTypeWithNoDefaults)
            {
                DefaultValuePinWidget = FNodeFactory::CreatePinWidget(FoundPin);
                DefaultValuePinWidget->SetOnlyShowDefaultValue(true);
                TSharedRef<SWidget> DefaultValueWidget = DefaultValuePinWidget->GetDefaultValueWidget();

                if (DefaultValueWidget != SNullWidget::NullWidget)
                {
                    ChildrenBuilder.AddCustomRow(LOCTEXT("FunctionArgDetailsDefaultValue", "Default Value"))
                        .NameContent()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("FunctionArgDetailsDefaultValue", "Default Value"))
                            .ToolTipText(LOCTEXT("FunctionArgDetailsDefaultValueParamTooltip", "The default value of the parameter."))
                            .Font(IDetailLayoutBuilder::GetDetailFont())
                        ]
                        .ValueContent()
                        .MaxDesiredWidth(512)
                        [
                            DefaultValueWidget
                        ];
                }
                else
                {
                    DefaultValuePinWidget.Reset();
                }
            }
        }

        // Exec pins can't be passed by reference
        if (FoundPin == nullptr || !UEdGraphSchema_OdysseyBrush::IsExecPin(*FoundPin))
        {
            ChildrenBuilder.AddCustomRow(LOCTEXT("FunctionArgDetailsPassByReference", "Pass-by-Reference"))
                .NameContent()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("FunctionArgDetailsPassByReference", "Pass-by-Reference"))
                .ToolTipText(LOCTEXT("FunctionArgDetailsPassByReferenceTooltip", "Pass this paremeter by reference?"))
                .Font(IDetailLayoutBuilder::GetDetailFont())
                ]
            .ValueContent()
                [
                    SNew(SCheckBox)
                    .IsChecked(this, &FOdysseyBrushGraphArgumentLayout::IsRefChecked)
                .OnCheckStateChanged(this, &FOdysseyBrushGraphArgumentLayout::OnRefCheckStateChanged)
                .IsEnabled(!ShouldPinBeReadOnly())
                ];
        }
    }


}

namespace
{
    static TArray<UK2Node_EditablePinBase*> GatherAllResultNodes(UK2Node_EditablePinBase* TargetNode)
    {
        if (UK2Node_FunctionResult* ResultNode = Cast<UK2Node_FunctionResult>(TargetNode))
        {
            return (TArray<UK2Node_EditablePinBase*>)ResultNode->GetAllResultNodes();
        }
        TArray<UK2Node_EditablePinBase*> Result;
        if (TargetNode)
        {
            Result.Add(TargetNode);
        }
        return Result;
    }

} // namespace

void FOdysseyBrushGraphArgumentLayout::OnRemoveClicked()
{
    TSharedPtr<FUserPinInfo> ParamItem = ParamItemPtr.Pin();
    if (ParamItem.IsValid())
    {
        const FScopedTransaction Transaction( LOCTEXT( "RemoveParam", "Remove Parameter" ) );

        TSharedPtr<FBaseOdysseyBrushGraphActionDetails> GraphActionDetails = GraphActionDetailsPtr.Pin();
        TArray<UK2Node_EditablePinBase*> TargetNodes = GatherAllResultNodes(TargetNode);
        for (UK2Node_EditablePinBase* Node : TargetNodes)
        {
            Node->Modify();
            Node->RemoveUserDefinedPinByName(ParamItem->PinName);

            if (GraphActionDetails.IsValid())
            {
                GraphActionDetails->OnParamsChanged(Node, true);
            }
        }
    }
}

FReply FOdysseyBrushGraphArgumentLayout::OnArgMoveUp()
{
    const int32 ThisParamIndex = TargetNode->UserDefinedPins.Find( ParamItemPtr.Pin() );
    const int32 NewParamIndex = ThisParamIndex-1;
    if (ThisParamIndex != INDEX_NONE && NewParamIndex >= 0)
    {
        const FScopedTransaction Transaction( LOCTEXT("K2_MovePinUp", "Move Pin Up") );
        TArray<UK2Node_EditablePinBase*> TargetNodes = GatherAllResultNodes(TargetNode);
        for (UK2Node_EditablePinBase* Node : TargetNodes)
        {
            Node->Modify();
            Node->UserDefinedPins.Swap(ThisParamIndex, NewParamIndex);

            TSharedPtr<FBaseOdysseyBrushGraphActionDetails> GraphActionDetails = GraphActionDetailsPtr.Pin();
            if (GraphActionDetails.IsValid())
            {
                GraphActionDetails->OnParamsChanged(Node, true);
            }
        }
    }
    return FReply::Handled();
}

FReply FOdysseyBrushGraphArgumentLayout::OnArgMoveDown()
{
    const int32 ThisParamIndex = TargetNode->UserDefinedPins.Find( ParamItemPtr.Pin() );
    const int32 NewParamIndex = ThisParamIndex+1;
    if (ThisParamIndex != INDEX_NONE && NewParamIndex < TargetNode->UserDefinedPins.Num())
    {
        const FScopedTransaction Transaction( LOCTEXT("K2_MovePinDown", "Move Pin Down") );
        TArray<UK2Node_EditablePinBase*> TargetNodes = GatherAllResultNodes(TargetNode);
        for (UK2Node_EditablePinBase* Node : TargetNodes)
        {
            Node->Modify();
            Node->UserDefinedPins.Swap(ThisParamIndex, NewParamIndex);

            TSharedPtr<FBaseOdysseyBrushGraphActionDetails> GraphActionDetails = GraphActionDetailsPtr.Pin();
            if (GraphActionDetails.IsValid())
            {
                GraphActionDetails->OnParamsChanged(Node, true);
            }
        }
    }
    return FReply::Handled();
}

bool FOdysseyBrushGraphArgumentLayout::ShouldPinBeReadOnly(bool bIsEditingPinType/* = false*/) const
{
    if (TargetNode && ParamItemPtr.IsValid())
    {
        // Right now, we only care that the user is unable to edit the auto-generated "then" pin
        if ((ParamItemPtr.Pin()->PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Exec) && (!TargetNode->CanModifyExecutionWires()))
        {
            return true;
        }
        else
        {
            // Check if pin editing is read only
            return IsPinEditingReadOnly(bIsEditingPinType);
        }
    }

    return false;
}

bool FOdysseyBrushGraphArgumentLayout::IsPinEditingReadOnly(bool bIsEditingPinType/* = false*/) const
{
    if(UEdGraph* NodeGraph = TargetNode->GetGraph())
    {
        // Math expression should not be modified directly (except for the pin type), do not let the user tweak the parameters
        if (!bIsEditingPinType && Cast<UK2Node_MathExpression>(NodeGraph->GetOuter()) )
        {
            return true;
        }
    }
    return false;
}

FText FOdysseyBrushGraphArgumentLayout::OnGetArgNameText() const
{
    if (ParamItemPtr.IsValid())
    {
        return FText::FromName(ParamItemPtr.Pin()->PinName);
    }
    return FText();
}

FText FOdysseyBrushGraphArgumentLayout::OnGetArgToolTipText() const
{
    if (ParamItemPtr.IsValid())
    {
        FText PinTypeText = UEdGraphSchema_OdysseyBrush::TypeToText(ParamItemPtr.Pin()->PinType);
        return FText::Format(LOCTEXT("OdysseyBrushArgToolTipText", "Name: {0}\nType: {1}"), FText::FromName(ParamItemPtr.Pin()->PinName), PinTypeText);
    }
    return FText::GetEmpty();
}

void FOdysseyBrushGraphArgumentLayout::OnArgNameChange(const FText& InNewText)
{
    bool bVerified = true;

    FText ErrorMessage;

    if (!ParamItemPtr.IsValid())
    {
        return;
    }

    if (InNewText.IsEmpty())
    {
        ErrorMessage = LOCTEXT("EmptyArgument", "Name cannot be empty!");
        bVerified = false;
    }
    else
    {
        bVerified = GraphActionDetailsPtr.Pin()->OnVerifyPinRename(TargetNode, ParamItemPtr.Pin()->PinName, InNewText.ToString(), ErrorMessage);
    }

    if(!bVerified)
    {
        ArgumentNameWidget.Pin()->SetError(ErrorMessage);
    }
    else
    {
        ArgumentNameWidget.Pin()->SetError(FText::GetEmpty());
    }
}

void FOdysseyBrushGraphArgumentLayout::OnArgNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
    if (!NewText.IsEmpty() && TargetNode && ParamItemPtr.IsValid() && GraphActionDetailsPtr.IsValid() && !ShouldPinBeReadOnly())
    {
        const FName OldName = ParamItemPtr.Pin()->PinName;
        const FString& NewName = NewText.ToString();
        if (OldName.ToString() != NewName)
        {
            GraphActionDetailsPtr.Pin()->OnPinRenamed(TargetNode, OldName, NewName);
        }
    }
}

FEdGraphPinType FOdysseyBrushGraphArgumentLayout::OnGetPinInfo() const
{
    if (ParamItemPtr.IsValid())
    {
        return ParamItemPtr.Pin()->PinType;
    }
    return FEdGraphPinType();
}

UEdGraphPin* FOdysseyBrushGraphArgumentLayout::GetPin() const
{
    if (ParamItemPtr.IsValid() && TargetNode)
    {
        return TargetNode->FindPin(ParamItemPtr.Pin()->PinName, ParamItemPtr.Pin()->DesiredPinDirection);
    }
    return nullptr;
}

ECheckBoxState FOdysseyBrushGraphArgumentLayout::IsRefChecked() const
{
    FEdGraphPinType PinType = OnGetPinInfo();
    return PinType.bIsReference? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FOdysseyBrushGraphArgumentLayout::OnRefCheckStateChanged(ECheckBoxState InState)
{
    FEdGraphPinType PinType = OnGetPinInfo();
    PinType.bIsReference = (InState == ECheckBoxState::Checked)? true : false;

    PinInfoChanged(PinType);
}

void FOdysseyBrushGraphArgumentLayout::PinInfoChanged(const FEdGraphPinType& PinType)
{
    if (ParamItemPtr.IsValid() && FBlueprintEditorUtils::IsPinTypeValid(PinType))
    {
        const FName PinName = ParamItemPtr.Pin()->PinName;
        TSharedPtr<class FBaseOdysseyBrushGraphActionDetails> GraphActionDetailsPinned = GraphActionDetailsPtr.Pin();
        if (GraphActionDetailsPinned.IsValid())
        {
            TSharedPtr<SMyOdysseyBrush> MyBPPinned = GraphActionDetailsPinned->GetMyOdysseyBrush().Pin();
            if (MyBPPinned.IsValid())
            {
                MyBPPinned->GetLastFunctionPinTypeUsed() = PinType;
            }
            if( !ShouldPinBeReadOnly(true) )
            {
                TArray<UK2Node_EditablePinBase*> TargetNodes = GatherAllResultNodes(TargetNode);
                for (UK2Node_EditablePinBase* Node : TargetNodes)
                {
                    if (Node)
                    {
                        TSharedPtr<FUserPinInfo>* UDPinPtr = Node->UserDefinedPins.FindByPredicate([PinName](TSharedPtr<FUserPinInfo>& UDPin)
                        {
                            return UDPin.IsValid() && (UDPin->PinName == PinName);
                        });
                        if (UDPinPtr)
                        {
                            (*UDPinPtr)->PinType = PinType;

                            // Inputs flagged as pass-by-reference will also be flagged as 'const' here to conform to the expected native C++
                            // declaration of 'const Type&' for input reference parameters on functions with no outputs (i.e. events). Array
                            // types are also flagged as 'const' here since they will always be implicitly passed by reference, regardless of
                            // the checkbox setting. See UEditablePinBase::PostLoad() for more details.
                            if(!PinType.bIsConst && Node->ShouldUseConstRefParams())
                            {
                                (*UDPinPtr)->PinType.bIsConst = PinType.IsArray() || PinType.bIsReference;
                            }

                            // Reset default value, it probably doesn't match
                            (*UDPinPtr)->PinDefaultValue.Reset();
                        }
                        GraphActionDetailsPinned->OnParamsChanged(Node);
                    }
                }
            }
        }
    }
}

void FOdysseyBrushGraphArgumentLayout::OnPrePinInfoChange(const FEdGraphPinType& PinType)
{
    if (!ShouldPinBeReadOnly(true))
    {
        TArray<UK2Node_EditablePinBase*> TargetNodes = GatherAllResultNodes(TargetNode);
        for (UK2Node_EditablePinBase* Node : TargetNodes)
        {
            if (Node)
            {
                Node->Modify();
            }
        }
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FOdysseyBrushGraphActionDetails::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
    DetailsLayoutPtr = &DetailLayout;
    ObjectsBeingEdited = DetailsLayoutPtr->GetSelectedObjects();

    SetEntryAndResultNodes();

    UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get();
    UK2Node_EditablePinBase* FunctionResultNode = FunctionResultNodePtr.Get();

    // Fill Access specifiers list
    AccessSpecifierLabels.Empty(3);
    AccessSpecifierLabels.Add( MakeShareable( new FAccessSpecifierLabel( AccessSpecifierProperName(FUNC_Public), FUNC_Public )));
    AccessSpecifierLabels.Add( MakeShareable( new FAccessSpecifierLabel( AccessSpecifierProperName(FUNC_Protected), FUNC_Protected )));
    AccessSpecifierLabels.Add( MakeShareable( new FAccessSpecifierLabel( AccessSpecifierProperName(FUNC_Private), FUNC_Private )));

    const bool bHasAGraph = (GetGraph() != NULL);

    if (FunctionEntryNode && FunctionEntryNode->IsEditable())
    {
        IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Graph", LOCTEXT("FunctionDetailsGraph", "Graph"));
        if (bHasAGraph)
        {
            Category.AddCustomRow( LOCTEXT( "DefaultTooltip", "Description" ) )
            .NameContent()
            [
                SNew(STextBlock)
                    .Text( LOCTEXT( "DefaultTooltip", "Description" ) )
                    .ToolTipText(LOCTEXT("FunctionTooltipTooltip", "Enter a short message describing the purpose and operation of this graph"))
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
            ]
            .ValueContent()
            [
                SNew(SMultiLineEditableTextBox)
                    .Text( this, &FOdysseyBrushGraphActionDetails::OnGetTooltipText )
                    .OnTextCommitted( this, &FOdysseyBrushGraphActionDetails::OnTooltipTextCommitted )
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
                    .ModiferKeyForNewLine(EModifierKey::Shift)
            ];

            // Composite graphs are auto-categorized into their parent graph
            if(!GetGraph()->GetOuter()->GetClass()->IsChildOf(UK2Node_Composite::StaticClass()))
            {
                FOdysseyBrushVarActionDetails::PopulateCategories(MyOdysseyBrush.Pin().Get(), CategorySource);
                TSharedPtr<SComboButton> NewComboButton;
                TSharedPtr<SListView<TSharedPtr<FText>>> NewListView;

                const FString DocLink = TEXT("Shared/Editors/OdysseyBrushEditor/GraphDetails");
                TSharedPtr<SToolTip> CategoryTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("EditGraphCategoryName_Tooltip", "The category of the graph; editing this will place the graph into another category or create a new one."), NULL, DocLink, TEXT("Category"));

                Category.AddCustomRow( LOCTEXT("CategoryLabel", "Category") )
                    .NameContent()
                    [
                        SNew(STextBlock)
                        .Text( LOCTEXT("CategoryLabel", "Category") )
                        .ToolTip(CategoryTooltip)
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                    ]
                .ValueContent()
                    [
                        SAssignNew(NewComboButton, SComboButton)
                        .ContentPadding(FMargin(0,0,5,0))
                        .ToolTip(CategoryTooltip)
                        .ButtonContent()
                        [
                            SNew(SBorder)
                            .BorderImage( FEditorStyle::GetBrush("NoBorder") )
                            .Padding(FMargin(0, 0, 5, 0))
                            [
                                SNew(SEditableTextBox)
                                .Text(this, &FOdysseyBrushGraphActionDetails::OnGetCategoryText)
                                .OnTextCommitted(this, &FOdysseyBrushGraphActionDetails::OnCategoryTextCommitted )
                                .ToolTip(CategoryTooltip)
                                .SelectAllTextWhenFocused(true)
                                .RevertTextOnEscape(true)
                                .Font( IDetailLayoutBuilder::GetDetailFont() )
                            ]
                        ]
                        .MenuContent()
                            [
                                SNew(SVerticalBox)
                                +SVerticalBox::Slot()
                                .AutoHeight()
                                .MaxHeight(400.0f)
                                [
                                    SAssignNew(NewListView, SListView<TSharedPtr<FText>>)
                                    .ListItemsSource(&CategorySource)
                                    .OnGenerateRow(this, &FOdysseyBrushGraphActionDetails::MakeCategoryViewWidget)
                                    .OnSelectionChanged(this, &FOdysseyBrushGraphActionDetails::OnCategorySelectionChanged)
                                ]
                            ]
                    ];

                CategoryComboButton = NewComboButton;
                CategoryListView = NewListView;

                TSharedPtr<SToolTip> KeywordsTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("EditKeywords_Tooltip", "Keywords for searching for the function or macro."), NULL, DocLink, TEXT("Keywords"));
                Category.AddCustomRow( LOCTEXT("KeywordsLabel", "Keywords") )
                    .NameContent()
                    [
                        SNew(STextBlock)
                        .Text( LOCTEXT("KeywordsLabel", "Keywords") )
                        .ToolTip(KeywordsTooltip)
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                    ]
                .ValueContent()
                    [
                        SNew(SEditableTextBox)
                        .Text(this, &FOdysseyBrushGraphActionDetails::OnGetKeywordsText)
                        .OnTextCommitted(this, &FOdysseyBrushGraphActionDetails::OnKeywordsTextCommitted )
                        .ToolTip(KeywordsTooltip)
                        .RevertTextOnEscape(true)
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                    ];

                TSharedPtr<SToolTip> CompactNodeTitleTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("EditCompactNodeTitle_Tooltip", "Sets the compact node title for calls to this function or macro. Compact node titles convert a node to display as a compact node and are used as a keyword for searching."), NULL, DocLink, TEXT("Compact Node Title"));
                Category.AddCustomRow( LOCTEXT("CompactNodeTitleLabel", "Compact Node Title") )
                    .NameContent()
                    [
                        SNew(STextBlock)
                        .Text( LOCTEXT("CompactNodeTitleLabel", "Compact Node Title") )
                        .ToolTip(CompactNodeTitleTooltip)
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                    ]
                .ValueContent()
                    [
                        SNew(SEditableTextBox)
                        .Text(this, &FOdysseyBrushGraphActionDetails::OnGetCompactNodeTitleText)
                        .OnTextCommitted(this, &FOdysseyBrushGraphActionDetails::OnCompactNodeTitleTextCommitted )
                        .ToolTip(CompactNodeTitleTooltip)
                        .RevertTextOnEscape(true)
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                    ];
            }

            if (IsAccessSpecifierVisible())
            {
                Category.AddCustomRow( LOCTEXT( "AccessSpecifier", "Access Specifier" ) )
                .NameContent()
                [
                    SNew(STextBlock)
                        .Text( LOCTEXT( "AccessSpecifier", "Access Specifier" ) )
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                ]
                .ValueContent()
                [
                    SAssignNew(AccessSpecifierComboButton, SComboButton)
                    .ContentPadding(0)
                    .ButtonContent()
                    [
                        SNew(STextBlock)
                            .Text(this, &FOdysseyBrushGraphActionDetails::GetCurrentAccessSpecifierName)
                            .Font( IDetailLayoutBuilder::GetDetailFont() )
                    ]
                    .MenuContent()
                    [
                        SNew(SListView<TSharedPtr<FAccessSpecifierLabel> >)
                            .ListItemsSource( &AccessSpecifierLabels )
                            .OnGenerateRow(this, &FOdysseyBrushGraphActionDetails::HandleGenerateRowAccessSpecifier)
                            .OnSelectionChanged(this, &FOdysseyBrushGraphActionDetails::OnAccessSpecifierSelected)
                    ]
                ];
            }
            if (GetInstanceColorVisibility())
            {
                Category.AddCustomRow( LOCTEXT( "InstanceColor", "Instance Color" ) )
                .NameContent()
                [
                    SNew(STextBlock)
                        .Text( LOCTEXT( "InstanceColor", "Instance Color" ) )
                        .ToolTipText( LOCTEXT("FunctionColorTooltip", "Choose a title bar color for references of this graph") )
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                ]
                .ValueContent()
                [
                    SAssignNew( ColorBlock, SColorBlock )
                        .Color( this, &FOdysseyBrushGraphActionDetails::GetNodeTitleColor )
                        .IgnoreAlpha(true)
                        .OnMouseButtonDown( this, &FOdysseyBrushGraphActionDetails::ColorBlock_OnMouseButtonDown )
                ];
            }
            if (IsPureFunctionVisible())
            {
                Category.AddCustomRow( LOCTEXT( "FunctionPure_Tooltip", "Pure" ) )
                .NameContent()
                [
                    SNew(STextBlock)
                        .Text( LOCTEXT( "FunctionPure_Tooltip", "Pure" ) )
                        .ToolTipText( LOCTEXT("FunctionIsPure_Tooltip", "Force this to be a pure function?") )
                        .Font( IDetailLayoutBuilder::GetDetailFont() )
                ]
                .ValueContent()
                [
                    SNew( SCheckBox )
                        .IsChecked( this, &FOdysseyBrushGraphActionDetails::GetIsPureFunction )
                        .OnCheckStateChanged( this, &FOdysseyBrushGraphActionDetails::OnIsPureFunctionModified )
                ];
            }
            if (IsConstFunctionVisible())
            {
                Category.AddCustomRow( LOCTEXT( "FunctionConst_Tooltip", "Const" ), true )
                .NameContent()
                [
                    SNew(STextBlock)
                    .Text( LOCTEXT( "FunctionConst_Tooltip", "Const" ) )
                    .ToolTipText( LOCTEXT("FunctionIsConst_Tooltip", "Force this to be a const function?") )
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
                ]
                .ValueContent()
                [
                    SNew( SCheckBox )
                    .IsChecked( this, &FOdysseyBrushGraphActionDetails::GetIsConstFunction )
                    .OnCheckStateChanged( this, &FOdysseyBrushGraphActionDetails::OnIsConstFunctionModified )
                ];
            }
        }

        if (IsCustomEvent())
        {
            /** A collection of static utility callbacks to provide the custom-event details ui with */
            struct LocalCustomEventUtils
            {
                /** Checks to see if the selected node is NOT an override */
                static bool IsNotCustomEventOverride(TWeakObjectPtr<UK2Node_EditablePinBase> SelectedNode)
                {
                    bool bIsOverride = false;
                    if (SelectedNode.IsValid())
                    {
                        UK2Node_CustomEvent const* SelectedCustomEvent = Cast<UK2Node_CustomEvent const>(SelectedNode.Get());
                        check(SelectedCustomEvent != NULL);

                        bIsOverride = SelectedCustomEvent->IsOverride();
                    }

                    return !bIsOverride;
                }

                /** If the selected node represent an override, this returns tooltip text explaining why you can't alter the replication settings */
                static FText GetDisabledTooltip(TWeakObjectPtr<UK2Node_EditablePinBase> SelectedNode)
                {
                    FText ToolTipOut = FText::GetEmpty();
                    if (!IsNotCustomEventOverride(SelectedNode))
                    {
                        ToolTipOut = LOCTEXT("CannotChangeOverrideReplication", "Cannot alter a custom-event's replication settings when it overrides an event declared in a parent.");
                    }
                    return ToolTipOut;
                }

                /** Determines if the selected node's "Reliable" net setting should be enabled for the user */
                static bool CanSetReliabilityProperty(TWeakObjectPtr<UK2Node_EditablePinBase> SelectedNode)
                {
                    bool bIsReliabilitySettingEnabled = false;
                    if (IsNotCustomEventOverride(SelectedNode))
                    {
                        UK2Node_CustomEvent const* SelectedCustomEvent = Cast<UK2Node_CustomEvent const>(SelectedNode.Get());
                        check(SelectedCustomEvent != NULL);

                        bIsReliabilitySettingEnabled = ((SelectedCustomEvent->GetNetFlags() & FUNC_Net) != 0);
                    }
                    return bIsReliabilitySettingEnabled;
                }
            };
            FCanExecuteAction CanExecuteDelegate = FCanExecuteAction::CreateStatic(&LocalCustomEventUtils::IsNotCustomEventOverride, FunctionEntryNodePtr);

            FMenuBuilder RepComboMenu( true, NULL );
            RepComboMenu.AddMenuEntry(     ReplicationSpecifierProperName(0),
                                        LOCTEXT("NotReplicatedToolTip", "This event is not replicated to anyone."),
                                        FSlateIcon(),
                                        FUIAction(FExecuteAction::CreateStatic( &FOdysseyBrushGraphActionDetails::SetNetFlags, FunctionEntryNodePtr, 0U ), CanExecuteDelegate));
            RepComboMenu.AddMenuEntry(    ReplicationSpecifierProperName(FUNC_NetMulticast),
                                        LOCTEXT("MulticastToolTip", "Replicate this event from the server to everyone else. Server executes this event locally too. Only call this from the server."),
                                        FSlateIcon(),
                                        FUIAction(FExecuteAction::CreateStatic( &FOdysseyBrushGraphActionDetails::SetNetFlags, FunctionEntryNodePtr, static_cast<uint32>(FUNC_NetMulticast) ), CanExecuteDelegate));
            RepComboMenu.AddMenuEntry(    ReplicationSpecifierProperName(FUNC_NetServer),
                                        LOCTEXT("ServerToolTip", "Replicate this event from net owning client to server."),
                                        FSlateIcon(),
                                        FUIAction(FExecuteAction::CreateStatic( &FOdysseyBrushGraphActionDetails::SetNetFlags, FunctionEntryNodePtr, static_cast<uint32>(FUNC_NetServer) ), CanExecuteDelegate));
            RepComboMenu.AddMenuEntry(    ReplicationSpecifierProperName(FUNC_NetClient),
                                        LOCTEXT("ClientToolTip", "Replicate this event from the server to owning client."),
                                        FSlateIcon(),
                                        FUIAction(FExecuteAction::CreateStatic( &FOdysseyBrushGraphActionDetails::SetNetFlags, FunctionEntryNodePtr, static_cast<uint32>(FUNC_NetClient) ), CanExecuteDelegate));

            Category.AddCustomRow( LOCTEXT( "FunctionReplicate", "Replicates" ) )
            .NameContent()
            [
                SNew(STextBlock)
                    .Text( LOCTEXT( "FunctionReplicate", "Replicates" ) )
                    .ToolTipText( LOCTEXT("FunctionReplicate_Tooltip", "Should this Event be replicated to all clients when called on the server?") )
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
            ]
            .ValueContent()
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                [
                    SNew(SComboButton)
                        .ContentPadding(0)
                        .IsEnabled_Static(&LocalCustomEventUtils::IsNotCustomEventOverride, FunctionEntryNodePtr)
                        .ToolTipText_Static(&LocalCustomEventUtils::GetDisabledTooltip, FunctionEntryNodePtr)
                        .ButtonContent()
                        [
                            SNew(STextBlock)
                                .Text(this, &FOdysseyBrushGraphActionDetails::GetCurrentReplicatedEventString)
                                .Font( IDetailLayoutBuilder::GetDetailFont() )
                        ]
                        .MenuContent()
                        [
                            SNew(SVerticalBox)
                            +SVerticalBox::Slot()
                            [
                                SNew(SVerticalBox)
                                +SVerticalBox::Slot()
                                    .AutoHeight()
                                    .MaxHeight(400.0f)
                                [
                                    RepComboMenu.MakeWidget()
                                ]
                            ]
                        ]
                ]

                +SVerticalBox::Slot()
                    .AutoHeight()
                    .MaxHeight(400.0f)
                [
                    SNew(SHorizontalBox)
                    +SHorizontalBox::Slot()
                        .AutoWidth()
                    [
                        SNew( SCheckBox )
                            .IsChecked( this, &FOdysseyBrushGraphActionDetails::GetIsReliableReplicatedFunction )
                            .IsEnabled_Static(&LocalCustomEventUtils::CanSetReliabilityProperty, FunctionEntryNodePtr)
                            .ToolTipText_Static(&LocalCustomEventUtils::GetDisabledTooltip, FunctionEntryNodePtr)
                            .OnCheckStateChanged( this, &FOdysseyBrushGraphActionDetails::OnIsReliableReplicationFunctionModified )
                        [
                            SNew(STextBlock)
                                .Text( LOCTEXT( "FunctionReplicateReliable", "Reliable" ) )
                        ]
                    ]
                ]
            ];
        }
        //ODYSSEY: PATCH
        //const bool bShowCallInEditor = IsCustomEvent() || FBlueprintEditorUtils::IsBlutility( GetBlueprintObj() ) || (FunctionEntryNode && FunctionEntryNode->IsEditable());
        const bool bShowCallInEditor = IsCustomEvent() || (FunctionEntryNode && FunctionEntryNode->IsEditable());
        if( bShowCallInEditor )
        {
            Category.AddCustomRow( LOCTEXT( "EditorCallable", "Call In Editor" ) )
            .NameContent()
            [
                SNew(STextBlock)
                    .Text( LOCTEXT( "EditorCallable", "Call In Editor" ) )
                    .ToolTipText( LOCTEXT("EditorCallable_Tooltip", "Enable this event to be called from within the editor") )
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
            ]
            .ValueContent()
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                    +SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew( SCheckBox )
                        .IsChecked( this, &FOdysseyBrushGraphActionDetails::GetIsEditorCallableEvent )
                        .ToolTipText( LOCTEXT("EditorCallable_Tooltip", "Enable this event to be called from within the editor" ))
                        .OnCheckStateChanged( this, &FOdysseyBrushGraphActionDetails::OnEditorCallableEventModified )
                    ]
                ]
            ];
        }

        IDetailCategoryBuilder& InputsCategory = DetailLayout.EditCategory("Inputs", LOCTEXT("FunctionDetailsInputs", "Inputs"));

        TSharedRef<FOdysseyBrushGraphArgumentGroupLayout> InputArgumentGroup =
            MakeShareable(new FOdysseyBrushGraphArgumentGroupLayout(SharedThis(this), FunctionEntryNode));
        InputsCategory.AddCustomBuilder(InputArgumentGroup);

        TSharedRef<SHorizontalBox> InputsHeaderContentWidget = SNew(SHorizontalBox);
        TWeakPtr<SWidget> WeakInputsHeaderWidget = InputsHeaderContentWidget;
        InputsHeaderContentWidget->AddSlot()
            [
                SNew(SHorizontalBox)
            ];
        InputsHeaderContentWidget->AddSlot()
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(FEditorStyle::Get(), "RoundButton")
                .ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
                .ContentPadding(FMargin(2, 0))
                .OnClicked(this, &FOdysseyBrushGraphActionDetails::OnAddNewInputClicked)
                .Visibility(this, &FOdysseyBrushGraphActionDetails::GetAddNewInputOutputVisibility)
                .HAlign(HAlign_Right)
                .ToolTipText(LOCTEXT("FunctionNewInputArgTooltip", "Create a new input argument"))
                .VAlign(VAlign_Center)
                .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("FunctionNewInputArg")))
                [
                    SNew(SHorizontalBox)

                    + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(0, 1))
                        [
                            SNew(SImage)
                            .Image(FEditorStyle::GetBrush("Plus"))
                        ]

                    + SHorizontalBox::Slot()
                        .VAlign(VAlign_Center)
                        .AutoWidth()
                        .Padding(FMargin(2, 0, 0, 0))
                        [
                            SNew(STextBlock)
                            .Font(IDetailLayoutBuilder::GetDetailFontBold())
                            .Text(LOCTEXT("FunctionNewParameterInputArg", "New Parameter"))
                            .Visibility(this, &FOdysseyBrushGraphActionDetails::OnGetSectionTextVisibility, WeakInputsHeaderWidget)
                            .ShadowOffset(FVector2D(1, 1))
                        ]
                ]
            ];
        InputsCategory.HeaderContent(InputsHeaderContentWidget);

        if (bHasAGraph)
        {
            IDetailCategoryBuilder& OutputsCategory = DetailLayout.EditCategory("Outputs", LOCTEXT("FunctionDetailsOutputs", "Outputs"));

            TSharedRef<FOdysseyBrushGraphArgumentGroupLayout> OutputArgumentGroup =
                MakeShareable(new FOdysseyBrushGraphArgumentGroupLayout(SharedThis(this), FunctionResultNode));
            OutputsCategory.AddCustomBuilder(OutputArgumentGroup);

            TSharedRef<SHorizontalBox> OutputsHeaderContentWidget = SNew(SHorizontalBox);
            TWeakPtr<SWidget> WeakOutputsHeaderWidget = OutputsHeaderContentWidget;
            OutputsHeaderContentWidget->AddSlot()
                [
                    SNew(SHorizontalBox)
                ];
            OutputsHeaderContentWidget->AddSlot()
                .AutoWidth()
                [
                    SNew(SButton)
                    .ButtonStyle(FEditorStyle::Get(), "RoundButton")
                    .ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
                    .ContentPadding(FMargin(2, 0))
                    .OnClicked(this, &FOdysseyBrushGraphActionDetails::OnAddNewOutputClicked)
                    .Visibility(this, &FOdysseyBrushGraphActionDetails::GetAddNewInputOutputVisibility)
                    .HAlign(HAlign_Right)
                    .ToolTipText(LOCTEXT("FunctionNewOutputArgTooltip", "Create a new output argument"))
                    .VAlign(VAlign_Center)
                    .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("FunctionNewOutputArg")))
                    [
                        SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding(FMargin(0, 1))
                    [
                        SNew(SImage)
                            .Image(FEditorStyle::GetBrush("Plus"))
                    ]

                + SHorizontalBox::Slot()
                    .VAlign(VAlign_Center)
                    .AutoWidth()
                    .Padding(FMargin(2, 0, 0, 0))
                    [
                        SNew(STextBlock)
                        .Font(IDetailLayoutBuilder::GetDetailFontBold())
                        .Text(LOCTEXT("FunctionNewOutputArg", "New Parameter"))
                        .Visibility(this, &FOdysseyBrushGraphActionDetails::OnGetSectionTextVisibility, WeakOutputsHeaderWidget)
                        .ShadowOffset(FVector2D(1, 1))
                    ]
                ]
            ];
            OutputsCategory.HeaderContent(OutputsHeaderContentWidget);
        }
    }
    else
    {
        if (bHasAGraph)
        {
            IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Graph", LOCTEXT("FunctionDetailsGraph", "Graph"));
            Category.AddCustomRow( FText::GetEmpty() )
            [
                SNew(STextBlock)
                .Text( LOCTEXT("GraphPresentButNotEditable", "Graph is not editable.") )
            ];
        }
    }

    if (MyOdysseyBrush.IsValid())
    {
        TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditor = MyOdysseyBrush.Pin()->GetBlueprintEditor();
        if (OdysseyBrushEditor.IsValid())
        {
            OdysseyBrushEditorRefreshDelegateHandle = OdysseyBrushEditor.Pin()->OnRefresh().AddSP(this, &FOdysseyBrushGraphActionDetails::OnPostEditorRefresh);
        }
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedRef<ITableRow> FOdysseyBrushGraphActionDetails::OnGenerateReplicationComboWidget( TSharedPtr<FReplicationSpecifierLabel> InNetFlag, const TSharedRef<STableViewBase>& OwnerTable )
{
    return
        SNew(STableRow< TSharedPtr<FString> >, OwnerTable)
        [
            SNew( STextBlock )
            .Text( InNetFlag.IsValid() ? InNetFlag.Get()->LocalizedName : FText::GetEmpty() )
            .ToolTipText( InNetFlag.IsValid() ? InNetFlag.Get()->LocalizedToolTip : FText::GetEmpty() )
        ];
}

void FOdysseyBrushGraphActionDetails::SetNetFlags( TWeakObjectPtr<UK2Node_EditablePinBase> FunctionEntryNode, uint32 NetFlags)
{
    if( FunctionEntryNode.IsValid() )
    {
        const int32 FlagsToSet = NetFlags ? FUNC_Net|NetFlags : 0;
        const int32 FlagsToClear = FUNC_Net|FUNC_NetMulticast|FUNC_NetServer|FUNC_NetClient;
        // Clear all net flags before setting
        if( FlagsToSet != FlagsToClear )
        {
            const FScopedTransaction Transaction( LOCTEXT("GraphSetNetFlags", "Change Replication") );
            FunctionEntryNode->Modify();

            bool bOdysseyBrushModified = false;

            if (UK2Node_FunctionEntry* TypedEntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode.Get()))
            {
                int32 ExtraFlags = TypedEntryNode->GetExtraFlags();
                ExtraFlags &= ~FlagsToClear;
                ExtraFlags |= FlagsToSet;
                TypedEntryNode->SetExtraFlags(ExtraFlags);
                bOdysseyBrushModified = true;
            }
            if (UK2Node_CustomEvent * CustomEventNode = Cast<UK2Node_CustomEvent>(FunctionEntryNode.Get()))
            {
                CustomEventNode->FunctionFlags &= ~FlagsToClear;
                CustomEventNode->FunctionFlags |= FlagsToSet;
                bOdysseyBrushModified = true;
            }

            if( bOdysseyBrushModified )
            {
                FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified( FunctionEntryNode->GetBlueprint() );
            }
        }
    }
}

FText FOdysseyBrushGraphActionDetails::GetCurrentReplicatedEventString() const
{
    const UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    const UK2Node_CustomEvent* CustomEvent = Cast<const UK2Node_CustomEvent>(FunctionEntryNode);

    uint32 const ReplicatedNetMask = (FUNC_NetMulticast | FUNC_NetServer | FUNC_NetClient);

    FText ReplicationText;

    if(CustomEvent)
    {
        uint32 NetFlags = CustomEvent->FunctionFlags & ReplicatedNetMask;
        if (CustomEvent->IsOverride())
        {
            UFunction* SuperFunction = FindField<UFunction>(CustomEvent->GetBlueprint()->ParentClass, CustomEvent->CustomFunctionName);
            check(SuperFunction != NULL);

            NetFlags = SuperFunction->FunctionFlags & ReplicatedNetMask;
        }
        ReplicationText = ReplicationSpecifierProperName(NetFlags);
    }
    return ReplicationText;
}

bool FBaseOdysseyBrushGraphActionDetails::AttemptToCreateResultNode()
{
    if (!FunctionResultNodePtr.IsValid())
    {
        FunctionResultNodePtr = FBlueprintEditorUtils::FindOrCreateFunctionResultNode(FunctionEntryNodePtr.Get());
    }
    return FunctionResultNodePtr.IsValid();
}

FBaseOdysseyBrushGraphActionDetails::~FBaseOdysseyBrushGraphActionDetails()
{
    if (OdysseyBrushEditorRefreshDelegateHandle.IsValid() && MyOdysseyBrush.IsValid())
    {
        // Remove the callback delegate we registered for
        TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditor = MyOdysseyBrush.Pin()->GetBlueprintEditor();
        if (OdysseyBrushEditor.IsValid())
        {
            OdysseyBrushEditor.Pin()->OnRefresh().Remove(OdysseyBrushEditorRefreshDelegateHandle);
        }
    }
}

void FBaseOdysseyBrushGraphActionDetails::OnPostEditorRefresh()
{
    /** OdysseyBrush changed, need to refresh inputs in case pin UI changed */
    RegenerateInputsChildrenDelegate.ExecuteIfBound();
    RegenerateOutputsChildrenDelegate.ExecuteIfBound();
}

void FBaseOdysseyBrushGraphActionDetails::SetRefreshDelegate(FSimpleDelegate RefreshDelegate, bool bForInputs)
{
    ((bForInputs) ? RegenerateInputsChildrenDelegate : RegenerateOutputsChildrenDelegate) = RefreshDelegate;
}

ECheckBoxState FOdysseyBrushGraphActionDetails::GetIsEditorCallableEvent() const
{
    ECheckBoxState Result = ECheckBoxState::Unchecked;

    if( FunctionEntryNodePtr.IsValid() )
    {
        if( UK2Node_CustomEvent* CustomEventNode = Cast<UK2Node_CustomEvent>(FunctionEntryNodePtr.Get()))
        {
            if( CustomEventNode->bCallInEditor  )
            {
                Result = ECheckBoxState::Checked;
            }
        }
        else if( UK2Node_FunctionEntry* EntryPoint = Cast<UK2Node_FunctionEntry>(FunctionEntryNodePtr.Get()) )
        {
            if( EntryPoint->MetaData.bCallInEditor )
            {
                Result = ECheckBoxState::Checked;
            }
        }
    }
    return Result;
}

void FOdysseyBrushGraphActionDetails::OnEditorCallableEventModified( const ECheckBoxState NewCheckedState ) const
{
    if( FunctionEntryNodePtr.IsValid() )
    {
        const bool bCallInEditor = NewCheckedState == ECheckBoxState::Checked;
        const FText TransactionType = bCallInEditor ?    LOCTEXT( "DisableCallInEditor", "Disable Call In Editor " ) :
                                                        LOCTEXT( "EnableCallInEditor", "Enable Call In Editor" );

        if( UK2Node_CustomEvent* CustomEventNode = Cast<UK2Node_CustomEvent>(FunctionEntryNodePtr.Get()) )
        {
            if( UBlueprint* OdysseyBrush = FunctionEntryNodePtr->GetBlueprint() )
            {
                const FScopedTransaction Transaction( TransactionType );
                CustomEventNode->bCallInEditor = bCallInEditor;
                FBlueprintEditorUtils::MarkBlueprintAsModified( CustomEventNode->GetBlueprint() );
            }
        }
        else if( UK2Node_FunctionEntry* EntryPoint = Cast<UK2Node_FunctionEntry>(FunctionEntryNodePtr.Get()) )
        {
            const FScopedTransaction Transaction( TransactionType );
            EntryPoint->MetaData.bCallInEditor = bCallInEditor;
            FBlueprintEditorUtils::MarkBlueprintAsModified( EntryPoint->GetBlueprint() );
        }
    }
}

UMulticastDelegateProperty* FOdysseyBrushDelegateActionDetails::GetDelegateProperty() const
{
    if (MyOdysseyBrush.IsValid())
    {
        if (const FEdGraphSchemaAction_K2Delegate* DelegateVar = MyOdysseyBrush.Pin()->SelectionAsDelegate())
        {
            return DelegateVar->GetDelegateProperty();
        }
    }
    return NULL;
}

bool FOdysseyBrushDelegateActionDetails::IsOdysseyBrushProperty() const
{
    const UMulticastDelegateProperty* Property = GetDelegateProperty();
    const UBlueprint* OdysseyBrush = GetBlueprintObj();
    if(Property && OdysseyBrush)
    {
        return (Property->GetOuter() == OdysseyBrush->SkeletonGeneratedClass);
    }

    return false;
}

void FOdysseyBrushDelegateActionDetails::SetEntryNode()
{
    if (UEdGraph* NewTargetGraph = GetGraph())
    {
        TArray<UK2Node_FunctionEntry*> EntryNodes;
        NewTargetGraph->GetNodesOfClass(EntryNodes);

        if ((EntryNodes.Num() > 0) && EntryNodes[0]->IsEditable())
        {
            FunctionEntryNodePtr = EntryNodes[0];
        }
    }
}

UEdGraph* FOdysseyBrushDelegateActionDetails::GetGraph() const
{
    if (MyOdysseyBrush.IsValid())
    {
        if (const FEdGraphSchemaAction_K2Delegate* DelegateVar = MyOdysseyBrush.Pin()->SelectionAsDelegate())
        {
            return DelegateVar->EdGraph;
        }
    }
    return NULL;
}

FText FOdysseyBrushDelegateActionDetails::OnGetTooltipText() const
{
    if (UMulticastDelegateProperty* DelegateProperty = GetDelegateProperty())
    {
        FString Result;
        FBlueprintEditorUtils::GetBlueprintVariableMetaData(GetBlueprintObj(), DelegateProperty->GetFName(), NULL, TEXT("tooltip"), Result);
        return FText::FromString(Result);
    }
    return FText();
}

void FOdysseyBrushDelegateActionDetails::OnTooltipTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
    if (UMulticastDelegateProperty* DelegateProperty = GetDelegateProperty())
    {
        FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), DelegateProperty->GetFName(), NULL, TEXT("tooltip"), NewText.ToString() );
    }
}

FText FOdysseyBrushDelegateActionDetails::OnGetCategoryText() const
{
    if (UMulticastDelegateProperty* DelegateProperty = GetDelegateProperty())
    {
        FName DelegateName = DelegateProperty->GetFName();
        FText Category = FBlueprintEditorUtils::GetBlueprintVariableCategory(GetBlueprintObj(), DelegateName, NULL);

        // Older blueprints will have their name as the default category
        if( Category.EqualTo(FText::FromString(GetBlueprintObj()->GetName())) || Category.EqualTo(UEdGraphSchema_OdysseyBrush::VR_DefaultCategory) )
        {
            return LOCTEXT("DefaultCategory", "Default");
        }
        else
        {
            return Category;
        }
    }
    return FText();
}

void FOdysseyBrushDelegateActionDetails::OnCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
    if (InTextCommit == ETextCommit::OnEnter || InTextCommit == ETextCommit::OnUserMovedFocus)
    {
        if (UMulticastDelegateProperty* DelegateProperty = GetDelegateProperty())
        {
            // Remove excess whitespace and prevent categories with just spaces
            FText CategoryName = FText::TrimPrecedingAndTrailing(NewText);

            FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), DelegateProperty->GetFName(), NULL, CategoryName);
            check(MyOdysseyBrush.IsValid());
            FOdysseyBrushVarActionDetails::PopulateCategories(MyOdysseyBrush.Pin().Get(), CategorySource);
            MyOdysseyBrush.Pin()->ExpandCategory(CategoryName);
        }
    }
}

TSharedRef< ITableRow > FOdysseyBrushDelegateActionDetails::MakeCategoryViewWidget( TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable )
{
    return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
        [
            SNew(STextBlock) .Text(*Item.Get())
        ];
}

void FOdysseyBrushDelegateActionDetails::OnCategorySelectionChanged( TSharedPtr<FText> ProposedSelection, ESelectInfo::Type /*SelectInfo*/ )
{
    UMulticastDelegateProperty* DelegateProperty = GetDelegateProperty();
    if (DelegateProperty && ProposedSelection.IsValid())
    {
        FText NewCategory = *ProposedSelection.Get();

        FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), DelegateProperty->GetFName(), NULL, NewCategory);
        CategoryListView.Pin()->ClearSelection();
        CategoryComboButton.Pin()->SetIsOpen(false);
        MyOdysseyBrush.Pin()->ExpandCategory(NewCategory);
    }
}

void FOdysseyBrushDelegateActionDetails::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
    DetailsLayoutPtr = &DetailLayout;
    ObjectsBeingEdited = DetailsLayoutPtr->GetSelectedObjects();

    SetEntryNode();

    const UEdGraphSchema_OdysseyBrush* Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
    const FSlateFontInfo DetailFontInfo = IDetailLayoutBuilder::GetDetailFont();

    {
        IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Delegate", LOCTEXT("DelegateDetailsCategory", "Delegate"));
        Category.AddCustomRow( LOCTEXT("VariableToolTipLabel", "Tooltip") )
        .NameContent()
        [
            SNew(STextBlock)
                .Text( LOCTEXT("VariableToolTipLabel", "Tooltip") )
                .Font( DetailFontInfo )
        ]
        .ValueContent()
        [
            SNew(SEditableTextBox)
                .Text( this, &FOdysseyBrushDelegateActionDetails::OnGetTooltipText )
                .OnTextCommitted( this, &FOdysseyBrushDelegateActionDetails::OnTooltipTextCommitted)
                .Font( DetailFontInfo )
        ];

        FOdysseyBrushVarActionDetails::PopulateCategories(MyOdysseyBrush.Pin().Get(), CategorySource);
        TSharedPtr<SComboButton> NewComboButton;
        TSharedPtr<SListView<TSharedPtr<FText>>> NewListView;

        Category.AddCustomRow( LOCTEXT("CategoryLabel", "Category") )
        .NameContent()
        [
            SNew(STextBlock)
                .Text( LOCTEXT("CategoryLabel", "Category") )
                .Font( DetailFontInfo )
        ]
        .ValueContent()
        [
            SAssignNew(NewComboButton, SComboButton)
            .ContentPadding(FMargin(0,0,5,0))
            .IsEnabled(this, &FOdysseyBrushDelegateActionDetails::IsOdysseyBrushProperty)
            .ButtonContent()
            [
                SNew(SBorder)
                .BorderImage( FEditorStyle::GetBrush("NoBorder") )
                .Padding(FMargin(0, 0, 5, 0))
                [
                    SNew(SEditableTextBox)
                        .Text(this, &FOdysseyBrushDelegateActionDetails::OnGetCategoryText)
                        .OnTextCommitted(this, &FOdysseyBrushDelegateActionDetails::OnCategoryTextCommitted)
                        .SelectAllTextWhenFocused(true)
                        .RevertTextOnEscape(true)
                        .Font( DetailFontInfo )
                ]
            ]
            .MenuContent()
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .AutoHeight()
                .MaxHeight(400.0f)
                [
                    SAssignNew(NewListView, SListView<TSharedPtr<FText>>)
                        .ListItemsSource(&CategorySource)
                        .OnGenerateRow(this, &FOdysseyBrushDelegateActionDetails::MakeCategoryViewWidget)
                        .OnSelectionChanged(this, &FOdysseyBrushDelegateActionDetails::OnCategorySelectionChanged)
                ]
            ]
        ];

        CategoryComboButton = NewComboButton;
        CategoryListView = NewListView;
    }

    if (UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get())
    {
        IDetailCategoryBuilder& InputsCategory = DetailLayout.EditCategory("DelegateInputs", LOCTEXT("DelegateDetailsInputs", "Inputs"));
        TSharedRef<FOdysseyBrushGraphArgumentGroupLayout> InputArgumentGroup = MakeShareable(new FOdysseyBrushGraphArgumentGroupLayout(SharedThis(this), FunctionEntryNode));
        InputsCategory.AddCustomBuilder(InputArgumentGroup);

        TSharedRef<SHorizontalBox> InputsHeaderContentWidget = SNew(SHorizontalBox);
        TWeakPtr<SWidget> WeakInputsHeaderWidget = InputsHeaderContentWidget;
        InputsHeaderContentWidget->AddSlot()
        [
            SNew(SHorizontalBox)
        ];
        InputsHeaderContentWidget->AddSlot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(FEditorStyle::Get(), "RoundButton")
            .ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
            .ContentPadding(FMargin(2, 0))
            .OnClicked(this, &FOdysseyBrushDelegateActionDetails::OnAddNewInputClicked)
            .HAlign(HAlign_Right)
            .ToolTipText(LOCTEXT("DelegateNewOutputArgTooltip", "Create a new input argument"))
            .VAlign(VAlign_Center)
            .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("DelegateNewInputArg")))
            [
                SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(0, 1))
                [
                    SNew(SImage)
                    .Image(FEditorStyle::GetBrush("Plus"))
                ]
                +SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .AutoWidth()
                .Padding(FMargin(2, 0, 0, 0))
                [
                    SNew(STextBlock)
                    .Font(IDetailLayoutBuilder::GetDetailFontBold())
                    .Text(LOCTEXT("DelegateNewParameterInputArg", "New Parameter"))
                    .Visibility(this, &FOdysseyBrushDelegateActionDetails::OnGetSectionTextVisibility, WeakInputsHeaderWidget)
                    .ShadowOffset(FVector2D(1, 1))
                ]
            ]
        ];
        InputsCategory.HeaderContent(InputsHeaderContentWidget);

        CollectAvailibleSignatures();

        InputsCategory.AddCustomRow( LOCTEXT("CopySignatureFrom", "Copy signature from") )
        .NameContent()
        [
            SNew(STextBlock)
                .Text(LOCTEXT("CopySignatureFrom", "Copy signature from"))
                .Font( DetailFontInfo )
        ]
        .ValueContent()
        [
            SAssignNew(CopySignatureComboButton, STextComboBox)
                .OptionsSource(&FunctionsToCopySignatureFrom)
                .OnSelectionChanged(this, &FOdysseyBrushDelegateActionDetails::OnFunctionSelected)
        ];
    }
}

void FOdysseyBrushDelegateActionDetails::CollectAvailibleSignatures()
{
    FunctionsToCopySignatureFrom.Empty();
    if (UMulticastDelegateProperty* Property = GetDelegateProperty())
    {
        if (UClass* ScopeClass = Cast<UClass>(Property->GetOuterUField()))
        {
            for(TFieldIterator<UFunction> It(ScopeClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
            {
                UFunction* Func = *It;
                if (UEdGraphSchema_OdysseyBrush::FunctionCanBeUsedInDelegate(Func) && !UEdGraphSchema_OdysseyBrush::HasFunctionAnyOutputParameter(Func))
                {
                    TSharedPtr<FString> ItemData = MakeShareable(new FString(Func->GetName()));
                    FunctionsToCopySignatureFrom.Add(ItemData);
                }
            }

            // Sort the function list
            FunctionsToCopySignatureFrom.Sort([](const TSharedPtr<FString>& ElementA, const TSharedPtr<FString>& ElementB) -> bool
            {
                return *ElementA < *ElementB;
            });
        }
    }
}

void FOdysseyBrushDelegateActionDetails::OnFunctionSelected(TSharedPtr<FString> FunctionName, ESelectInfo::Type SelectInfo)
{
    UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get();
    UMulticastDelegateProperty* Property = GetDelegateProperty();
    UClass* ScopeClass = Property ? Cast<UClass>(Property->GetOuterUField()) : NULL;
    const UEdGraphSchema_OdysseyBrush* Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

    if (FunctionEntryNode && FunctionName.IsValid() && ScopeClass)
    {
        const FName Name( *(*FunctionName) );
        if (UFunction* NewSignature = ScopeClass->FindFunctionByName(Name))
        {
            const FScopedTransaction Transaction(LOCTEXT("CopySignature", "Copy Signature"));

            while (FunctionEntryNode->UserDefinedPins.Num())
            {
                TSharedPtr<FUserPinInfo> Pin = FunctionEntryNode->UserDefinedPins[0];
                FunctionEntryNode->RemoveUserDefinedPin(Pin);
            }

            for (TFieldIterator<UProperty> PropIt(NewSignature); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
            {
                UProperty* FuncParam = *PropIt;
                FEdGraphPinType TypeOut;
                Schema->ConvertPropertyToPinType(FuncParam, TypeOut);
                UEdGraphPin* EdGraphPin = FunctionEntryNode->CreateUserDefinedPin(FuncParam->GetFName(), TypeOut, EGPD_Output);
                ensure(EdGraphPin);
            }

            OnParamsChanged(FunctionEntryNode);
        }
    }
}

void FBaseOdysseyBrushGraphActionDetails::OnParamsChanged(UK2Node_EditablePinBase* TargetNode, bool bForceRefresh)
{
    UEdGraph* Graph = GetGraph();

    // TargetNode can be null, if we just removed the result node because there are no more out params
    if (TargetNode)
    {
        RegenerateInputsChildrenDelegate.ExecuteIfBound();
        RegenerateOutputsChildrenDelegate.ExecuteIfBound();

        // Reconstruct the entry/exit definition and recompile the blueprint to make sure the signature has changed before any fixups
        {
            const bool bCurDisableOrphanSaving = TargetNode->bDisableOrphanPinSaving;
            TargetNode->bDisableOrphanPinSaving = true;

            TargetNode->ReconstructNode();

            TargetNode->bDisableOrphanPinSaving = bCurDisableOrphanSaving;
        }

        const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();

        K2Schema->HandleParameterDefaultValueChanged(TargetNode);
    }
}

EVisibility FOdysseyBrushDelegateActionDetails::OnGetSectionTextVisibility(TWeakPtr<SWidget> RowWidget) const
{
    bool ShowText = RowWidget.Pin()->IsHovered();

    // If the row is currently hovered, or a menu is being displayed for a button, keep the button expanded.
    if (ShowText)
    {
        return EVisibility::SelfHitTestInvisible;
    }
    else
    {
        return EVisibility::Collapsed;
    }
}

struct FPinRenamedHelper : public FBasePinChangeHelper
{
    TSet<UBlueprint*> ModifiedBlueprints;
    TSet<UK2Node*> NodesToRename;

    virtual void EditMacroInstance(UK2Node_MacroInstance* MacroInstance, UBlueprint* OdysseyBrush) override
    {
        NodesToRename.Add(MacroInstance);
        if (OdysseyBrush)
        {
            ModifiedBlueprints.Add(OdysseyBrush);
        }
    }

    virtual void EditCallSite(UK2Node_CallFunction* CallSite, UBlueprint* OdysseyBrush) override
    {
        NodesToRename.Add(CallSite);
        if (OdysseyBrush)
        {
            ModifiedBlueprints.Add(OdysseyBrush);
        }
    }
};

bool FBaseOdysseyBrushGraphActionDetails::OnVerifyPinRename(UK2Node_EditablePinBase* InTargetNode, const FName InOldName, const FString& InNewName, FText& OutErrorMessage)
{
    // If the name is unchanged, allow the name
    if (InOldName.ToString() == InNewName)
    {
        return true;
    }

    if (InNewName.Len() > NAME_SIZE)
    {
        OutErrorMessage = FText::Format( LOCTEXT("PinNameTooLong", "The name you entered is too long. Names must be less than {0} characters"), FText::AsNumber( NAME_SIZE ) );
        return false;
    }

    if (InNewName == TEXT("None"))
    {
        OutErrorMessage = FText::Format( LOCTEXT("PinNameNone", "'None' is a reserved name"), FText::AsNumber( NAME_SIZE ) );
        return false;
    }

    if (InTargetNode)
    {
        // Check if the name conflicts with any of the other internal UFunction's property names (local variables and parameters).
        const UFunction* FoundFunction = FFunctionFromNodeHelper::FunctionFromNode(InTargetNode);
        const UProperty* ExistingProperty = FindField<const UProperty>(FoundFunction, *InNewName);
        if (ExistingProperty)
        {
            OutErrorMessage = LOCTEXT("ConflictsWithProperty", "Conflicts with another local variable or function parameter!");
            return false;
        }
    }
    return true;
}

bool FBaseOdysseyBrushGraphActionDetails::OnPinRenamed(UK2Node_EditablePinBase* TargetNode, const FName OldName, const FString& NewName)
{
    // Before changing the name, verify the name
    FText ErrorMessage;
    if(!OnVerifyPinRename(TargetNode, OldName, NewName, ErrorMessage))
    {
        return false;
    }

    UEdGraph* Graph = GetGraph();

    if (TargetNode)
    {
        FPinRenamedHelper PinRenamedHelper;

        const FScopedTransaction Transaction(LOCTEXT("RenameParam", "Rename Parameter"));

        TArray<UK2Node_EditablePinBase*> TerminalNodes = GatherAllResultNodes(FunctionResultNodePtr.Get());
        if (UK2Node_EditablePinBase* EntryNode = FunctionEntryNodePtr.Get())
        {
            TerminalNodes.Add(EntryNode);
        }
        for (UK2Node_EditablePinBase* TerminalNode : TerminalNodes)
        {
            TerminalNode->Modify();
            PinRenamedHelper.NodesToRename.Add(TerminalNode);
        }

        PinRenamedHelper.ModifiedBlueprints.Add(GetBlueprintObj());

        // GATHER
        PinRenamedHelper.Broadcast(GetBlueprintObj(), TargetNode, Graph);

        const FName NewFName = *NewName;

        // TEST
        for (UK2Node* NodeToRename : PinRenamedHelper.NodesToRename)
        {
            if (ERenamePinResult::ERenamePinResult_NameCollision == NodeToRename->RenameUserDefinedPin(OldName, NewFName, true))
            {
                return false;
            }
        }

        // UPDATE
        for (UK2Node* NodeToRename : PinRenamedHelper.NodesToRename)
        {
            NodeToRename->RenameUserDefinedPin(OldName, NewFName, false);
        }

        for (UK2Node_EditablePinBase* TerminalNode : TerminalNodes)
        {
            TSharedPtr<FUserPinInfo>* UDPinPtr = TerminalNode->UserDefinedPins.FindByPredicate([&](TSharedPtr<FUserPinInfo>& Pin)
            {
                return Pin.IsValid() && (Pin->PinName == OldName);
            });
            if (UDPinPtr)
            {
                (*UDPinPtr)->PinName = NewFName;
            }
        }
    }
    return true;
}

void FOdysseyBrushGraphActionDetails::SetEntryAndResultNodes()
{
    // Clear the entry and exit nodes to the graph
    FunctionEntryNodePtr = nullptr;
    FunctionResultNodePtr = nullptr;

    if (UEdGraph* NewTargetGraph = GetGraph())
    {
        FBlueprintEditorUtils::GetEntryAndResultNodes(NewTargetGraph, FunctionEntryNodePtr, FunctionResultNodePtr);
    }
    else if (UK2Node_EditablePinBase* Node = GetEditableNode())
    {
        FunctionEntryNodePtr = Node;
    }
}

UEdGraph* FBaseOdysseyBrushGraphActionDetails::GetGraph() const
{
    check(ObjectsBeingEdited.Num() > 0);

    if (ObjectsBeingEdited.Num() == 1)
    {
        UObject* const Object = ObjectsBeingEdited[0].Get();
        if (!Object)
        {
            return nullptr;
        }

        if (Object->IsA<UK2Node_Composite>())
        {
            return Cast<UK2Node_Composite>(Object)->BoundGraph;
        }
        else if (!Object->IsA<UK2Node_MacroInstance>() && (Object->IsA<UK2Node_Tunnel>() || Object->IsA<UK2Node_FunctionTerminator>()))
        {
            return Cast<UK2Node>(Object)->GetGraph();
        }
        else if (UK2Node_CallFunction* FunctionCall = Cast<UK2Node_CallFunction>(Object))
        {
            return FindObject<UEdGraph>(FunctionCall->GetBlueprint(), *(FunctionCall->FunctionReference.GetMemberName().ToString()));
        }
        else if (Object->IsA<UEdGraph>())
        {
            return Cast<UEdGraph>(Object);
        }
    }

    return nullptr;
}

UK2Node_EditablePinBase* FOdysseyBrushGraphActionDetails::GetEditableNode() const
{
    check(ObjectsBeingEdited.Num() > 0);

    if (ObjectsBeingEdited.Num() == 1)
    {
        UObject* const Object = ObjectsBeingEdited[0].Get();
        if (!Object)
        {
            return nullptr;
        }

        if (Object->IsA<UK2Node_CustomEvent>())
        {
            return Cast<UK2Node_CustomEvent>(Object);
        }
    }

    return nullptr;
}

UFunction* FOdysseyBrushGraphActionDetails::FindFunction() const
{
    UEdGraph* Graph = GetGraph();
    if(Graph)
    {
        if(UBlueprint* OdysseyBrush = FBlueprintEditorUtils::FindBlueprintForGraph(Graph))
        {
            UClass* Class = OdysseyBrush->SkeletonGeneratedClass;

            for (TFieldIterator<UFunction> FunctionIt(Class, EFieldIteratorFlags::IncludeSuper); FunctionIt; ++FunctionIt)
            {
                UFunction* Function = *FunctionIt;
                if (Function->GetName() == Graph->GetName())
                {
                    return Function;
                }
            }
        }
    }
    return NULL;
}

FKismetUserDeclaredFunctionMetadata* FOdysseyBrushGraphActionDetails::GetMetadataBlock() const
{
    UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get();
    if (UK2Node_FunctionEntry* TypedEntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode))
    {
        return &(TypedEntryNode->MetaData);
    }
    else if (UK2Node_Tunnel* TunnelNode = ExactCast<UK2Node_Tunnel>(FunctionEntryNode))
    {
        // Must be exactly a tunnel, not a macro instance
        return &(TunnelNode->MetaData);
    }

    return NULL;
}

FText FOdysseyBrushGraphActionDetails::OnGetTooltipText() const
{
    if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
    {
        return Metadata->ToolTip;
    }
    else
    {
        return LOCTEXT( "NoTooltip", "(None)" );
    }
}

void FOdysseyBrushGraphActionDetails::OnTooltipTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
    if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
    {
        Metadata->ToolTip = NewText;
        if (UFunction* Function = FindFunction())
        {
            Function->Modify();
            Function->SetMetaData(FBlueprintMetadata::MD_Tooltip, *NewText.ToString());
        }
    }
}

FText FOdysseyBrushGraphActionDetails::OnGetCategoryText() const
{
    if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
    {
        if( Metadata->Category.IsEmpty() || Metadata->Category.EqualTo(UEdGraphSchema_OdysseyBrush::VR_DefaultCategory) )
        {
            return LOCTEXT("DefaultCategory", "Default");
        }

        return Metadata->Category;
    }
    else
    {
        return LOCTEXT( "NoFunctionCategory", "(None)" );
    }
}

void FOdysseyBrushGraphActionDetails::OnCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
    if (InTextCommit == ETextCommit::OnEnter || InTextCommit == ETextCommit::OnUserMovedFocus)
    {
        // Remove excess whitespace and prevent categories with just spaces
        FText CategoryName = FText::TrimPrecedingAndTrailing(NewText);

        FBlueprintEditorUtils::SetBlueprintFunctionOrMacroCategory(GetGraph(), CategoryName);
        MyOdysseyBrush.Pin()->Refresh();
    }
}

void FOdysseyBrushGraphActionDetails::OnCategorySelectionChanged( TSharedPtr<FText> ProposedSelection, ESelectInfo::Type /*SelectInfo*/ )
{
    if(ProposedSelection.IsValid())
    {
        if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
        {
            FBlueprintEditorUtils::SetBlueprintFunctionOrMacroCategory(GetGraph(), *ProposedSelection.Get());
            MyOdysseyBrush.Pin()->Refresh();

            CategoryListView.Pin()->ClearSelection();
            CategoryComboButton.Pin()->SetIsOpen(false);
            MyOdysseyBrush.Pin()->ExpandCategory(*ProposedSelection.Get());
        }
    }
}

TSharedRef< ITableRow > FOdysseyBrushGraphActionDetails::MakeCategoryViewWidget( TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable )
{
    return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
        [
            SNew(STextBlock) .Text(*Item.Get())
        ];
}

FText FOdysseyBrushGraphActionDetails::OnGetKeywordsText() const
{
    FText ResultKeywords;
    if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
    {
        ResultKeywords = Metadata->Keywords;
    }
    return ResultKeywords;
}

void FOdysseyBrushGraphActionDetails::OnKeywordsTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
    if (InTextCommit == ETextCommit::OnEnter || InTextCommit == ETextCommit::OnUserMovedFocus)
    {
        if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
        {
            // Remove excess whitespace and prevent keywords with just spaces
            FText Keywords = FText::TrimPrecedingAndTrailing(NewText);

            if (!Keywords.EqualTo(Metadata->Keywords))
            {
                Metadata->Keywords = Keywords;

                if (UFunction* Function = FindFunction())
                {
                    Function->Modify();
                    Function->SetMetaData(FBlueprintMetadata::MD_FunctionKeywords, *Keywords.ToString());
                }
                OnParamsChanged(GetFunctionEntryNode().Get(), true);
                FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprintObj());
            }
        }
    }
}

FText FOdysseyBrushGraphActionDetails::OnGetCompactNodeTitleText() const
{
    FText ResultKeywords;
    if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
    {
        ResultKeywords = Metadata->CompactNodeTitle;
    }
    return ResultKeywords;
}

void FOdysseyBrushGraphActionDetails::OnCompactNodeTitleTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
    if (InTextCommit == ETextCommit::OnEnter || InTextCommit == ETextCommit::OnUserMovedFocus)
    {
        if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
        {
            // Remove excess whitespace and prevent CompactNodeTitle with just spaces
            FText CompactNodeTitle = FText::TrimPrecedingAndTrailing(NewText);

            if (!CompactNodeTitle.EqualTo(Metadata->CompactNodeTitle))
            {
                Metadata->CompactNodeTitle = CompactNodeTitle;

                if (UFunction* Function = FindFunction())
                {
                    Function->Modify();

                    if (CompactNodeTitle.IsEmpty())
                    {
                        // Remove the metadata from the function, empty listings will still display the node as Compact
                        Function->RemoveMetaData(FBlueprintMetadata::MD_FunctionKeywords);
                    }
                    else
                    {
                        Function->SetMetaData(FBlueprintMetadata::MD_CompactNodeTitle, *CompactNodeTitle.ToString());
                    }
                }
                OnParamsChanged(GetFunctionEntryNode().Get(), true);
                FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprintObj());
            }
        }
    }
}

FText FOdysseyBrushGraphActionDetails::AccessSpecifierProperName( uint32 AccessSpecifierFlag ) const
{
    switch(AccessSpecifierFlag)
    {
    case FUNC_Public:
        return LOCTEXT( "Public", "Public" );
    case FUNC_Private:
        return LOCTEXT( "Private", "Private" );
    case FUNC_Protected:
        return LOCTEXT( "Protected", "Protected" );
    case 0:
        return LOCTEXT( "Unknown", "Unknown" ); // Default?
    }
    return LOCTEXT( "Error", "Error" );
}

FText FOdysseyBrushGraphActionDetails::ReplicationSpecifierProperName( uint32 ReplicationSpecifierFlag ) const
{
    switch(ReplicationSpecifierFlag)
    {
    case FUNC_NetMulticast:
        return LOCTEXT( "MulticastDropDown", "Multicast" );
    case FUNC_NetServer:
        return LOCTEXT( "ServerDropDown", "Run on Server" );
    case FUNC_NetClient:
        return LOCTEXT( "ClientDropDown", "Run on owning Client" );
    case 0:
        return LOCTEXT( "NotReplicatedDropDown", "Not Replicated" );
    }
    return LOCTEXT( "Error", "Error" );
}

TSharedRef<ITableRow> FOdysseyBrushGraphActionDetails::HandleGenerateRowAccessSpecifier( TSharedPtr<FAccessSpecifierLabel> SpecifierName, const TSharedRef<STableViewBase>& OwnerTable )
{
    return SNew(STableRow< TSharedPtr<FAccessSpecifierLabel> >, OwnerTable)
        .Content()
        [
            SNew( STextBlock )
                .Text( SpecifierName.IsValid() ? SpecifierName->LocalizedName : FText::GetEmpty() )
        ];
}

FText FOdysseyBrushGraphActionDetails::GetCurrentAccessSpecifierName() const
{
    uint32 AccessSpecifierFlag = 0;
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    if(UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode))
    {
        AccessSpecifierFlag = FUNC_AccessSpecifiers & EntryNode->GetFunctionFlags();
    }
    else if(UK2Node_CustomEvent* CustomEventNode = Cast<UK2Node_CustomEvent>(FunctionEntryNode))
    {
        AccessSpecifierFlag = FUNC_AccessSpecifiers & CustomEventNode->FunctionFlags;
    }
    return AccessSpecifierProperName( AccessSpecifierFlag );
}

bool FOdysseyBrushGraphActionDetails::IsAccessSpecifierVisible() const
{
    bool bSupportedType = false;
    bool bIsEditable = false;
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    if(FunctionEntryNode)
    {
        UBlueprint* OdysseyBrush = FunctionEntryNode->GetBlueprint();
        const bool bIsInterface = FBlueprintEditorUtils::IsInterfaceBlueprint(OdysseyBrush);

        bSupportedType = !bIsInterface && (FunctionEntryNode->IsA<UK2Node_FunctionEntry>() || FunctionEntryNode->IsA<UK2Node_Event>());
        bIsEditable = FunctionEntryNode->IsEditable();
    }
    return bSupportedType && bIsEditable;
}

void FOdysseyBrushGraphActionDetails::OnAccessSpecifierSelected( TSharedPtr<FAccessSpecifierLabel> SpecifierName, ESelectInfo::Type SelectInfo )
{
    if(AccessSpecifierComboButton.IsValid())
    {
        AccessSpecifierComboButton->SetIsOpen(false);
    }

    UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get();
    if(FunctionEntryNode && SpecifierName.IsValid())
    {
        const FScopedTransaction Transaction( LOCTEXT( "ChangeAccessSpecifier", "Change Access Specifier" ) );

        FunctionEntryNode->Modify();
        UFunction* Function = FindFunction();
        if(Function)
        {
            Function->Modify();
        }

        const EFunctionFlags ClearAccessSpecifierMask = ~FUNC_AccessSpecifiers;
        if(UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode))
        {
            int32 ExtraFlags = EntryNode->GetExtraFlags();
            ExtraFlags &= ClearAccessSpecifierMask;
            ExtraFlags |= SpecifierName->SpecifierFlag;
            EntryNode->SetExtraFlags(ExtraFlags);
        }
        else if(UK2Node_Event* EventNode = Cast<UK2Node_Event>(FunctionEntryNode))
        {
            EventNode->FunctionFlags &= ClearAccessSpecifierMask;
            EventNode->FunctionFlags |= SpecifierName->SpecifierFlag;
        }
        if(Function)
        {
            Function->FunctionFlags &= ClearAccessSpecifierMask;
            Function->FunctionFlags |= SpecifierName->SpecifierFlag;
        }

        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());
    }
}

bool FOdysseyBrushGraphActionDetails::GetInstanceColorVisibility() const
{
    // Hide the color editor if it's a top level function declaration.
    // Show it if we're editing a collapsed graph or macro
    UEdGraph* Graph = GetGraph();
    if (Graph)
    {
        const UBlueprint* OdysseyBrush = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
        if (OdysseyBrush)
        {
            const bool bIsTopLevelFunctionGraph = OdysseyBrush->FunctionGraphs.Contains(Graph);
            const bool bIsTopLevelMacroGraph = OdysseyBrush->MacroGraphs.Contains(Graph);
            const bool bIsMacroGraph = OdysseyBrush->BlueprintType == BPTYPE_MacroLibrary;
            return ((bIsMacroGraph || bIsTopLevelMacroGraph) || !bIsTopLevelFunctionGraph);
        }

    }

    return false;
}

FLinearColor FOdysseyBrushGraphActionDetails::GetNodeTitleColor() const
{
    if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
    {
        return Metadata->InstanceTitleColor;
    }
    else
    {
        return FLinearColor::White;
    }
}

FReply FOdysseyBrushGraphActionDetails::ColorBlock_OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        if (FKismetUserDeclaredFunctionMetadata* Metadata = GetMetadataBlock())
        {
            TArray<FLinearColor*> LinearColorArray;
            LinearColorArray.Add(&(Metadata->InstanceTitleColor));

            FColorPickerArgs PickerArgs;
            PickerArgs.bIsModal = true;
            PickerArgs.ParentWidget = ColorBlock;
            PickerArgs.DisplayGamma = TAttribute<float>::Create( TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma) );
            PickerArgs.LinearColorArray = &LinearColorArray;

            OpenColorPicker(PickerArgs);
        }

        return FReply::Handled();
    }
    else
    {
        return FReply::Unhandled();
    }
}

bool FOdysseyBrushGraphActionDetails::IsCustomEvent() const
{
    return (NULL != Cast<UK2Node_CustomEvent>(FunctionEntryNodePtr.Get()));
}

void FOdysseyBrushGraphActionDetails::OnIsReliableReplicationFunctionModified(const ECheckBoxState NewCheckedState)
{
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    UK2Node_CustomEvent* CustomEvent = Cast<UK2Node_CustomEvent>(FunctionEntryNode);
    if( CustomEvent )
    {
        if (NewCheckedState == ECheckBoxState::Checked)
        {
            if (UK2Node_FunctionEntry* TypedEntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode))
            {
                TypedEntryNode->AddExtraFlags(FUNC_NetReliable);
            }
            if (UK2Node_CustomEvent * CustomEventNode = Cast<UK2Node_CustomEvent>(FunctionEntryNode))
            {
                CustomEventNode->FunctionFlags |= FUNC_NetReliable;
            }
        }
        else
        {
            if (UK2Node_FunctionEntry* TypedEntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode))
            {
                TypedEntryNode->ClearExtraFlags(FUNC_NetReliable);
            }
            if (UK2Node_CustomEvent * CustomEventNode = Cast<UK2Node_CustomEvent>(FunctionEntryNode))
            {
                CustomEventNode->FunctionFlags &= ~FUNC_NetReliable;
            }
        }

        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());
    }
}

ECheckBoxState FOdysseyBrushGraphActionDetails::GetIsReliableReplicatedFunction() const
{
    const UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    const UK2Node_CustomEvent* CustomEvent = Cast<const UK2Node_CustomEvent>(FunctionEntryNode);
    if(!CustomEvent)
    {
        return ECheckBoxState::Undetermined;
    }

    uint32 const NetReliableMask = (FUNC_Net | FUNC_NetReliable);
    if ((CustomEvent->GetNetFlags() & NetReliableMask) == NetReliableMask)
    {
        return ECheckBoxState::Checked;
    }

    return ECheckBoxState::Unchecked;
}

bool FOdysseyBrushGraphActionDetails::IsPureFunctionVisible() const
{
    bool bSupportedType = false;
    bool bIsEditable = false;
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    if(FunctionEntryNode)
    {
        UBlueprint* OdysseyBrush = FunctionEntryNode->GetBlueprint();
        const bool bIsInterface = FBlueprintEditorUtils::IsInterfaceBlueprint(OdysseyBrush);

        bSupportedType = !bIsInterface && FunctionEntryNode->IsA<UK2Node_FunctionEntry>();
        bIsEditable = FunctionEntryNode->IsEditable();
    }
    return bSupportedType && bIsEditable;
}

void FOdysseyBrushGraphActionDetails::OnIsPureFunctionModified( const ECheckBoxState NewCheckedState )
{
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    UFunction* Function = FindFunction();
    UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode);
    if (EntryNode && Function)
    {
        const FScopedTransaction Transaction( LOCTEXT( "ChangePure", "Change Pure" ) );
        EntryNode->Modify();
        Function->Modify();

        //set flags on function entry node also
        Function->FunctionFlags ^= FUNC_BlueprintPure;
        EntryNode->SetExtraFlags(EntryNode->GetExtraFlags() ^ FUNC_BlueprintPure);
        OnParamsChanged(FunctionEntryNode);
    }
}

ECheckBoxState FOdysseyBrushGraphActionDetails::GetIsPureFunction() const
{
    UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get();
    UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode);
    if(!EntryNode)
    {
        return ECheckBoxState::Undetermined;
    }
    return (EntryNode->GetFunctionFlags() & FUNC_BlueprintPure) ? ECheckBoxState::Checked :  ECheckBoxState::Unchecked;
}

bool FOdysseyBrushGraphActionDetails::IsConstFunctionVisible() const
{
    bool bSupportedType = false;
    bool bIsEditable = false;
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    if(FunctionEntryNode)
    {
        UBlueprint* OdysseyBrush = FunctionEntryNode->GetBlueprint();

        bSupportedType = FunctionEntryNode->IsA<UK2Node_FunctionEntry>();
        bIsEditable = FunctionEntryNode->IsEditable();
    }
    return bSupportedType && bIsEditable;
}

void FOdysseyBrushGraphActionDetails::OnIsConstFunctionModified( const ECheckBoxState NewCheckedState )
{
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();
    UFunction* Function = FindFunction();
    UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode);
    if(EntryNode && Function)
    {
        const FScopedTransaction Transaction( LOCTEXT( "ChangeConst", "Change Const" ) );
        EntryNode->Modify();
        Function->Modify();

        //set flags on function entry node also
        Function->FunctionFlags ^= FUNC_Const;
        EntryNode->SetExtraFlags(EntryNode->GetExtraFlags() ^ FUNC_Const);
        OnParamsChanged(FunctionEntryNode);
    }
}

ECheckBoxState FOdysseyBrushGraphActionDetails::GetIsConstFunction() const
{
    UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get();
    UK2Node_FunctionEntry* EntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryNode);
    if(!EntryNode)
    {
        return ECheckBoxState::Undetermined;
    }
    return (EntryNode->GetFunctionFlags() & FUNC_Const) ? ECheckBoxState::Checked :  ECheckBoxState::Unchecked;
}

FReply FBaseOdysseyBrushGraphActionDetails::OnAddNewInputClicked()
{
    UK2Node_EditablePinBase * FunctionEntryNode = FunctionEntryNodePtr.Get();

    if( FunctionEntryNode )
    {
        FScopedTransaction Transaction( LOCTEXT( "AddInParam", "Add In Parameter" ) );
        FunctionEntryNode->Modify();

        FEdGraphPinType PinType = MyOdysseyBrush.Pin()->GetLastFunctionPinTypeUsed();

        // Make sure that if this is an exec node we are allowed one.
        if ((PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Exec) && (!FunctionEntryNode->CanModifyExecutionWires()))
        {
            MyOdysseyBrush.Pin()->ResetLastPinType();
            PinType = MyOdysseyBrush.Pin()->GetLastFunctionPinTypeUsed();
        }
        const FName NewPinName = TEXT("NewParam");
        if (FunctionEntryNode->CreateUserDefinedPin(NewPinName, PinType, EGPD_Output))
        {
            OnParamsChanged(FunctionEntryNode, true);
        }
        else
        {
            Transaction.Cancel();
        }
    }

    return FReply::Handled();
}

EVisibility FOdysseyBrushGraphActionDetails::GetAddNewInputOutputVisibility() const
{
    UK2Node_EditablePinBase* FunctionEntryNode = FunctionEntryNodePtr.Get();
    if (FunctionEntryNodePtr.IsValid())
    {
        if(UEdGraph* Graph = FunctionEntryNode->GetGraph())
        {
            // Math expression graphs are read only, do not allow adding or removing of pins
            if(Cast<UK2Node_MathExpression>(Graph->GetOuter()))
            {
                return EVisibility::Collapsed;
            }
        }
    }
    return EVisibility::Visible;
}

EVisibility FOdysseyBrushGraphActionDetails::OnGetSectionTextVisibility(TWeakPtr<SWidget> RowWidget) const
{
    bool ShowText = RowWidget.Pin()->IsHovered();

    // If the row is currently hovered, or a menu is being displayed for a button, keep the button expanded.
    if (ShowText)
    {
        return EVisibility::SelfHitTestInvisible;
    }
    else
    {
        return EVisibility::Collapsed;
    }
}

FReply FOdysseyBrushGraphActionDetails::OnAddNewOutputClicked()
{
    FScopedTransaction Transaction( LOCTEXT( "AddOutParam", "Add Out Parameter" ) );

    GetBlueprintObj()->Modify();
    GetGraph()->Modify();
    UK2Node_EditablePinBase* EntryPin = FunctionEntryNodePtr.Get();
    EntryPin->Modify();
    for (int32 iPin = 0; iPin < EntryPin->Pins.Num() ; iPin++)
    {
        EntryPin->Pins[iPin]->Modify();
    }

    UK2Node_EditablePinBase* PreviousResultNode = FunctionResultNodePtr.Get();

    AttemptToCreateResultNode();

    UK2Node_EditablePinBase* FunctionResultNode = FunctionResultNodePtr.Get();
    if( FunctionResultNode )
    {
        FEdGraphPinType PinType = MyOdysseyBrush.Pin()->GetLastFunctionPinTypeUsed();
        PinType.bIsReference = false;
        // Make sure that if this is an exec node we are allowed one.
        if ((PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Exec) && (!FunctionResultNode->CanModifyExecutionWires()))
        {
            MyOdysseyBrush.Pin()->ResetLastPinType();
            PinType = MyOdysseyBrush.Pin()->GetLastFunctionPinTypeUsed();
        }

        const FName NewPinName = FunctionResultNode->CreateUniquePinName(TEXT("NewParam"));
        TArray<UK2Node_EditablePinBase*> TargetNodes = GatherAllResultNodes(FunctionResultNode);
        bool bAllChanged = TargetNodes.Num() > 0;
        for (UK2Node_EditablePinBase* Node : TargetNodes)
        {
            Node->Modify();
            UEdGraphPin* NewPin = Node->CreateUserDefinedPin(NewPinName, PinType, EGPD_Input, false);
            bAllChanged &= (nullptr != NewPin);

            if (bAllChanged)
            {
                OnParamsChanged(Node, true);
            }
            else
            {
                break;
            }
        }
        if (!bAllChanged)
        {
            Transaction.Cancel();
        }

        if (!PreviousResultNode)
        {
            DetailsLayoutPtr->ForceRefreshDetails();
        }
    }
    else
    {
        Transaction.Cancel();
    }

    return FReply::Handled();
}



void FOdysseyBrushInterfaceLayout::GenerateHeaderRowContent( FDetailWidgetRow& NodeRow )
{
    NodeRow
    [
        SNew(STextBlock)
            .Text( bShowsInheritedInterfaces ?
            LOCTEXT("OdysseyBrushInheritedInterfaceTitle", "Inherited Interfaces") :
            LOCTEXT("OdysseyBrushImplementedInterfaceTitle", "Implemented Interfaces") )
            .Font( IDetailLayoutBuilder::GetDetailFont() )
    ];
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FOdysseyBrushInterfaceLayout::GenerateChildContent( IDetailChildrenBuilder& ChildrenBuilder )
{
    UBlueprint* OdysseyBrush = GlobalOptionsDetailsPtr.Pin()->GetBlueprintObj();
    check(OdysseyBrush);

    TArray<FInterfaceName> Interfaces;

    if (!bShowsInheritedInterfaces)
    {
        // Generate a list of interfaces already implemented
        for (const FBPInterfaceDescription& ImplementedInterface : OdysseyBrush->ImplementedInterfaces)
        {
            if (const TSubclassOf<UInterface> Interface = ImplementedInterface.Interface)
            {
                Interfaces.AddUnique(FInterfaceName(Interface->GetFName(), Interface->GetDisplayNameText()));
            }
        }
    }
    else
    {
        // Generate a list of interfaces implemented by classes this blueprint inherited from
        UClass* OdysseyBrushParent = OdysseyBrush->ParentClass;
        while (OdysseyBrushParent)
        {
            for (TArray<FImplementedInterface>::TIterator It(OdysseyBrushParent->Interfaces); It; ++It)
            {
                FImplementedInterface& CurrentInterface = *It;
                if( CurrentInterface.Class )
                {
                    Interfaces.Add(FInterfaceName(CurrentInterface.Class->GetFName(), CurrentInterface.Class->GetDisplayNameText()));
                }
            }
            OdysseyBrushParent = OdysseyBrushParent->GetSuperClass();
        }
    }

    for (int32 i = 0; i < Interfaces.Num(); ++i)
    {
        TSharedPtr<SHorizontalBox> Box;
        ChildrenBuilder.AddCustomRow( LOCTEXT( "OdysseyBrushInterfaceValue", "Interface Value" ) )
        [
            SAssignNew(Box, SHorizontalBox)
            +SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                    .Text(Interfaces[i].DisplayText)
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
            ]
        ];

        // See if we need to add a button for opening this interface
        if (!bShowsInheritedInterfaces)
        {
            UBlueprintGeneratedClass* Class = Cast<UBlueprintGeneratedClass>(*OdysseyBrush->ImplementedInterfaces[i].Interface);
            if (Class)
            {
                TWeakObjectPtr<UObject> Asset = Class->ClassGeneratedBy;

                const TSharedRef<SWidget> BrowseButton = PropertyCustomizationHelpers::MakeBrowseButton(FSimpleDelegate::CreateSP(this, &FOdysseyBrushInterfaceLayout::OnBrowseToInterface, Asset));
                BrowseButton->SetToolTipText( LOCTEXT("OdysseyBrushInterfaceBrowseTooltip", "Opens this interface") );

                Box->AddSlot()
                .AutoWidth()
                .Padding(2.0f, 0.0f)
                [
                    BrowseButton
                ];
            }
        }

        if (!bShowsInheritedInterfaces)
        {
            Box->AddSlot()
            .AutoWidth()
            [
                PropertyCustomizationHelpers::MakeClearButton(FSimpleDelegate::CreateSP(this, &FOdysseyBrushInterfaceLayout::OnRemoveInterface, Interfaces[i]))
            ];
        }
    }

    // Add message if no interfaces are being used
    if (Interfaces.Num() == 0)
    {
        ChildrenBuilder.AddCustomRow(LOCTEXT("OdysseyBrushInterfaceValue", "Interface Value"))
        [
            SNew(STextBlock)
            .Text(LOCTEXT("NoOdysseyBrushInterface", "No Interfaces"))
            .Font(IDetailLayoutBuilder::GetDetailFontItalic())
        ];
    }

    if (!bShowsInheritedInterfaces)
    {
        ChildrenBuilder.AddCustomRow( LOCTEXT( "OdysseyBrushAddInterface", "Add Interface" ) )
        [
            SNew(SBox)
            .HAlign(HAlign_Right)
            [
                SAssignNew(AddInterfaceComboButton, SComboButton)
                .ButtonContent()
                [
                    SNew(STextBlock)
                        .Text(LOCTEXT("OdysseyBrushAddInterfaceButton", "Add"))
                ]
                .OnGetMenuContent(this, &FOdysseyBrushInterfaceLayout::OnGetAddInterfaceMenuContent)
            ]
        ];
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FOdysseyBrushInterfaceLayout::OnBrowseToInterface(TWeakObjectPtr<UObject> Asset)
{
    if (Asset.IsValid())
    {
        FAssetEditorManager::Get().OpenEditorForAsset(Asset.Get());
    }
}

void FOdysseyBrushInterfaceLayout::OnRemoveInterface(FInterfaceName InterfaceName)
{
    UBlueprint* OdysseyBrush = GlobalOptionsDetailsPtr.Pin()->GetBlueprintObj();
    check(OdysseyBrush);

    const EAppReturnType::Type DialogReturn = FMessageDialog::Open(EAppMsgType::YesNoCancel, NSLOCTEXT("UnrealEd", "TransferInterfaceFunctionsToOdysseyBrush", "Would you like to transfer the interface functions to be part of your blueprint?"));

    if (DialogReturn == EAppReturnType::Cancel)
    {
        // We canceled!
        return;
    }
    const FName InterfaceFName = InterfaceName.Name;

    // Close all graphs that are about to be removed
    TArray<UEdGraph*> Graphs;
    FBlueprintEditorUtils::GetInterfaceGraphs(OdysseyBrush, InterfaceFName, Graphs);
    for( TArray<UEdGraph*>::TIterator GraphIt(Graphs); GraphIt; ++GraphIt )
    {
        GlobalOptionsDetailsPtr.Pin()->GetBlueprintEditorPtr().Pin()->CloseDocumentTab(*GraphIt);
    }

    // Do the work of actually removing the interface
    FBlueprintEditorUtils::RemoveInterface(OdysseyBrush, InterfaceFName, DialogReturn == EAppReturnType::Yes);

    RegenerateChildrenDelegate.ExecuteIfBound();

    OnRefreshInDetailsView();
}

void FOdysseyBrushInterfaceLayout::OnClassPicked(UClass* PickedClass)
{
    if (AddInterfaceComboButton.IsValid())
    {
        AddInterfaceComboButton->SetIsOpen(false);
    }

    if (PickedClass)
    {
        UBlueprint* OdysseyBrush = GlobalOptionsDetailsPtr.Pin()->GetBlueprintObj();
        check(OdysseyBrush);

        FBlueprintEditorUtils::ImplementNewInterface(OdysseyBrush, PickedClass->GetFName());

        RegenerateChildrenDelegate.ExecuteIfBound();
    }

    OnRefreshInDetailsView();
}

TSharedRef<SWidget> FOdysseyBrushInterfaceLayout::OnGetAddInterfaceMenuContent()
{
    UBlueprint* OdysseyBrush = GlobalOptionsDetailsPtr.Pin()->GetBlueprintObj();

    TArray<UBlueprint*> Blueprints;
    Blueprints.Add(OdysseyBrush);
    TSharedRef<SWidget> ClassPicker = FBlueprintEditorUtils::ConstructBlueprintInterfaceClassPicker(Blueprints, FOnClassPicked::CreateSP(this, &FOdysseyBrushInterfaceLayout::OnClassPicked));
    return
        SNew(SBorder)
        .BorderImage(FEditorStyle::GetBrush("Menu.Background"))
        [
            // Achieving fixed width by nesting items within a fixed width box.
            SNew(SBox)
            .WidthOverride(350.0f)
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .MaxHeight(400.0f)
                .AutoHeight()
                [
                    ClassPicker
                ]
            ]
        ];
}

void FOdysseyBrushInterfaceLayout::OnRefreshInDetailsView()
{
    TSharedPtr<SKismetInspector> Inspector = GlobalOptionsDetailsPtr.Pin()->GetBlueprintEditorPtr().Pin()->GetInspector();
    UBlueprint* OdysseyBrush = GlobalOptionsDetailsPtr.Pin()->GetBlueprintObj();
    check(OdysseyBrush);

    // Show details for the OdysseyBrush instance we're editing
    Inspector->ShowDetailsForSingleObject(OdysseyBrush);
}

UBlueprint* FOdysseyBrushGlobalOptionsDetails::GetBlueprintObj() const
{
    if(OdysseyBrushEditorPtr.IsValid())
    {
        return OdysseyBrushEditorPtr.Pin()->GetBlueprintObj();
    }

    return NULL;
}

FText FOdysseyBrushGlobalOptionsDetails::GetParentClassName() const
{
    const UBlueprint* OdysseyBrush = GetBlueprintObj();
    const UClass* ParentClass = OdysseyBrush ? OdysseyBrush->ParentClass : NULL;
    return ParentClass ? ParentClass->GetDisplayNameText() : FText::FromName(NAME_None);
}

bool FOdysseyBrushGlobalOptionsDetails::CanReparent() const
{
    return OdysseyBrushEditorPtr.IsValid() && OdysseyBrushEditorPtr.Pin()->ReparentOdysseyBrush_IsVisible();
}

TSharedRef<SWidget> FOdysseyBrushGlobalOptionsDetails::GetParentClassMenuContent()
{
    TArray<UBlueprint*> Blueprints;
    Blueprints.Add(GetBlueprintObj());
    TSharedRef<SWidget> ClassPicker = FBlueprintEditorUtils::ConstructBlueprintParentClassPicker(Blueprints, FOnClassPicked::CreateSP(this, &FOdysseyBrushGlobalOptionsDetails::OnClassPicked));

    return
        SNew(SBorder)
        .BorderImage(FEditorStyle::GetBrush("Menu.Background"))
        [
            // Achieving fixed width by nesting items within a fixed width box.
            SNew(SBox)
            .WidthOverride(350.0f)
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .MaxHeight(400.0f)
                .AutoHeight()
                [
                    ClassPicker
                ]
            ]
        ];
}

void FOdysseyBrushGlobalOptionsDetails::OnClassPicked(UClass* PickedClass)
{
    ParentClassComboButton->SetIsOpen(false);
    if(OdysseyBrushEditorPtr.IsValid())
    {
        OdysseyBrushEditorPtr.Pin()->ReparentOdysseyBrush_NewParentChosen(PickedClass);
    }

    check(OdysseyBrushEditorPtr.IsValid());
    TSharedPtr<SKismetInspector> Inspector = OdysseyBrushEditorPtr.Pin()->GetInspector();
    // Show details for the OdysseyBrush instance we're editing
    Inspector->ShowDetailsForSingleObject(GetBlueprintObj());
}

bool FOdysseyBrushGlobalOptionsDetails::CanDeprecateOdysseyBrush() const
{
    if (UBlueprint* OdysseyBrush = GetBlueprintObj())
    {
        // If the parent is deprecated, we cannot modify deprecation on this OdysseyBrush
        if (OdysseyBrush->ParentClass && OdysseyBrush->ParentClass->HasAnyClassFlags(CLASS_Deprecated))
        {
            return false;
        }

        return true;
    }

    return false;
}

void FOdysseyBrushGlobalOptionsDetails::OnDeprecateOdysseyBrush(ECheckBoxState InCheckState)
{
    GetBlueprintObj()->bDeprecate = InCheckState == ECheckBoxState::Checked? true : false;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());
}

ECheckBoxState FOdysseyBrushGlobalOptionsDetails::IsDeprecatedOdysseyBrush() const
{
    if (UBlueprint* OdysseyBrush = GetBlueprintObj())
    {
        return OdysseyBrush->bDeprecate ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
    }
    return ECheckBoxState::Unchecked;
}

FText FOdysseyBrushGlobalOptionsDetails::GetDeprecatedTooltip() const
{
    if(CanDeprecateOdysseyBrush())
    {
        return LOCTEXT("DeprecateOdysseyBrushTooltip", "Deprecate the OdysseyBrush and all child Blueprints to make it no longer placeable in the World nor child classes created from it.");
    }

    return LOCTEXT("DisabledDeprecateOdysseyBrushTooltip", "This OdysseyBrush is deprecated because of a parent, it is not possible to remove deprecation from it!");
}

void FOdysseyBrushGlobalOptionsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    const UBlueprint* OdysseyBrush = GetBlueprintObj();
    if(OdysseyBrush != NULL)
    {
        // Hide any properties that aren't included in the "Option" category
        for (TFieldIterator<UProperty> PropertyIt(OdysseyBrush->GetClass(), EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
        {
            UProperty* Property = *PropertyIt;
            const FString& Category = Property->GetMetaData(TEXT("Category"));

            if (Category != TEXT("OdysseyBrushOptions") && Category != TEXT("ClassOptions"))
            {
                DetailLayout.HideProperty(DetailLayout.GetProperty(Property->GetFName()));
            }
        }

        // Display the parent class and set up the menu for reparenting
        IDetailCategoryBuilder& Category = DetailLayout.EditCategory("ClassOptions", LOCTEXT("ClassOptions", "Class Options"));
        Category.AddCustomRow( LOCTEXT("ClassOptions", "Class Options") )
        .NameContent()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyBrushDetails_ParentClass", "Parent Class"))
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ]
        .ValueContent()
        [
            SAssignNew(ParentClassComboButton, SComboButton)
            .IsEnabled(this, &FOdysseyBrushGlobalOptionsDetails::CanReparent)
            .OnGetMenuContent(this, &FOdysseyBrushGlobalOptionsDetails::GetParentClassMenuContent)
            .ButtonContent()
            [
                SNew(STextBlock)
                .Text(this, &FOdysseyBrushGlobalOptionsDetails::GetParentClassName)
                .Font(IDetailLayoutBuilder::GetDetailFont())
            ]
        ];

        const bool bIsInterfaceBP = FBlueprintEditorUtils::IsInterfaceBlueprint(OdysseyBrush);
        const bool bIsMacroLibrary = OdysseyBrush->BlueprintType == BPTYPE_MacroLibrary;
        const bool bIsLevelScriptBP = FBlueprintEditorUtils::IsLevelScriptBlueprint(OdysseyBrush);
        const bool bIsFunctionLibrary = OdysseyBrush->BlueprintType == BPTYPE_FunctionLibrary;
        const bool bSupportsInterfaces = !bIsInterfaceBP && !bIsMacroLibrary && !bIsFunctionLibrary;

        if (bSupportsInterfaces)
        {
            // Interface details customization
            IDetailCategoryBuilder& InterfacesCategory = DetailLayout.EditCategory("Interfaces", LOCTEXT("OdysseyBrushInterfacesDetailsCategory", "Interfaces"));

            TSharedRef<FOdysseyBrushInterfaceLayout> InterfaceLayout = MakeShareable(new FOdysseyBrushInterfaceLayout(SharedThis(this), false));
            InterfacesCategory.AddCustomBuilder(InterfaceLayout);

            TSharedRef<FOdysseyBrushInterfaceLayout> InheritedInterfaceLayout = MakeShareable(new FOdysseyBrushInterfaceLayout(SharedThis(this), true));
            InterfacesCategory.AddCustomBuilder(InheritedInterfaceLayout);
        }

        // Hide the bDeprecate, we override the functionality.
        static FName DeprecatePropName(TEXT("bDeprecate"));
        DetailLayout.HideProperty(DetailLayout.GetProperty(DeprecatePropName));

        // Hide the experimental CompileMode setting (if not enabled)
        const UBlueprintEditorSettings* EditorSettings = GetDefault<UBlueprintEditorSettings>();
        if (EditorSettings && !EditorSettings->bAllowExplicitImpureNodeDisabling)
        {
            static FName CompileModePropertyName(TEXT("CompileMode"));
            DetailLayout.HideProperty(DetailLayout.GetProperty(CompileModePropertyName));
        }

        // Hide 'run on drag' for LevelBP
        if (bIsLevelScriptBP)
        {
            static FName RunOnDragPropName(TEXT("bRunConstructionScriptOnDrag"));
            DetailLayout.HideProperty(DetailLayout.GetProperty(RunOnDragPropName));
        }
        else
        {
            // Only display the ability to deprecate a OdysseyBrush on non-level Blueprints.
            Category.AddCustomRow( LOCTEXT("DeprecateLabel", "Deprecate"), true )
                .NameContent()
                [
                    SNew(STextBlock)
                    .Text( LOCTEXT("DeprecateLabel", "Deprecate") )
                    .ToolTipText( this, &FOdysseyBrushGlobalOptionsDetails::GetDeprecatedTooltip )
                    .Font(IDetailLayoutBuilder::GetDetailFont())
                ]
                .ValueContent()
                [
                    SNew(SCheckBox)
                    .IsEnabled( this, &FOdysseyBrushGlobalOptionsDetails::CanDeprecateOdysseyBrush )
                    .IsChecked( this, &FOdysseyBrushGlobalOptionsDetails::IsDeprecatedOdysseyBrush )
                    .OnCheckStateChanged( this, &FOdysseyBrushGlobalOptionsDetails::OnDeprecateOdysseyBrush )
                    .ToolTipText( this, &FOdysseyBrushGlobalOptionsDetails::GetDeprecatedTooltip )
                ];
        }

        IDetailCategoryBuilder& PkgCategory = DetailLayout.EditCategory("Packaging", LOCTEXT("OdysseyBrushPackagingCategory", "Packaging"));
        PkgCategory.AddCustomRow(LOCTEXT("NativizeLabel", "Nativize"))
            .NameContent()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("NativizeLabel", "Nativize"))
                .Font(IDetailLayoutBuilder::GetDetailFont())
            ]
            .ValueContent()
            [
                SNew(SCheckBox)
                .IsEnabled(this, &FOdysseyBrushGlobalOptionsDetails::IsNativizeEnabled)
                .IsChecked(this, &FOdysseyBrushGlobalOptionsDetails::GetNativizeState)
                .OnCheckStateChanged(this, &FOdysseyBrushGlobalOptionsDetails::OnNativizeToggled)
                .ToolTipText(this, &FOdysseyBrushGlobalOptionsDetails::GetNativizeTooltip)
            ];
    }
}

bool FOdysseyBrushGlobalOptionsDetails::IsNativizeEnabled() const
{
    bool bIsEnabled = false;
    if (UBlueprint* OdysseyBrush = GetBlueprintObj())
    {
        bIsEnabled = OdysseyBrush->SupportsNativization() && !FBlueprintEditorUtils::ShouldNativizeImplicitly(OdysseyBrush);
    }
    return bIsEnabled;
}

ECheckBoxState FOdysseyBrushGlobalOptionsDetails::GetNativizeState() const
{
    ECheckBoxState CheckboxState = ECheckBoxState::Undetermined;
    if (UBlueprint* OdysseyBrush = GetBlueprintObj())
    {
        if (FBlueprintEditorUtils::ShouldNativizeImplicitly(OdysseyBrush))
        {
            CheckboxState = ECheckBoxState::Checked;
        }
        else
        {
            switch (OdysseyBrush->NativizationFlag)
            {
            case EBlueprintNativizationFlag::Disabled:
                CheckboxState = ECheckBoxState::Unchecked;
                break;

            case EBlueprintNativizationFlag::ExplicitlyEnabled:
                CheckboxState = ECheckBoxState::Checked;
                break;

            case EBlueprintNativizationFlag::Dependency:
            default:
                // leave "Undetermined"
                break;
            }
        }
    }
    return CheckboxState;
}

FText FOdysseyBrushGlobalOptionsDetails::GetNativizeTooltip() const
{
    UBlueprint* OdysseyBrush = GetBlueprintObj();

    if (OdysseyBrush)
    {
        if (FBlueprintEditorUtils::ShouldNativizeImplicitly(OdysseyBrush))
        {
            return LOCTEXT("NativizeImplicitlyTooltip", "This OdysseyBrush must be nativized because it overrides one or more BlueprintCallable functions inherited from a parent OdysseyBrush class that has also been flagged for nativization.");
        }

        FText Reason;
        if (!OdysseyBrush->SupportsNativization(&Reason))
        {
            return Reason.IsEmpty() ? LOCTEXT("NativizeDisabledTooltip", "This blueprint does not support nativization.") : Reason;
        }

        if (OdysseyBrush->NativizationFlag == EBlueprintNativizationFlag::Dependency)
        {
            return LOCTEXT("NativizeAsDependencyTooltip", "This OdysseyBrush has been flagged to nativize as a dependency needed by another OdysseyBrush. This will be applied once that OdysseyBrush is saved.");
        }
    }

    return LOCTEXT("NativizeTooltip", "When exclusive nativization is enabled, then this asset will be nativized. NOTE: All super classes must be also nativized.");
}

void FOdysseyBrushGlobalOptionsDetails::OnNativizeToggled(ECheckBoxState NewState) const
{
    if (UBlueprint* OdysseyBrush = GetBlueprintObj())
    {
        if (NewState == ECheckBoxState::Checked)
        {
            OdysseyBrush->NativizationFlag = EBlueprintNativizationFlag::ExplicitlyEnabled;

            TArray<UClass*> NativizationDependencies;
            FBlueprintEditorUtils::FindNativizationDependencies(OdysseyBrush, NativizationDependencies);

            int32 bDependenciesFlagged = 0;
            // tag all dependencies as needing nativization
            for (int32 DependencyIndex = 0; DependencyIndex < NativizationDependencies.Num(); ++DependencyIndex)
            {
                UClass* Dependency = NativizationDependencies[DependencyIndex];
                if (UBlueprint* DependentBp = UBlueprint::GetBlueprintFromClass(Dependency))
                {
                    if (DependentBp->NativizationFlag == EBlueprintNativizationFlag::Disabled)
                    {
                        DependentBp->NativizationFlag = EBlueprintNativizationFlag::Dependency;
                        ++bDependenciesFlagged;
                    }
                    // recursively tag dependencies up the chain...
                    // relying on the fact that this only adds to the array via AddUnique()
                    FBlueprintEditorUtils::FindNativizationDependencies(DependentBp, NativizationDependencies);
                }
            }


            if (bDependenciesFlagged > 0)
            {
                FNotificationInfo Warning(LOCTEXT("DependenciesMarkedForNativization", "Flagged extra (required dependency) Blueprints for nativization."));
                Warning.ExpireDuration = 5.0f;
                Warning.bFireAndForget = true;
                Warning.Image = FCoreStyle::Get().GetBrush(TEXT("MessageLog.Warning"));
                FSlateNotificationManager::Get().AddNotification(Warning);
            }
        }
        else
        {
            OdysseyBrush->NativizationFlag = EBlueprintNativizationFlag::Disabled;
        }

        // don't need to alter (dirty) compilation state, just the package's save state (since we save this setting to a config on save)
//         UProperty* NativizeProperty = UBlueprint::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UBlueprint, NativizationFlag));
//         if (ensure(NativizeProperty != nullptr))
//         {
//             FPropertyChangedEvent PropertyChangedEvent(NativizeProperty);
//             PropertyChangedEvent.ChangeType = EPropertyChangeType::ValueSet;
//
//             FBlueprintEditorUtils::MarkBlueprintAsModified(OdysseyBrush, PropertyChangedEvent);
//         }
        OdysseyBrush->MarkPackageDirty();
    }
}

void FOdysseyBrushComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    check( OdysseyBrushEditorPtr.IsValid() );
    TSharedPtr<SSCSEditor> Editor = OdysseyBrushEditorPtr.Pin()->GetSCSEditor();
    check( Editor.IsValid() );
    const UBlueprint* OdysseyBrushObj = GetBlueprintObj();
    check(OdysseyBrushObj != nullptr);

    TArray<FSCSEditorTreeNodePtrType> Nodes = Editor->GetSelectedNodes();

    if (!Nodes.Num())
    {
        CachedNodePtr = nullptr;
    }
    else if (Nodes.Num() == 1)
    {
        CachedNodePtr = Nodes[0];
    }

    if( CachedNodePtr.IsValid() )
    {
        IDetailCategoryBuilder& VariableCategory = DetailLayout.EditCategory("Variable", LOCTEXT("VariableDetailsCategory", "Variable"), ECategoryPriority::Variable);

        VariableNameEditableTextBox = SNew(SEditableTextBox)
            .Text(this, &FOdysseyBrushComponentDetails::OnGetVariableText)
            .OnTextChanged(this, &FOdysseyBrushComponentDetails::OnVariableTextChanged)
            .OnTextCommitted(this, &FOdysseyBrushComponentDetails::OnVariableTextCommitted)
            .IsReadOnly(!CachedNodePtr->CanRename())
            .Font(IDetailLayoutBuilder::GetDetailFont());

        VariableCategory.AddCustomRow(LOCTEXT("OdysseyBrushComponentDetails_VariableNameLabel", "Variable Name"))
        .NameContent()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyBrushComponentDetails_VariableNameLabel", "Variable Name"))
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ]
        .ValueContent()
        [
            VariableNameEditableTextBox.ToSharedRef()
        ];

        VariableCategory.AddCustomRow(LOCTEXT("OdysseyBrushComponentDetails_VariableTooltipLabel", "Tooltip"))
        .NameContent()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyBrushComponentDetails_VariableTooltipLabel", "Tooltip"))
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ]
        .ValueContent()
        [
            SNew(SEditableTextBox)
            .Text(this, &FOdysseyBrushComponentDetails::OnGetTooltipText)
            .OnTextCommitted(this, &FOdysseyBrushComponentDetails::OnTooltipTextCommitted, CachedNodePtr->GetVariableName())
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ];

        PopulateVariableCategories();
        const FText CategoryTooltip = LOCTEXT("EditCategoryName_Tooltip", "The category of the variable; editing this will place the variable into another category or create a new one.");

        VariableCategory.AddCustomRow( LOCTEXT("OdysseyBrushComponentDetails_VariableCategoryLabel", "Category") )
        .NameContent()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyBrushComponentDetails_VariableCategoryLabel", "Category"))
            .ToolTipText(CategoryTooltip)
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ]
        .ValueContent()
        [
            SAssignNew(VariableCategoryComboButton, SComboButton)
            .ContentPadding(FMargin(0,0,5,0))
            .IsEnabled(this, &FOdysseyBrushComponentDetails::OnVariableCategoryChangeEnabled)
            .ButtonContent()
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("NoBorder"))
                .Padding(FMargin(0, 0, 5, 0))
                [
                    SNew(SEditableTextBox)
                    .Text(this, &FOdysseyBrushComponentDetails::OnGetVariableCategoryText)
                    .OnTextCommitted(this, &FOdysseyBrushComponentDetails::OnVariableCategoryTextCommitted, CachedNodePtr->GetVariableName())
                    .ToolTipText(CategoryTooltip)
                    .SelectAllTextWhenFocused(true)
                    .RevertTextOnEscape(true)
                    .Font(IDetailLayoutBuilder::GetDetailFont())
                ]
            ]
            .MenuContent()
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .AutoHeight()
                .MaxHeight(400.0f)
                [
                    SAssignNew(VariableCategoryListView, SListView<TSharedPtr<FText>>)
                    .ListItemsSource(&VariableCategorySource)
                    .OnGenerateRow(this, &FOdysseyBrushComponentDetails::MakeVariableCategoryViewWidget)
                    .OnSelectionChanged(this, &FOdysseyBrushComponentDetails::OnVariableCategorySelectionChanged)
                ]
            ]
        ];

        IDetailCategoryBuilder& SocketsCategory = DetailLayout.EditCategory("Sockets", LOCTEXT("OdysseyBrushComponentDetailsCategory", "Sockets"), ECategoryPriority::Important);

        SocketsCategory.AddCustomRow(LOCTEXT("OdysseyBrushComponentDetails_Sockets", "Sockets"))
        .NameContent()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("OdysseyBrushComponentDetails_ParentSocket", "Parent Socket"))
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ]
        .ValueContent()
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SEditableTextBox)
                .Text(this, &FOdysseyBrushComponentDetails::GetSocketName)
                .IsReadOnly(true)
                .Font(IDetailLayoutBuilder::GetDetailFont())
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(2.0f, 1.0f)
            [
                PropertyCustomizationHelpers::MakeBrowseButton(
                    FSimpleDelegate::CreateSP(this, &FOdysseyBrushComponentDetails::OnBrowseSocket),
                    LOCTEXT( "SocketBrowseButtonToolTipText", "Select a different Parent Socket - cannot change socket on inherited components"),
                    TAttribute<bool>(this, &FOdysseyBrushComponentDetails::CanChangeSocket)
                )
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .Padding(2.0f, 1.0f)
            [
                PropertyCustomizationHelpers::MakeClearButton(
                    FSimpleDelegate::CreateSP(this, &FOdysseyBrushComponentDetails::OnClearSocket),
                    LOCTEXT("SocketClearButtonToolTipText", "Clear the Parent Socket - cannot change socket on inherited components"),
                    TAttribute<bool>(this, &FOdysseyBrushComponentDetails::CanChangeSocket)
                )
            ]
        ];
    }

    // Handle event generation
    if ( FBlueprintEditorUtils::DoesSupportEventGraphs(OdysseyBrushObj) && Nodes.Num() == 1 )
    {
        FName PropertyName = CachedNodePtr->GetVariableName();
        UObjectProperty* VariableProperty = FindField<UObjectProperty>(OdysseyBrushObj->SkeletonGeneratedClass, PropertyName);

        AddEventsCategory(DetailLayout, VariableProperty);
    }

    // Don't show tick properties for components in the blueprint details
    TSharedPtr<IPropertyHandle> PrimaryTickProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UActorComponent, PrimaryComponentTick));
    PrimaryTickProperty->MarkHiddenByCustomization();
}

FText FOdysseyBrushComponentDetails::OnGetVariableText() const
{
    check(CachedNodePtr.IsValid());

    return FText::FromName(CachedNodePtr->GetVariableName());
}

void FOdysseyBrushComponentDetails::OnVariableTextChanged(const FText& InNewText)
{
    check(CachedNodePtr.IsValid());

    bIsVariableNameInvalid = true;

    USCS_Node* SCS_Node = CachedNodePtr->GetSCSNode();
    if(SCS_Node != NULL && !InNewText.IsEmpty() && !FComponentEditorUtils::IsValidVariableNameString(SCS_Node->ComponentTemplate, InNewText.ToString()))
    {
        VariableNameEditableTextBox->SetError(LOCTEXT("ComponentVariableRenameFailed_NotValid", "This name is reserved for engine use."));
        return;
    }

    TSharedPtr<INameValidatorInterface> VariableNameValidator = MakeShareable(new FKismetNameValidator(GetBlueprintObj(), CachedNodePtr->GetVariableName()));

    EValidatorResult ValidatorResult = VariableNameValidator->IsValid(InNewText.ToString());
    if(ValidatorResult == EValidatorResult::AlreadyInUse)
    {
        VariableNameEditableTextBox->SetError(FText::Format(LOCTEXT("ComponentVariableRenameFailed_InUse", "{0} is in use by another variable or function!"), InNewText));
    }
    else if(ValidatorResult == EValidatorResult::EmptyName)
    {
        VariableNameEditableTextBox->SetError(LOCTEXT("RenameFailed_LeftBlank", "Names cannot be left blank!"));
    }
    else if(ValidatorResult == EValidatorResult::TooLong)
    {
        VariableNameEditableTextBox->SetError(FText::Format( LOCTEXT("RenameFailed_NameTooLong", "Names must have fewer than {0} characters!"), FText::AsNumber( FKismetNameValidator::GetMaximumNameLength())));
    }
    else
    {
        bIsVariableNameInvalid = false;
        VariableNameEditableTextBox->SetError(FText::GetEmpty());
    }
}

void FOdysseyBrushComponentDetails::OnVariableTextCommitted(const FText& InNewName, ETextCommit::Type InTextCommit)
{
    if ( !bIsVariableNameInvalid )
    {
        check(CachedNodePtr.IsValid());

        USCS_Node* SCS_Node = CachedNodePtr->GetSCSNode();
        if(SCS_Node != NULL)
        {
            const FScopedTransaction Transaction( LOCTEXT("RenameComponentVariable", "Rename Component Variable") );
            FBlueprintEditorUtils::RenameComponentMemberVariable(GetBlueprintObj(), CachedNodePtr->GetSCSNode(), FName( *InNewName.ToString() ));
        }
    }

    bIsVariableNameInvalid = false;
    VariableNameEditableTextBox->SetError(FText::GetEmpty());
}

FText FOdysseyBrushComponentDetails::OnGetTooltipText() const
{
    check(CachedNodePtr.IsValid());

    FName VarName = CachedNodePtr->GetVariableName();
    if (VarName != NAME_None)
    {
        FString Result;
        FBlueprintEditorUtils::GetBlueprintVariableMetaData(GetBlueprintObj(), VarName, NULL, TEXT("tooltip"), Result);
        return FText::FromString(Result);
    }

    return FText();
}

void FOdysseyBrushComponentDetails::OnTooltipTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit, FName VarName)
{
    FBlueprintEditorUtils::SetBlueprintVariableMetaData(GetBlueprintObj(), VarName, NULL, TEXT("tooltip"), NewText.ToString() );
}

bool FOdysseyBrushComponentDetails::OnVariableCategoryChangeEnabled() const
{
    check(CachedNodePtr.IsValid());

    return !CachedNodePtr->CanRename();
}

FText FOdysseyBrushComponentDetails::OnGetVariableCategoryText() const
{
    check(CachedNodePtr.IsValid());

    FName VarName = CachedNodePtr->GetVariableName();
    if (VarName != NAME_None)
    {
        FText Category = FBlueprintEditorUtils::GetBlueprintVariableCategory(GetBlueprintObj(), VarName, NULL);

        // Older blueprints will have their name as the default category
        if( Category.EqualTo(FText::FromString(GetBlueprintObj()->GetName())) )
        {
            return UEdGraphSchema_OdysseyBrush::VR_DefaultCategory;
        }
        else
        {
            return Category;
        }
    }

    return FText();
}

void FOdysseyBrushComponentDetails::OnVariableCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit, FName VarName)
{
    check(CachedNodePtr.IsValid());

    if (InTextCommit == ETextCommit::OnEnter || InTextCommit == ETextCommit::OnUserMovedFocus)
    {
        FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), CachedNodePtr->GetVariableName(), NULL, NewText);
        PopulateVariableCategories();
    }
}

void FOdysseyBrushComponentDetails::OnVariableCategorySelectionChanged( TSharedPtr<FText> ProposedSelection, ESelectInfo::Type /*SelectInfo*/ )
{
    check(CachedNodePtr.IsValid());

    FName VarName = CachedNodePtr->GetVariableName();
    if (ProposedSelection.IsValid() && VarName != NAME_None)
    {
        FText NewCategory = *ProposedSelection.Get();
        FBlueprintEditorUtils::SetBlueprintVariableCategory(GetBlueprintObj(), VarName, NULL, NewCategory);

        check(VariableCategoryListView.IsValid());
        check(VariableCategoryComboButton.IsValid());

        VariableCategoryListView->ClearSelection();
        VariableCategoryComboButton->SetIsOpen(false);
    }
}

TSharedRef< ITableRow > FOdysseyBrushComponentDetails::MakeVariableCategoryViewWidget( TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable )
{
    return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
    [
        SNew(STextBlock)
            .Text(*Item.Get())
    ];
}

void FOdysseyBrushComponentDetails::PopulateVariableCategories()
{
    UBlueprint* OdysseyBrushObj = GetBlueprintObj();

    check(OdysseyBrushObj);
    check(OdysseyBrushObj->SkeletonGeneratedClass);

    TSet<FName> VisibleVariables;
    for (TFieldIterator<UProperty> PropertyIt(OdysseyBrushObj->SkeletonGeneratedClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
    {
        UProperty* Property = *PropertyIt;

        if ((!Property->HasAnyPropertyFlags(CPF_Parm) && Property->HasAllPropertyFlags(CPF_BlueprintVisible)))
        {
            VisibleVariables.Add(Property->GetFName());
        }
    }

    FBlueprintEditorUtils::GetSCSVariableNameList(OdysseyBrushObj, VisibleVariables);

    VariableCategorySource.Empty();
    VariableCategorySource.Add(MakeShareable(new FText(LOCTEXT("Default", "Default"))));
    for (const FName& VariableName : VisibleVariables)
    {
        FText Category = FBlueprintEditorUtils::GetBlueprintVariableCategory(OdysseyBrushObj, VariableName, nullptr);
        if (!Category.IsEmpty() && !Category.EqualTo(FText::FromString(OdysseyBrushObj->GetName())))
        {
            bool bNewCategory = true;
            for (int32 j = 0; j < VariableCategorySource.Num() && bNewCategory; ++j)
            {
                bNewCategory &= !VariableCategorySource[j].Get()->EqualTo(Category);
            }
            if (bNewCategory)
            {
                VariableCategorySource.Add(MakeShareable(new FText(Category)));
            }
        }
    }
}

FText FOdysseyBrushComponentDetails::GetSocketName() const
{
    check(CachedNodePtr.IsValid());

    if (CachedNodePtr->GetSCSNode() != NULL)
    {
        return FText::FromName(CachedNodePtr->GetSCSNode()->AttachToName);
    }
    return FText::GetEmpty();
}

bool FOdysseyBrushComponentDetails::CanChangeSocket() const
{
    check(CachedNodePtr.IsValid());

    if (CachedNodePtr->GetSCSNode() != NULL)
    {
        return !CachedNodePtr->IsInherited();
    }
    return true;
}

void FOdysseyBrushComponentDetails::OnBrowseSocket()
{
    check(CachedNodePtr.IsValid());

    if (CachedNodePtr->GetSCSNode() != NULL)
    {
        TSharedPtr<SSCSEditor> Editor = OdysseyBrushEditorPtr.Pin()->GetSCSEditor();
        check( Editor.IsValid() );

        FSCSEditorTreeNodePtrType ParentFNode = CachedNodePtr->GetParent();

        if (ParentFNode.IsValid())
        {
            if (USceneComponent* ParentSceneComponent = Cast<USceneComponent>(ParentFNode->GetEditableComponentTemplate(Editor->GetBlueprint())))
            {
                if (ParentSceneComponent->HasAnySockets())
                {
                    // Pop up a combo box to pick socket from mesh
                    FSlateApplication::Get().PushMenu(
                        Editor.ToSharedRef(),
                        FWidgetPath(),
                        SNew(SSocketChooserPopup)
                        .SceneComponent( ParentSceneComponent )
                        .OnSocketChosen( this, &FOdysseyBrushComponentDetails::OnSocketSelection ),
                        FSlateApplication::Get().GetCursorPos(),
                        FPopupTransitionEffect( FPopupTransitionEffect::TypeInPopup )
                        );
                }
            }
        }
    }
}

void FOdysseyBrushComponentDetails::OnClearSocket()
{
    check(CachedNodePtr.IsValid());

    if (CachedNodePtr->GetSCSNode() != NULL)
    {
        CachedNodePtr->GetSCSNode()->AttachToName = NAME_None;
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());
    }
}

void FOdysseyBrushComponentDetails::OnSocketSelection( FName SocketName )
{
    check(CachedNodePtr.IsValid());

    USCS_Node* SCS_Node = CachedNodePtr->GetSCSNode();
    if (SCS_Node != NULL)
    {
        // Record selection if there is an actual asset attached
        SCS_Node->AttachToName = SocketName;
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FOdysseyBrushGraphNodeDetails::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
    const TArray<TWeakObjectPtr<UObject>>& SelectedObjects = DetailLayout.GetSelectedObjects();
    if( SelectedObjects.Num() == 1 )
    {
        if (SelectedObjects[0].IsValid() && SelectedObjects[0]->IsA<UEdGraphNode>())
        {
            GraphNodePtr = Cast<UEdGraphNode>(SelectedObjects[0].Get());
        }
    }

    if(!GraphNodePtr.IsValid() || !GraphNodePtr.Get()->bCanRenameNode)
    {
        return;
    }

    IDetailCategoryBuilder& Category = DetailLayout.EditCategory("GraphNodeDetail", LOCTEXT("GraphNodeDetailsCategory", "Graph Node"), ECategoryPriority::Important);
    const FSlateFontInfo DetailFontInfo = IDetailLayoutBuilder::GetDetailFont();
    FText RowHeader;
    FText NameContent;

    if( GraphNodePtr->IsA( UEdGraphNode_Comment::StaticClass() ))
    {
        RowHeader = LOCTEXT("GraphNodeDetail_CommentRowTitle", "Comment");
        NameContent = LOCTEXT("GraphNodeDetail_CommentContentTitle", "Comment Text");
    }
    else
    {
        RowHeader = LOCTEXT("GraphNodeDetail_NodeRowTitle", "Node Title");
        NameContent = LOCTEXT("GraphNodeDetail_ContentTitle", "Name");
    }

    bool bNameAllowsMultiLine = false;
    if( GraphNodePtr.IsValid() && GraphNodePtr.Get()->IsA<UEdGraphNode_Comment>() )
    {
        bNameAllowsMultiLine = true;
    }

    TSharedPtr<SWidget> EditNameWidget;
    float WidgetMinDesiredWidth = OdysseyBrushDocumentationDetailDefs::DetailsTitleMinWidth;
    float WidgetMaxDesiredWidth = OdysseyBrushDocumentationDetailDefs::DetailsTitleMaxWidth;
    if( bNameAllowsMultiLine )
    {
        SAssignNew(MultiLineNameEditableTextBox, SMultiLineEditableTextBox)
        .Text(this, &FOdysseyBrushGraphNodeDetails::OnGetName)
        .OnTextChanged(this, &FOdysseyBrushGraphNodeDetails::OnNameChanged)
        .OnTextCommitted(this, &FOdysseyBrushGraphNodeDetails::OnNameCommitted)
        .ClearKeyboardFocusOnCommit(true)
        .ModiferKeyForNewLine(EModifierKey::Shift)
        .RevertTextOnEscape(true)
        .SelectAllTextWhenFocused(true)
        .IsReadOnly(this, &FOdysseyBrushGraphNodeDetails::IsNameReadOnly)
        .Font(DetailFontInfo)
        .WrapTextAt(WidgetMaxDesiredWidth - OdysseyBrushDocumentationDetailDefs::DetailsTitleWrapPadding);

        EditNameWidget = MultiLineNameEditableTextBox;
    }
    else
    {
        SAssignNew(NameEditableTextBox, SEditableTextBox)
        .Text(this, &FOdysseyBrushGraphNodeDetails::OnGetName)
        .OnTextChanged(this, &FOdysseyBrushGraphNodeDetails::OnNameChanged)
        .OnTextCommitted(this, &FOdysseyBrushGraphNodeDetails::OnNameCommitted)
        .Font(DetailFontInfo);

        EditNameWidget = NameEditableTextBox;
        WidgetMaxDesiredWidth = WidgetMinDesiredWidth;
    }

    Category.AddCustomRow( RowHeader )
    .NameContent()
    [
        SNew(STextBlock)
        .Text( NameContent )
        .Font(DetailFontInfo)
    ]
    .ValueContent()
    .MinDesiredWidth(WidgetMinDesiredWidth)
    .MaxDesiredWidth(WidgetMaxDesiredWidth)
    [
        EditNameWidget.ToSharedRef()
    ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FOdysseyBrushGraphNodeDetails::SetNameError( const FText& Error )
{
    if( NameEditableTextBox.IsValid() )
    {
        NameEditableTextBox->SetError( Error );
    }
    if( MultiLineNameEditableTextBox.IsValid() )
    {
        MultiLineNameEditableTextBox->SetError( Error );
    }
}

bool FOdysseyBrushGraphNodeDetails::IsNameReadOnly() const
{
    bool bReadOnly = true;
    if(GraphNodePtr.IsValid())
    {
        bReadOnly = !GraphNodePtr->bCanRenameNode;
    }
    return bReadOnly;
}

FText FOdysseyBrushGraphNodeDetails::OnGetName() const
{
    FText Name;
    if(GraphNodePtr.IsValid())
    {
        Name = GraphNodePtr->GetNodeTitle( ENodeTitleType::EditableTitle );
    }
    return Name;
}

struct FGraphNodeNameValidatorHelper
{
    static EValidatorResult Validate(TWeakObjectPtr<UEdGraphNode> GraphNodePtr, TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr, const FString& NewName)
    {
        check(GraphNodePtr.IsValid() && OdysseyBrushEditorPtr.IsValid());
        TSharedPtr<INameValidatorInterface> NameValidator = GraphNodePtr->MakeNameValidator();
        if (!NameValidator.IsValid())
        {
            const FName NodeName(*GraphNodePtr->GetNodeTitle(ENodeTitleType::EditableTitle).ToString());
            NameValidator = MakeShareable(new FKismetNameValidator(OdysseyBrushEditorPtr.Pin()->GetBlueprintObj(), NodeName));
        }
        return NameValidator->IsValid(NewName);
    }
};

void FOdysseyBrushGraphNodeDetails::OnNameChanged(const FText& InNewText)
{
    if( GraphNodePtr.IsValid() && OdysseyBrushEditorPtr.IsValid() )
    {
        const EValidatorResult ValidatorResult = FGraphNodeNameValidatorHelper::Validate(GraphNodePtr, OdysseyBrushEditorPtr, InNewText.ToString());
        if(ValidatorResult == EValidatorResult::AlreadyInUse)
        {
            SetNameError(FText::Format(LOCTEXT("RenameFailed_InUse", "{0} is in use by another variable or function!"), InNewText));
        }
        else if(ValidatorResult == EValidatorResult::EmptyName)
        {
            SetNameError(LOCTEXT("RenameFailed_LeftBlank", "Names cannot be left blank!"));
        }
        else if(ValidatorResult == EValidatorResult::TooLong)
        {
            SetNameError(FText::Format( LOCTEXT("RenameFailed_NameTooLong", "Names must have fewer than {0} characters!"), FText::AsNumber( FKismetNameValidator::GetMaximumNameLength())));
        }
        else
        {
            SetNameError(FText::GetEmpty());
        }
    }
}

void FOdysseyBrushGraphNodeDetails::OnNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit)
{
    if (OdysseyBrushEditorPtr.IsValid() && GraphNodePtr.IsValid())
    {
        if (FGraphNodeNameValidatorHelper::Validate(GraphNodePtr, OdysseyBrushEditorPtr, InNewText.ToString()) == EValidatorResult::Ok)
        {
            OdysseyBrushEditorPtr.Pin()->OnNodeTitleCommitted(InNewText, InTextCommit, GraphNodePtr.Get());
        }
    }
}

UBlueprint* FOdysseyBrushGraphNodeDetails::GetBlueprintObj() const
{
    if(OdysseyBrushEditorPtr.IsValid())
    {
        return OdysseyBrushEditorPtr.Pin()->GetBlueprintObj();
    }

    return NULL;
}

TSharedRef<IDetailCustomization> FChildActorComponentDetails::MakeInstance(TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtrIn)
{
    return MakeShareable(new FChildActorComponentDetails(OdysseyBrushEditorPtrIn));
}

FChildActorComponentDetails::FChildActorComponentDetails(TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtrIn)
    : OdysseyBrushEditorPtr(OdysseyBrushEditorPtrIn)
{
}

void FChildActorComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TSharedPtr<IPropertyHandle> ActorClassProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UChildActorComponent, ChildActorClass));
    if (ActorClassProperty->IsValidHandle())
    {

        TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
        DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

        IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(TEXT("ChildActorComponent"));

        // Ensure ordering is what we want by adding class in first
        CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(UChildActorComponent, ChildActorClass));

        IDetailPropertyRow& CATRow = CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(UChildActorComponent, ChildActorTemplate));
        CATRow.Visibility(TAttribute<EVisibility>::Create([ObjectsBeingCustomized]()
        {
            for (const TWeakObjectPtr<UObject>& ObjectBeingCustomized : ObjectsBeingCustomized)
            {
                if (UChildActorComponent* CAC = Cast<UChildActorComponent>(ObjectBeingCustomized.Get()))
                {
                    if (CAC->ChildActorTemplate == nullptr)
                    {
                        return EVisibility::Hidden;
                    }
                }
                else
                {
                    return EVisibility::Hidden;
                }
            }

            return EVisibility::Visible;
        }));
    }
}

void FOdysseyBrushDocumentationDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    check( OdysseyBrushEditorPtr.IsValid() );
    // find currently selected edgraph documentation node
    DocumentationNodePtr = EdGraphSelectionAsDocumentNode();

    if( DocumentationNodePtr.IsValid() )
    {
        // Cache Link
        DocumentationLink = DocumentationNodePtr->GetDocumentationLink();
        DocumentationExcerpt = DocumentationNodePtr->GetDocumentationExcerptName();

        IDetailCategoryBuilder& DocumentationCategory = DetailLayout.EditCategory("Documentation", LOCTEXT("DocumentationDetailsCategory", "Documentation"), ECategoryPriority::Default);

        DocumentationCategory.AddCustomRow( LOCTEXT( "DocumentationLinkLabel", "Documentation Link" ))
        .NameContent()
        .HAlign( HAlign_Fill )
        [
            SNew( STextBlock )
            .Text( LOCTEXT( "FOdysseyBrushDocumentationDetails_Link", "Link" ) )
            .ToolTipText( LOCTEXT( "FOdysseyBrushDocumentationDetails_LinkPathTooltip", "The documentation content path" ))
            .Font( IDetailLayoutBuilder::GetDetailFont() )
        ]
        .ValueContent()
        .HAlign( HAlign_Left )
        .MinDesiredWidth( OdysseyBrushDocumentationDetailDefs::DetailsTitleMinWidth )
        .MaxDesiredWidth( OdysseyBrushDocumentationDetailDefs::DetailsTitleMaxWidth )
        [
            SNew( SEditableTextBox )
            .Padding( FMargin( 4.f, 2.f ))
            .Text( this, &FOdysseyBrushDocumentationDetails::OnGetDocumentationLink )
            .ToolTipText( LOCTEXT( "FOdysseyBrushDocumentationDetails_LinkTooltip", "The path of the documentation content relative to /Engine/Documentation/Source" ))
            .OnTextCommitted( this, &FOdysseyBrushDocumentationDetails::OnDocumentationLinkCommitted )
            .Font( IDetailLayoutBuilder::GetDetailFont() )
        ];

        DocumentationCategory.AddCustomRow( LOCTEXT( "DocumentationExcerptsLabel", "Documentation Excerpts" ))
        .NameContent()
        .HAlign( HAlign_Left )
        [
            SNew( STextBlock )
            .Text( LOCTEXT( "FOdysseyBrushDocumentationDetails_Excerpt", "Excerpt" ) )
            .ToolTipText( LOCTEXT( "FOdysseyBrushDocumentationDetails_ExcerptTooltip", "The current documentation excerpt" ))
            .Font( IDetailLayoutBuilder::GetDetailFont() )
        ]
        .ValueContent()
        .HAlign( HAlign_Left )
        .MinDesiredWidth( OdysseyBrushDocumentationDetailDefs::DetailsTitleMinWidth )
        .MaxDesiredWidth( OdysseyBrushDocumentationDetailDefs::DetailsTitleMaxWidth )
        [
            SAssignNew( ExcerptComboButton, SComboButton )
            .ContentPadding( 2.f )
            .IsEnabled( this, &FOdysseyBrushDocumentationDetails::OnExcerptChangeEnabled )
            .ButtonContent()
            [
                SNew(SBorder)
                .BorderImage( FEditorStyle::GetBrush( "NoBorder" ))
                .Padding( FMargin( 0, 0, 5, 0 ))
                [
                    SNew( STextBlock )
                    .Text( this, &FOdysseyBrushDocumentationDetails::OnGetDocumentationExcerpt )
                    .ToolTipText( LOCTEXT( "FOdysseyBrushDocumentationDetails_ExcerptComboTooltip", "Select Excerpt" ))
                    .Font( IDetailLayoutBuilder::GetDetailFont() )
                ]
            ]
            .OnGetMenuContent( this, &FOdysseyBrushDocumentationDetails::GenerateExcerptList )
        ];
    }
}

TWeakObjectPtr<UEdGraphNode_Documentation> FOdysseyBrushDocumentationDetails::EdGraphSelectionAsDocumentNode()
{
    DocumentationNodePtr.Reset();

    if( OdysseyBrushEditorPtr.IsValid() )
    {
        /** Get the currently selected set of nodes */
        if( OdysseyBrushEditorPtr.Pin()->GetNumberOfSelectedNodes() == 1 )
        {
            TSet<UObject*> Objects = OdysseyBrushEditorPtr.Pin()->GetSelectedNodes();
            TSet<UObject*>::TIterator Iter( Objects );
            UObject* Object = *Iter;

            if( Object && Object->IsA<UEdGraphNode_Documentation>() )
            {
                DocumentationNodePtr = Cast<UEdGraphNode_Documentation>( Object );
            }
        }
    }
    return DocumentationNodePtr;
}

FText FOdysseyBrushDocumentationDetails::OnGetDocumentationLink() const
{
    return FText::FromString( DocumentationLink );
}

FText FOdysseyBrushDocumentationDetails::OnGetDocumentationExcerpt() const
{
    return FText::FromString( DocumentationExcerpt );
}

bool FOdysseyBrushDocumentationDetails::OnExcerptChangeEnabled() const
{
    return IDocumentation::Get()->PageExists( DocumentationLink );
}

void FOdysseyBrushDocumentationDetails::OnDocumentationLinkCommitted( const FText& InNewName, ETextCommit::Type InTextCommit )
{
    DocumentationLink = InNewName.ToString();
    DocumentationExcerpt = NSLOCTEXT( "FOdysseyBrushDocumentationDetails", "ExcerptCombo_DefaultText", "Select Excerpt" ).ToString();
}

TSharedRef< ITableRow > FOdysseyBrushDocumentationDetails::MakeExcerptViewWidget( TSharedPtr<FString> Item, const TSharedRef< STableViewBase >& OwnerTable )
{
    return
        SNew( STableRow<TSharedPtr<FString>>, OwnerTable )
        [
            SNew( STextBlock )
            .Text( FText::FromString(*Item.Get()) )
        ];
}

void FOdysseyBrushDocumentationDetails::OnExcerptSelectionChanged( TSharedPtr<FString> ProposedSelection, ESelectInfo::Type /*SelectInfo*/ )
{
    if( ProposedSelection.IsValid() && DocumentationNodePtr.IsValid() )
    {
        DocumentationNodePtr->Link = DocumentationLink;
        DocumentationExcerpt = *ProposedSelection.Get();
        DocumentationNodePtr->Excerpt = DocumentationExcerpt;
        ExcerptComboButton->SetIsOpen( false );
    }
}

TSharedRef<SWidget> FOdysseyBrushDocumentationDetails::GenerateExcerptList()
{
    ExcerptList.Empty();

    if( IDocumentation::Get()->PageExists( DocumentationLink ))
    {
        TSharedPtr<IDocumentationPage> DocumentationPage = IDocumentation::Get()->GetPage( DocumentationLink, NULL );
        TArray<FExcerpt> Excerpts;
        DocumentationPage->GetExcerpts( Excerpts );

        for (const FExcerpt& Excerpt : Excerpts)
        {
            ExcerptList.Add( MakeShareable( new FString( Excerpt.Name )));
        }
    }

    return
        SNew( SHorizontalBox )
        +SHorizontalBox::Slot()
        .Padding( 2.f )
        [
            SNew( SListView< TSharedPtr<FString>> )
            .ListItemsSource( &ExcerptList )
            .OnGenerateRow( this, &FOdysseyBrushDocumentationDetails::MakeExcerptViewWidget )
            .OnSelectionChanged( this, &FOdysseyBrushDocumentationDetails::OnExcerptSelectionChanged )
        ];
}


#undef LOCTEXT_NAMESPACE
