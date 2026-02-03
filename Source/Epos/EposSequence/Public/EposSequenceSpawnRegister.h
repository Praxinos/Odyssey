// IDDN.FR.000.000000.000.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "LevelSequenceSpawnRegister.h"

#define UE_API EPOSSEQUENCE_API

/** Movie scene spawn register that knows how to handle spawning objects (actors) for a board/shot sequence  */
class FEposSequenceSpawnRegister
    : public FLevelSequenceSpawnRegister
{
public:

    UE_API FEposSequenceSpawnRegister();
};

#undef UE_API
