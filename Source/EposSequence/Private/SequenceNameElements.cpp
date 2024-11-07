// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
