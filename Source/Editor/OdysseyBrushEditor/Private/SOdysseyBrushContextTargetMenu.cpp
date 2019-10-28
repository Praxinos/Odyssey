// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SOdysseyBrushContextTargetMenu.h"
#include "Engine/Blueprint.h"
#include "Misc/ConfigCacheIni.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "Components/ActorComponent.h"
#include "EdGraphSchema_OdysseyBrush.h"
#include "K2Node.h"
#include "BlueprintEditorSettings.h"

#define LOCTEXT_NAMESPACE "SOdysseyBrushContextTargetMenu"

/*******************************************************************************
 * FOdysseyContextMenuTargetProfile
 ******************************************************************************/

namespace OdysseyContextMenuTargetProfileImpl
{
    const FString OdysseyConfigSection("BlueprintEditor.ContextTargets");
    const FString OdysseySharedProfileName("SharedMenuProfile");

    /**
     * Query methods to check and see if the user is dragging off a specific pin
     * type (for the context menu).
     *
     * @param  MenuContext    Defines the context from which the menu was requested.
     * @return True if the context contains a pin matching the type in question, otherwise false.
     */
    static bool HasExecPinContext(const FBlueprintActionContext& MenuContext);
    static bool HasComponentPinContext(const FBlueprintActionContext& MenuContext);
    static bool HasObjectPinContext(const FBlueprintActionContext& MenuContext);
    static bool HasClassPinContext(const FBlueprintActionContext& MenuContext);
    static bool HasInterfacePinContext(const FBlueprintActionContext& MenuContext);

    static bool IsObjectPin(UEdGraphPin* Pin);
    static UClass* GetPinClass(UEdGraphPin const* Pin);
    static bool HasAnyExposedComponents(UClass* TargetClass);

    /**
     * Returns a user facing description that provides a short succinct title,
     * discerning this profile from others.
     */
    static FText GetProfileDescription(const FBlueprintActionContext& MenuContext);

    /**
     * Determines which target options are unused, given the current context.
     */
    static uint32 GetIncompatibleTargetFlags(const FBlueprintActionContext& MenuContext, uint32& HasComponentsMaskOut);

    /**
     * Controls how user set targets are saved. Determines how to split context
     * scenarios by providing a separate save name for each.
     */
    static FString GetProfileSaveName(const FBlueprintActionContext& MenuContext);
}

//------------------------------------------------------------------------------
static bool OdysseyContextMenuTargetProfileImpl::HasExecPinContext(const FBlueprintActionContext& MenuContext)
{
    const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
    for (UEdGraphPin* ContextPin : MenuContext.Pins)
    {
        if (K2Schema->IsExecPin(*ContextPin))
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
static bool OdysseyContextMenuTargetProfileImpl::HasComponentPinContext(const FBlueprintActionContext& MenuContext)
{
    const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
    for (UEdGraphPin* ContextPin : MenuContext.Pins)
    {
        UClass* PinClass = GetPinClass(ContextPin);
        if ((PinClass != nullptr) && PinClass->IsChildOf<UActorComponent>())
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
static bool OdysseyContextMenuTargetProfileImpl::HasObjectPinContext(const FBlueprintActionContext& MenuContext)
{
    for (UEdGraphPin* ContextPin : MenuContext.Pins)
    {
        if (IsObjectPin(ContextPin))
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
static bool OdysseyContextMenuTargetProfileImpl::HasClassPinContext(const FBlueprintActionContext& MenuContext)
{
    for (UEdGraphPin* ContextPin : MenuContext.Pins)
    {
        if (ContextPin->PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Class)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
static bool OdysseyContextMenuTargetProfileImpl::HasInterfacePinContext(const FBlueprintActionContext& MenuContext)
{
    for (UEdGraphPin* ContextPin : MenuContext.Pins)
    {
        if (ContextPin->PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Interface)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
static bool OdysseyContextMenuTargetProfileImpl::IsObjectPin(UEdGraphPin* Pin)
{
    return GetPinClass(Pin) != nullptr;
}

//------------------------------------------------------------------------------
static UClass* OdysseyContextMenuTargetProfileImpl::GetPinClass(UEdGraphPin const* Pin)
{
    UClass* PinObjClass = nullptr;

    FEdGraphPinType const& PinType = Pin->PinType;
    if ((PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Object) ||
        (PinType.PinCategory == UEdGraphSchema_OdysseyBrush::PC_Interface))
    {
        bool const bIsSelfPin = !PinType.PinSubCategoryObject.IsValid();
        if (bIsSelfPin)
        {
            PinObjClass = CastChecked<UK2Node>(Pin->GetOwningNode())->GetBlueprint()->SkeletonGeneratedClass;
        }
        else
        {
            PinObjClass = Cast<UClass>(PinType.PinSubCategoryObject.Get());
            if (PinObjClass != nullptr)
            {
                if (UBlueprint* ClassBlueprint = Cast<UBlueprint>(PinObjClass->ClassGeneratedBy))
                {
                    if (ClassBlueprint->SkeletonGeneratedClass != nullptr)
                    {
                        PinObjClass = ClassBlueprint->SkeletonGeneratedClass;
                    }
                }
            }
        }
    }

    return PinObjClass;
}

//------------------------------------------------------------------------------
static bool OdysseyContextMenuTargetProfileImpl::HasAnyExposedComponents(UClass* TargetClass)
{
    for (TFieldIterator<UObjectProperty> PropertyIt(TargetClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
    {
        UObjectProperty* ObjectProperty = *PropertyIt;
        if (!ObjectProperty->HasAnyPropertyFlags(CPF_BlueprintVisible) || !ObjectProperty->PropertyClass->IsChildOf<UActorComponent>())
        {
            continue;
        }

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
FText OdysseyContextMenuTargetProfileImpl::GetProfileDescription(const FBlueprintActionContext& MenuContext)
{
    if (MenuContext.Pins.Num() == 0)
    {
        return LOCTEXT("BlueprintContextTarget", "Blueprint Context Target(s)");
    }
    else if (HasComponentPinContext(MenuContext))
    {
        return LOCTEXT("ComponentContextTarget", "Component Context Target(s)");
    }
//     else if (HasClassPinContext(MenuContext))
//     {
//         return LOCTEXT("ClassContextTarget", "Class Context Target(s)");
//     }
//    else if (HasInterfacePinContext(MenuContext))
//     {
//         return LOCTEXT("InterfaceContextTarget", "Interface Context Target(s)");
//     }
    else if (HasObjectPinContext(MenuContext))
    {
        return LOCTEXT("ObjectContextTarget", "Object Context Target(s)");
    }
    else if (HasExecPinContext(MenuContext))
    {
        return LOCTEXT("ExecContextTarget", "Exec Context Target(s)");
    }
    else
    {
//         const FEdGraphPinType& PinType = MenuContext.Pins[0]->PinType;
//         FText PinTypename = UEdGraphSchema_OdysseyBrush::TypeToText(PinType);
//         return FText::Format(LOCTEXT("TypedPinContextTarget", "{0} Context Target(s)"), PinTypename);

        return LOCTEXT("PODPinContextTarget", "Pin Context Target(s)");
    }
}

//------------------------------------------------------------------------------
uint32 OdysseyContextMenuTargetProfileImpl::GetIncompatibleTargetFlags(const FBlueprintActionContext& MenuContext, uint32& HasComponentsMaskOut)
{
    uint32 IncompatibleFlags = (EOdysseyContextTargetFlags::TARGET_NodeTarget | EOdysseyContextTargetFlags::TARGET_PinObject |
        EOdysseyContextTargetFlags::TARGET_SiblingPinObjects | EOdysseyContextTargetFlags::TARGET_SubComponents);

    TArray<UClass*> BlueprintClasses;

    HasComponentsMaskOut = 0;
    for (UBlueprint* Blueprint : MenuContext.Blueprints)
    {
        UClass* BpClass = Blueprint->SkeletonGeneratedClass;
        BlueprintClasses.Add(BpClass);

        if (HasAnyExposedComponents(BpClass))
        {
            HasComponentsMaskOut |= EOdysseyContextTargetFlags::TARGET_OdysseyBrush;
        }
    }

    const UEdGraphSchema_OdysseyBrush* K2Schema = GetDefault<UEdGraphSchema_OdysseyBrush>();
    for (UEdGraphPin* ContextPin : MenuContext.Pins)
    {
        UEdGraphNode* OwnerNode = ContextPin->GetOwningNodeUnchecked();
        if (OwnerNode != nullptr)
        {
            UEdGraphPin* SelfPin = K2Schema->FindSelfPin(*OwnerNode, EGPD_Input);
            UClass* SelfClass = (SelfPin != nullptr) ? GetPinClass(SelfPin) : nullptr;
            // if this node has a hidden self pin, then that probably means its
            // target is a static lib, or the same as the current blueprint
            bool const bHasUniqueTarget = (SelfPin != nullptr) && !SelfPin->bHidden;// && !BlueprintClasses.Contains(SelfClass);
            // @TODO: check that "self" is different from this blueprint
            if (bHasUniqueTarget)
            {
                IncompatibleFlags &= ~EOdysseyContextTargetFlags::TARGET_NodeTarget;
                if (HasAnyExposedComponents(SelfClass))
                {
                    HasComponentsMaskOut |= EOdysseyContextTargetFlags::TARGET_NodeTarget;
                }
            }

            for (UEdGraphPin* SiblingPin : OwnerNode->Pins)
            {
                UClass* PinClass = GetPinClass(SiblingPin);
                if ((SiblingPin != ContextPin) && (SiblingPin->Direction == EGPD_Output) && (PinClass != nullptr))
                {
                    IncompatibleFlags &= ~EOdysseyContextTargetFlags::TARGET_SiblingPinObjects;
                    if (HasAnyExposedComponents(PinClass))
                    {
                        HasComponentsMaskOut |= EOdysseyContextTargetFlags::TARGET_SiblingPinObjects;
                    }
                }
            }
        }

        if (ContextPin->Direction == EGPD_Input)
        {
            continue;
        }

        if (UClass* PinClass = GetPinClass(ContextPin))
        {
            IncompatibleFlags &= ~EOdysseyContextTargetFlags::TARGET_PinObject;
            if (HasAnyExposedComponents(PinClass))
            {
                HasComponentsMaskOut |= EOdysseyContextTargetFlags::TARGET_PinObject;
            }
        }
    }

    if ((HasComponentsMaskOut != 0) && !HasComponentPinContext(MenuContext))
    {
        IncompatibleFlags &= ~EOdysseyContextTargetFlags::TARGET_SubComponents;
    }

    return IncompatibleFlags;
}


//------------------------------------------------------------------------------
static FString OdysseyContextMenuTargetProfileImpl::GetProfileSaveName(const FBlueprintActionContext& MenuContext)
{
    const UBlueprintEditorSettings* BpSettings = GetDefault<UBlueprintEditorSettings>();
    if (!BpSettings->bSplitContextTargetSettings)
    {
        return OdysseyContextMenuTargetProfileImpl::OdysseySharedProfileName;
    }

    if (MenuContext.Pins.Num() == 0)
    {
        return TEXT("NoPinProfile");
    }
    else if (HasExecPinContext(MenuContext))
    {
        return TEXT("ExecPinProfile");
    }
    else if (HasComponentPinContext(MenuContext))
    {
        return TEXT("ComponentPinProfile");
    }
//     else if (HasClassPinContext(MenuContext))
//     {
//         return TEXT("ClassPinProfile");
//     }
//     else if (HasInterfacePinContext(MenuContext))
//     {
//         return TEXT("InterfacePinProfile");
//     }

    uint32 HasComponentMask = 0;
    uint32 IncompatibleTargetFlags = GetIncompatibleTargetFlags(MenuContext, HasComponentMask);
    return FString::Printf(TEXT("MenuProfile_%d"), IncompatibleTargetFlags);

//     if (MenuContext.Pins.Num() == 0)
//     {
//         return TEXT("NoPin");
//     }
//     else if (OdysseyContextMenuTargetProfileImpl::HasObjectPinContext(MenuContext))
//     {
//         return TEXT("ObjectPin");
//     }
//     else
//     {
//         return TEXT("PODPin");
//     }
}

//------------------------------------------------------------------------------
FOdysseyContextMenuTargetProfile::FOdysseyContextMenuTargetProfile()
    : ProfileSaveName(TEXT("Default"))
    , HasComponentsMask(0)
    , IncompatibleTargetFlags(0)
    , SavedTargetFlags(~IncompatibleTargetFlags)
{
}

//------------------------------------------------------------------------------
FOdysseyContextMenuTargetProfile::FOdysseyContextMenuTargetProfile(const FBlueprintActionContext& MenuContext)
    : ProfileSaveName(OdysseyContextMenuTargetProfileImpl::GetProfileSaveName(MenuContext))
    , HasComponentsMask(0)
    , IncompatibleTargetFlags(OdysseyContextMenuTargetProfileImpl::GetIncompatibleTargetFlags(MenuContext, HasComponentsMask))
    , SavedTargetFlags(~IncompatibleTargetFlags)
{
    const UBlueprintEditorSettings* BpSettings = GetDefault<UBlueprintEditorSettings>();
    FString const BpConfigKey = BpSettings->GetClass()->GetPathName();

    bool bOldUseTargetContextForNodeMenu = true;
    if (GConfig->GetBool(*BpConfigKey, TEXT("bUseTargetContextForNodeMenu"), bOldUseTargetContextForNodeMenu, GEditorPerProjectIni) && !bOldUseTargetContextForNodeMenu)
    {
        SavedTargetFlags = 0;
    }

    if (!LoadProfile())
    {
        // maybe they were originally using the shared context profile? so let's default to that
        FString CachedProfileSaveName = ProfileSaveName;
        ProfileSaveName = OdysseyContextMenuTargetProfileImpl::OdysseySharedProfileName;
        LoadProfile();
        ProfileSaveName = CachedProfileSaveName;
    }
}

//------------------------------------------------------------------------------
uint32 FOdysseyContextMenuTargetProfile::GetContextTargetMask() const
{
    return SavedTargetFlags & ~IncompatibleTargetFlags;
}

//------------------------------------------------------------------------------
void FOdysseyContextMenuTargetProfile::SetContextTarget(EOdysseyContextTargetFlags::Type Flag, bool bClear)
{
    if (bClear)
    {
        SavedTargetFlags &= ~Flag;
    }
    else
    {
        SavedTargetFlags |= Flag;
    }
    SaveProfile();
}

//------------------------------------------------------------------------------
uint32 FOdysseyContextMenuTargetProfile::GetIncompatibleTargetsMask() const
{
    return IncompatibleTargetFlags;
}

//------------------------------------------------------------------------------
bool FOdysseyContextMenuTargetProfile::IsTargetEnabled(EOdysseyContextTargetFlags::Type Flag) const
{
    if (IncompatibleTargetFlags & Flag)
    {
        return false;
    }
    else if (Flag == EOdysseyContextTargetFlags::TARGET_SubComponents)
    {
        return ((SavedTargetFlags|EOdysseyContextTargetFlags::TARGET_OdysseyBrush) & HasComponentsMask) != 0;
    }
    return true;
}

//------------------------------------------------------------------------------
void FOdysseyContextMenuTargetProfile::SaveProfile() const
{
    // want to save with all bits set (except for ones matching flags that
    // currently exist)... this is so we can later add flags, and a user's
    // saved value doesn't immediately disable them (default to on)
    uint32 const GreatestUsedFlag = (EOdysseyContextTargetFlags::ContextTargetFlagsEnd & ~1);
    uint32 const UnusedFlasgMask  = ((0xFFFFFFFF & GreatestUsedFlag) & ~GreatestUsedFlag);
    uint32 const SaveValue = UnusedFlasgMask | SavedTargetFlags;

    GConfig->SetInt(*OdysseyContextMenuTargetProfileImpl::OdysseyConfigSection, *ProfileSaveName, SaveValue, GEditorIni);
}

//------------------------------------------------------------------------------
bool FOdysseyContextMenuTargetProfile::LoadProfile()
{
    int32 SavedFlags = SavedTargetFlags;
    if (GConfig->GetInt(*OdysseyContextMenuTargetProfileImpl::OdysseyConfigSection, *ProfileSaveName, SavedFlags, GEditorIni))
    {
        SavedTargetFlags = SavedFlags;

        uint32 const GreatestUsedFlag = (EOdysseyContextTargetFlags::ContextTargetFlagsEnd & ~1);
        uint32 const LowestUnusedFlag = GreatestUsedFlag << 1;
        // before we saved values with the unused bits all set (to support future
        // flags), we saved only the bits that were explicitly set by the user
        if ((SavedFlags & LowestUnusedFlag) == 0)
        {
            uint32 const OriginalFlagsMask = EOdysseyContextTargetFlags::TARGET_OdysseyBrush | EOdysseyContextTargetFlags::TARGET_SubComponents |
                EOdysseyContextTargetFlags::TARGET_NodeTarget | EOdysseyContextTargetFlags::TARGET_PinObject | EOdysseyContextTargetFlags::TARGET_SiblingPinObjects;
            // add in any new flags that have been added since this profile was last saved
            SavedTargetFlags = (0xFFFFFFFF & ~OriginalFlagsMask) | SavedFlags;

        }
        return true;
    }
    return false;
}

/*******************************************************************************
 * SOdysseyBrushContextTargetMenu
 ******************************************************************************/

/**  */
namespace BlueprintContextTargetMenuImpl
{
    static FText GetContextTargetDisplayName(UEnum* Enum, int32 EnumIndex)
    {
        if (Enum != nullptr)
        {
            return Enum->GetDisplayNameTextByIndex(EnumIndex);
        }
        return LOCTEXT("UnrecognizedTarget", "Error: <UNRECOGNIZED>");
    }
}

//------------------------------------------------------------------------------
void SOdysseyBrushContextTargetMenu::Construct(const FArguments& InArgs, const FBlueprintActionContext& MenuContext)
{
    TargetProfile = FOdysseyContextMenuTargetProfile(MenuContext);
    OnTargetMaskChanged = InArgs._OnTargetMaskChanged;

    FSlateFontInfo HeaderFontStyle = FEditorStyle::GetFontStyle("BlueprintEditor.ActionMenu.ContextDescriptionFont");
    HeaderFontStyle.Size -= 2.f;
    FText const HeaderText = OdysseyContextMenuTargetProfileImpl::GetProfileDescription(MenuContext);

    FText const MenuToolTip = LOCTEXT("MenuToolTip", "Select whose functions/variables you want to see.\nNOTE: Unchecking everything is akin to 'SHOW EVERYTHING' (you're choosing to have NO target context and to not limit the scope)");

    TSharedPtr<SHorizontalBox> MenuBody;
    SBorder::Construct(SBorder::FArguments()
        .BorderImage(FEditorStyle::GetBrush("Menu.Background"))
        .Padding(5.f)
        .ToolTipText(MenuToolTip)
        [
            SNew(SBox)
                .MinDesiredWidth(200)
                .ToolTipText(MenuToolTip)
                .Padding(FMargin(0.f, 0.f, 0.f, 18.f))
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBorder)
                        .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
                        .ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
                    [
                        SNew(STextBlock)
                            .Text(HeaderText)
                            .Font(HeaderFontStyle)
                    ]
                ]

                +SVerticalBox::Slot()
                    .VAlign(VAlign_Fill)
                    .HAlign(HAlign_Fill)
                [
                    SAssignNew(MenuBody, SHorizontalBox)
                        .ToolTipText(MenuToolTip)
                ]
            ]
        ]
    );

    const uint32 ColumnCount = 2;
    TSharedPtr<SVerticalBox> Columns[ColumnCount];

    for (int32 Col = 0; Col < ColumnCount; ++Col)
    {
        MenuBody->AddSlot()
            .AutoWidth()
        [
            SAssignNew(Columns[Col], SVerticalBox)
                .ToolTipText(MenuToolTip)
        ];
    }

    UEnum* const TargetEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EOdysseyContextTargetFlags"));
    uint32 const GreatestFlag = (EOdysseyContextTargetFlags::ContextTargetFlagsEnd & ~1);

    int32  ColIndex = 0;
    for (int32 BitMaskOffset = 0; (1 << BitMaskOffset) <= GreatestFlag; ++BitMaskOffset)
    {
        EOdysseyContextTargetFlags::Type ContextTarget = (EOdysseyContextTargetFlags::Type)(1 << BitMaskOffset);

        if (TargetEnum && TargetEnum->HasMetaData(TEXT("Hidden"), ContextTarget))
        {
            continue;
        }

        FText const MenuName = BlueprintContextTargetMenuImpl::GetContextTargetDisplayName(TargetEnum, BitMaskOffset);
        const FOdysseyContextMenuTargetProfile& ProfileRef = TargetProfile;

        Columns[ColIndex]->AddSlot()
            .AutoHeight()
            .VAlign(VAlign_Top)
            .Padding(3.f, 2.5f)
        [
            SNew(SCheckBox)
                .IsEnabled_Raw(&TargetProfile, &FOdysseyContextMenuTargetProfile::IsTargetEnabled, ContextTarget)
                .IsChecked(this, &SOdysseyBrushContextTargetMenu::GetTargetCheckedState, ContextTarget)
                .OnCheckStateChanged(this, &SOdysseyBrushContextTargetMenu::OnTargetCheckStateChanged, ContextTarget)
                .ToolTipText_Lambda([TargetEnum, BitMaskOffset, ContextTarget, &ProfileRef]()->FText
                {
                    if (!ProfileRef.IsTargetEnabled(ContextTarget))
                    {
                        return LOCTEXT("DisabledTargetTooltip", "This target is invalid or redundant for this context.");
                    }
                    else if (TargetEnum != nullptr)
                    {
                        return TargetEnum->GetToolTipTextByIndex(BitMaskOffset);
                    }
                    return LOCTEXT("GenericTargetTooltip", "Include variables/functions that belong to this target.");
                })
            [
                SNew(STextBlock).Text(MenuName)
            ]
        ];


        ColIndex = (ColIndex + 1) % ColumnCount;
    }
}

//------------------------------------------------------------------------------
uint32 SOdysseyBrushContextTargetMenu::GetContextTargetMask() const
{
    return TargetProfile.GetContextTargetMask();
}

//------------------------------------------------------------------------------
void SOdysseyBrushContextTargetMenu::OnTargetCheckStateChanged(const ECheckBoxState NewCheckedState, EOdysseyContextTargetFlags::Type ContextTarget)
{
    TargetProfile.SetContextTarget(ContextTarget, (NewCheckedState != ECheckBoxState::Checked));
    OnTargetMaskChanged.ExecuteIfBound(TargetProfile.GetContextTargetMask());
}

//------------------------------------------------------------------------------
ECheckBoxState SOdysseyBrushContextTargetMenu::GetTargetCheckedState(EOdysseyContextTargetFlags::Type ContextTarget) const
{
    return ((TargetProfile.GetContextTargetMask() & ContextTarget) != 0) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE
