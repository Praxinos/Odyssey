// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

/**
 * Implements the smoothing for a Bezier 3 Points
 */
class ODYSSEYSTROKEPIPELINE_API FOdysseySmoothingParameters
{
public:
    ~FOdysseySmoothingParameters();
    FOdysseySmoothingParameters();

public:
    int GetStrength() const;
    void SetStrength( int iValue );

protected:
    int mStrength;
};
