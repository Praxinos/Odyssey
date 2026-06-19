// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SequenceNameElements.h"

bool
FBoardNameElements::IsValid() const
{
    return Index > INDEX_NONE;
}

bool
FShotNameElements::IsValid() const
{
    return Index > INDEX_NONE && TakeIndex > INDEX_NONE;
}
