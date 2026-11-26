// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

namespace Odyssey::TiledImageMigration
{
/**
 * Shows the legacy packages of the project and its plugins by group, and converts the groups the user chooses, then shows
 * what the conversion did; or records that the user no longer wants the startup offer.
 * Tells the user when there is nothing to convert, and refuses to run while the asset registry scans or during Play In Editor.
 * Game thread; the editor must be able to interact with the user.
 */
void Run();
}
