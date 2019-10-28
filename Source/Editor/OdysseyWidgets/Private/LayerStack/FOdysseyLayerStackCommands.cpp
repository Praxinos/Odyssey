// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "LayerStack/FOdysseyLayerStackCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackCommands"

void FOdysseyLayerStackCommands::RegisterCommands()
{
    UI_COMMAND( TestOption, "Test Option", "A Test Option", EUserInterfaceActionType::Button, FInputChord(EKeys::SpaceBar) );

    UI_COMMAND(MergeDownLayer, "Merge Layer Down", "Merge Layer Down", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND(DeleteLayer, "Delete Layer", "Delete Layer", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
