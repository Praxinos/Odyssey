// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "LayerStack/FOdysseyLayerStackCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackCommands"

void FOdysseyLayerStackCommands::RegisterCommands()
{
    UI_COMMAND(mMergeDownLayer, "Merge Layer Down", "Merge Layer Down", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND(mDeleteLayer, "Delete Layer", "Delete Layer", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
