// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationMaterialSelectionDialog.h"

#include "AssetRegistry/AssetData.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Views/ITypedTableView.h"
#include "Materials/MaterialInterface.h"
#include "PropertyCustomizationHelpers.h"
#include "Styling/AppStyle.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SWindow.h"
#include "Widgets/Layout/SScrollBox.h"

#include "OdysseyAnimationSettings.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

struct FMaterialOption
{
    FString Name;
    FString Description;
    FString MaterialPath;
};


static const TArray<FMaterialOption>& GetMaterialOptions()
{
    static const TArray<FMaterialOption> Options =
    {
        {
            TEXT("Lit, No DOF, Shadow Cast (Default)"),
            TEXT("Translucent light-sensitive Material that casts shadows and ignores Depth of Field & Motion Blur."),
            TEXT("/Odyssey/Materials/Instances/MI_Anim_CastShadow_AfterDOF_Lit.MI_Anim_CastShadow_AfterDOF_Lit")
        },

        {
            TEXT("Unlit, No DOF, Shadow Cast"),
            TEXT("Translucent Material that casts shadows and ignores Depth of Field, Motion Blur and lights from the 3D Level."),
            TEXT("/Odyssey/Materials/Instances/MI_Anim_CastShadow_AfterDOF_Unlit.MI_Anim_CastShadow_AfterDOF_Unlit")
        },

        {
            TEXT("Lit, DOF, Shadow Cast"),
            TEXT("Translucent light-sensitive Material that casts shadows and is sensitive to Depth of Field & Motion Blur."),
            TEXT("/Odyssey/Materials/Instances/MI_Anim_CastShadow_BeforeDOF_Lit.MI_Anim_CastShadow_BeforeDOF_Lit")
        },

        {
            TEXT("Unlit, DOF, Shadow Cast"),
            TEXT("Translucent Material that casts shadows, ignores lights from the 3D Level and is sensitive to Depth of Field & Motion Blur."),
            TEXT("/Odyssey/Materials/Instances/MI_Anim_CastShadow_BeforeDOF_Unlit.MI_Anim_CastShadow_BeforeDOF_Unlit")
        },

        {
            TEXT("Lit, DOF, Shadow Cast & Reception"),
            TEXT("Translucent light-sensitive Material that casts and receives shadows and is sensitive to Depth of Field & Motion Blur. Disable Virtual Shadow Map in Project Settings to avoid glitches!"),
            TEXT("/Odyssey/Materials/Instances/MI_Anim_CastShadow_SurfaceShading_BeforeDOF_Lit.MI_Anim_CastShadow_SurfaceShading_BeforeDOF_Lit")
        },

        {
            TEXT("Lit, DOF, Shadow Reception"),
            TEXT("Translucent light-sensitive Material that receives shadows and is sensitive to Depth of Field & Motion Blur."),
            TEXT("/Odyssey/Materials/Instances/MI_Anim_SurfaceShading_BeforeDOF_Lit.MI_Anim_SurfaceShading_BeforeDOF_Lit")
        },

        {
            TEXT("Non flat assets"),
            TEXT("Translucent light-sensitive Material with many exposed parameters to be used on non-flat Actors."),
            TEXT("/Odyssey/Materials/Instances/MI_NonPlaneSurfaces_Anim.MI_NonPlaneSurfaces_Anim")
        },

        {
            TEXT("Storyboard, Lit, DOF, Shadow Reception"),
            TEXT("Translucent light-sensitive Material that receives shadows and is sensitive to Depth of Field & Motion Blur, with an activable grid. Ideal for Storyboards."),
            TEXT("/Odyssey/Materials/Instances/MI_SB_Lit_SurfaceShadow_BeforeDOF.MI_SB_Lit_SurfaceShadow_BeforeDOF")
        },

        {
            TEXT("Storyboard, Unlit, No DOF, No Shadow"),
            TEXT("Translucent Material that ignores lights, shadows, Depth of Field and Motion Blur, with an activable grid. Ideal for Storyboards."),
            TEXT("/Odyssey/Materials/Instances/MI_SB_Unlit_NoShadow_AfterDOF.MI_SB_Unlit_NoShadow_AfterDOF")
        },

        {
            TEXT("Sprite, Lit"),
            TEXT("Masked light-sensitive Material for 2D Sprites."),
            TEXT("/Odyssey/Materials/Instances/MI_Sprite_Lit.MI_Sprite_Lit")
        },

        {
            TEXT("Sprite, Unlit"),
            TEXT("Masked Material for 2D Sprites that ignores lights from the 3D Level."),
            TEXT("/Odyssey/Materials/Instances/MI_Sprite_Unlit.MI_Sprite_Unlit")
        }
    };

    return Options;
}

TObjectPtr<UMaterialInterface> FMaterialSelectionDialog::Show()
{
    UOdysseyAnimationDialogSettings* Settings = GetMutableDefault<UOdysseyAnimationDialogSettings>();

    TObjectPtr<UMaterialInterface> MaterialSelected = nullptr;

    TSharedRef<FString> MaterialName = MakeShared<FString>("");

    TSharedRef<bool> bRememberChoice = MakeShared<bool>(Settings->bApplyMaterialWithoutAsking);
    TSharedRef<bool> bCopyMaterial = MakeShared<bool>(false);

    // Create Window

    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString(TEXT("Select Material for your animation")))
        .SizingRule(ESizingRule::UserSized)
        .ClientSize(FVector2D(950.0f, 670.0f))
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        .HasCloseButton(false);

    // Material default options

    TSharedRef<SVerticalBox> OptionsBox = SNew(SVerticalBox);

    for (const FMaterialOption& Option : GetMaterialOptions())
    {
        OptionsBox->AddSlot()
            .AutoHeight()
            .Padding(8.0f)
            [
                SNew(SHorizontalBox)
                    +SHorizontalBox::Slot()
                    .FillWidth(0.35f)
                    .VAlign(VAlign_Center)
                    .Padding(0.0f, 0.0f, 15.0f, 0.0f)
                    [
                        SNew(SButton)
                            .ContentPadding(FMargin(12.0f, 8.0f))
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            .OnClicked_Lambda(
                                [
                                    &Window,
                                    &MaterialSelected,
                                    Option
                                ]()
                                {
                                    UMaterialInterface* Material =
                                        LoadObject<UMaterialInterface>(
                                            nullptr,
                                            *Option.MaterialPath
                                        );

                                    if (Material)
                                    {
                                        MaterialSelected = Material;

                                        Window->RequestDestroyWindow();
                                    }

                                    return FReply::Handled();
                                })
                            [
                                SNew(STextBlock)
                                    .Text(FText::FromString(Option.Name))
                                    .Justification(ETextJustify::Center)
                            ]
                    ]
                    +SHorizontalBox::Slot()
                    .FillWidth(0.65f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SVerticalBox)
                            // Description
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            [
                                SNew(STextBlock)
                                    .Text(FText::FromString(Option.Description))
                                    .AutoWrapText(true)
                            ]
                    ]
                ];
    }

    // Material Asset Picker

    TSharedRef<SWidget> MaterialPicker =
        SNew(SObjectPropertyEntryBox)
        .AllowedClass(UMaterialInterface::StaticClass())
        .AllowClear(true)
        .AllowCreate(false)
        .DisplayUseSelected(true)
        .DisplayBrowse(true)
        .EnableContentPicker(true)
        .DisplayThumbnail(true)
        .ObjectPath_Lambda(
            [MaterialName]()
            {
                return *MaterialName;
            }
        )
        .OnObjectChanged_Lambda(
            [MaterialName, &MaterialSelected](const FAssetData& AssetData)
            {
                if (AssetData.IsValid())
                {
                    *MaterialName = AssetData.GetObjectPathString();
                    MaterialSelected = Cast<UMaterialInterface>(AssetData.GetAsset());
                }
                else
                {
                    MaterialName->Empty();
                    MaterialSelected = nullptr;
                }
            }
        );

    // Widget Creation ---
    Window->SetContent(
        SNew(SBorder)
        .Padding(10.0f)
        [
            //Options
            SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .FillHeight(1.0f)
                [
                    SNew(SScrollBox)
                        + SScrollBox::Slot()
                        [
                            OptionsBox
                        ]
                ]

            //Separator
            +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 8.0f)
                [
                    SNew(SSeparator)
                ]

            //Material Picker
            +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 3.0f, 0.0f, 0.0f)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Custom material"))
                                .Font(FAppStyle::GetFontStyle("SmallFont"))
                                .ColorAndOpacity(
                                    FSlateColor::UseSubduedForeground()
                                )
                        ]
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            MaterialPicker
                        ]
                ]
            + SVerticalBox::Slot()
                .Padding(0.0f, 8.0f)
                .AutoHeight()
                [
                    SNew(SCheckBox)
                        .IsChecked_Lambda(
                            [bCopyMaterial]()
                            {
                                return *bCopyMaterial
                                    ? ECheckBoxState::Checked
                                    : ECheckBoxState::Unchecked;
                            })
                        .OnCheckStateChanged_Lambda(
                            [bCopyMaterial](ECheckBoxState State)
                            {
                                *bCopyMaterial = State == ECheckBoxState::Checked;
                            })
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("Create a copy of the material")))
                        ]
                ]
            //Remember choice
            +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 8.0f, 0.0f, 0.0f)
                [
                    SNew(SHorizontalBox)

                        // Remember checkbox
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .VAlign(VAlign_Center)
                        [
                            SNew(SCheckBox)
                                .IsChecked_Lambda(
                                    [bRememberChoice]()
                                    {
                                        return *bRememberChoice
                                            ? ECheckBoxState::Checked
                                            : ECheckBoxState::Unchecked;
                                    })
                                .OnCheckStateChanged_Lambda(
                                    [bRememberChoice](ECheckBoxState State)
                                    {
                                        *bRememberChoice = State == ECheckBoxState::Checked;
                                    })
                                [
                                    SNew(STextBlock)
                                        .Text(FText::FromString(TEXT("Remember my choice")))
                                ]
                        ]

                    // Apply
                    + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(5.0f, 0.0f)
                        [
                            SNew(SButton)
                                .Text(FText::FromString(TEXT("Apply")))
                                .OnClicked_Lambda(
                                    [
                                        &Window
                                    ]()
                                    {
                                        Window->RequestDestroyWindow();
                                        return FReply::Handled();
                                    })
                        ]
                ]
        ]
    );

    //Displays Widget in window

    FSlateApplication::Get().AddModalWindow(
        Window,
        nullptr,
        false
    );

    //Copy material at the root of the content browser if needed

    if( *bCopyMaterial && MaterialSelected != nullptr )
    {
        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

        const FString PackagePath = TEXT("/Game");
        const FString AssetName = MaterialSelected->GetName();

        FString UniquePackageName;
        FString UniqueAssetName;

        AssetToolsModule.Get().CreateUniqueAssetName(
            PackagePath / AssetName,
            TEXT(""),
            UniquePackageName,
            UniqueAssetName
        );

        UObject* DuplicatedObject =
            AssetToolsModule.Get().DuplicateAsset(
                UniqueAssetName,
                PackagePath,
                MaterialSelected
            );

        UMaterialInterface* DuplicatedMaterial = Cast<UMaterialInterface>(DuplicatedObject);

        MaterialSelected = DuplicatedMaterial;
    }

    //Save config and return

    Settings->bApplyMaterialWithoutAsking = *bRememberChoice;
    Settings->DefaultMaterial = MaterialSelected;
    Settings->SaveConfig();

    return MaterialSelected;
}

#undef LOCTEXT_NAMESPACE
