// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Misc/NotNull.h"

class UPackage;

/**
 * Tells the user which packages were converted from a legacy Odyssey storage while loading, so that they save them.
 *
 * Shortly after no package is loading anymore, each reported package is marked dirty, and one notification lists the
 * packages reported meanwhile. It stays in the active window until the user saves the packages from it or dismisses it.
 * Nothing is reported in commandlets, unattended sessions or automation tests.
 * Game thread only.
 */
class ODYSSEYIMAGING_API FOdysseyLegacyMigrationNotifier
{
public:
    /** Reports a package whose content was converted while loading. A package reported several times is listed once. */
    static void Add(TNotNull<UPackage*> Package);

    /** Drops the reports not notified yet. */
    static void Shutdown();
};
