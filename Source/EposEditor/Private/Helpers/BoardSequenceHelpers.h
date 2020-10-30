// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

#include "ArrangeSectionsType.h"

class ISequencer;

class EPOSEDITOR_API BoardSequenceHelpers
{
public:
    /**
    *  Arrange Sections of the board track
    *
    * @param ISequencer* iSequencer to manage sections.
    */
    static void ArrangeSections( ISequencer* iSequencer );

    /**
    *  Set the arrange section settings
    *
    * @param ISequencer* iSequencer to manage sections.
    * @param EArrangeSections iArrangeSections is the new value.
    */
    static void SetArrangeSections( ISequencer* iSequencer, EArrangeSections iArrangeSections );
};
