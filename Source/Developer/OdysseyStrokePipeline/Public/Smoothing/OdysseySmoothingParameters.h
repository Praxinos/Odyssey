// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

    bool GetCatchUp() const;
    void SetCatchUp( bool iValue );

protected:
    int mStrength;
    bool mCatchUp;
};
