// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyUserInteraction.h"

#include "CoreGlobals.h"
#include "Misc/App.h"

namespace Odyssey
{
bool CanInteractWithUser()
{
    return GIsEditor && !IsRunningCommandlet() && !FApp::IsUnattended() && !GIsAutomationTesting;
}
}
