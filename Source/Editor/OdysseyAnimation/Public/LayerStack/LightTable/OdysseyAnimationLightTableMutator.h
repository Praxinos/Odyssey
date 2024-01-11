// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyAnimationLightTable;

class ODYSSEYANIMATION_API FOdysseyAnimationLightTableMutator
    : public FOdysseyMutator
{
public:
    virtual ~FOdysseyAnimationLightTableMutator() {};
    FOdysseyAnimationLightTableMutator(TSharedPtr<FOdysseyAnimationLightTable> iLightTable);

public:
    void SetDisplayPosition(EOdysseyLightTableDisplayPosition iDisplayPosition);

public:
    void SetKeyIsActivated(int iIndex, bool iIsActivated);
    void SetKeyOpacity(int iIndex, float iOpacity);
    void SetNextKeysColor(const FLinearColor& iColor);
    void SetPreviousKeysColor(const FLinearColor& iColor);
    void SetNextKeysContrast(float iContrast);
    void SetPreviousKeysContrast(float iContrast);

private:
    TSharedPtr<FOdysseyAnimationLightTable> mLightTable;
};