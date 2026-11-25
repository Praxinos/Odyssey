// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Unreal headers
#include "Modules/ModuleManager.h"

// No startup code: the default implementation is enough for the module manager to load Ariane,
// now that it is declared in Odyssey.uplugin (a packaged game has no ArianeEditor to pull it in).
IMPLEMENT_MODULE( FDefaultModuleImpl, Ariane );
