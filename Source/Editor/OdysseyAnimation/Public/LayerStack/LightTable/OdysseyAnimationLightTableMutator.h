// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyAnimationLightTable;

class ODYSSEYANIMATION_API FOdysseyAnimationLightTableMutator
    : public FOdysseyMutator
{
public:
    FOdysseyAnimationLightTableMutator(TSharedPtr<FOdysseyAnimationLightTable> iLightTable);

public:
    void SetSourceLayer(UOdysseyAnimationLayer* iLayer);
    void SetDisplayPosition(EOdysseyLightTableDisplayPosition iDisplayPosition);
    void SetDisplayMode(EOdysseyLightTableDisplayMode iDisplayMode);

public:
    void SetKeyIsActivated(int iIndex, bool iIsActivated);
    void SetKeyFrameOffset(int iIndex, int iOffset);
    void SetKeyOpacity(int iIndex, float iOpacity);

private:
    TSharedPtr<FOdysseyAnimationLightTable> mLightTable;
};