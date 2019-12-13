// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBorder.h"
#include "OdysseyBrushActionMenuUtils.h"

enum class ECheckBoxState : uint8;

/*******************************************************************************
 * FOdysseyContextMenuTargetProfile
 ******************************************************************************/

/** Used internally by SOdysseyBrushContextTargetMenu, to track and save the user's context target settings */
struct FOdysseyContextMenuTargetProfile
{
public:
    FOdysseyContextMenuTargetProfile();
    FOdysseyContextMenuTargetProfile(const FBlueprintActionContext& MenuContext);

    uint32 GetContextTargetMask() const;
    void   SetContextTarget(EOdysseyContextTargetFlags::Type Flag, bool bClear = false);
    uint32 GetIncompatibleTargetsMask() const;

    bool   IsTargetEnabled(EOdysseyContextTargetFlags::Type Flag) const;

private:
    void SaveProfile() const;
    bool LoadProfile();

    FString ProfileSaveName;
    uint32  HasComponentsMask;
    uint32  IncompatibleTargetFlags;
    uint32  SavedTargetFlags;
};

/*******************************************************************************
 * SOdysseyBrushContextTargetMenu
 ******************************************************************************/

/**  */
class SOdysseyBrushContextTargetMenu : public SBorder
{
    DECLARE_DELEGATE_OneParam(FOnTargetMaskChanged, uint32);

public:
    SLATE_BEGIN_ARGS(SOdysseyBrushContextTargetMenu){}
        SLATE_EVENT(FOnTargetMaskChanged, OnTargetMaskChanged)
    SLATE_END_ARGS()

    // SWidget interface
    void Construct(const FArguments& InArgs, const FBlueprintActionContext& MenuContext);
    // End SWidget interface

    /**
     * Returns a EOdysseyContextTargetFlags::Type bitmask, defining which targets the
     * user currently has enabled.
     */
    uint32 GetContextTargetMask() const;

private:
    /**
     * Internal UI callback that handles when one of the context targets is
     * checked/unchecked.
     *
     * @param  NewCheckedState    The checkbox's new state.
     * @param  ContextTarget    A flag identifying which checkbox was altered.
     */
    void OnTargetCheckStateChanged(const ECheckBoxState NewCheckedState, EOdysseyContextTargetFlags::Type ContextTarget);

    /**
     * Internal UI callback that determines the checkbox state for one of the
     * context target options.
     *
     * @param  ContextTarget    A flag identifying which checkbox we want the state of.
     * @return ECheckBoxState::Checked if the user has the flag set, otherwise ECheckBoxState::UnChecked
     */
    ECheckBoxState GetTargetCheckedState(EOdysseyContextTargetFlags::Type ContextTarget) const;

    /** Determines what flags are incompatible with the current context, and saves/loads the user's choice settings */
    FOdysseyContextMenuTargetProfile TargetProfile;
    /** Delegate for external users to hook into (so they can act when the menu's settings are changed). */
    FOnTargetMaskChanged OnTargetMaskChanged;
};
