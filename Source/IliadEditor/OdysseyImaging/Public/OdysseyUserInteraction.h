// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

namespace Odyssey
{
/**
 * Returns true when the editor may ask something of the user, e.g. show a notification or a window:
 * false in commandlets, unattended sessions and automation tests.
 */
ODYSSEYIMAGING_API bool CanInteractWithUser();
}
