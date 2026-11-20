// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyHUDElement.h"

/////////////////////////////////////////////////////
// FOdysseyHUDEllipse
class ODYSSEYHUD_API FOdysseyHUDEllipse : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDEllipse();

    //Constructor
    FOdysseyHUDEllipse( const FVector2D& iCenter, int iXRadius, int iYRadius);

public:
    virtual void DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams) override;

public:
    void SetCenter(const FVector2D& iCenter);
    void SetXRadius(int iRadius);
    void SetYRadius(int iRadius);
    void SetRotation(float iRotationDegrees);
    float GetRotation() const;
    void SetColor(const FLinearColor& iColor);
    const FLinearColor& GetColor() const;
    void SetOpacity(float iOpacity);
    float GetOpacity() const;
    void SetThickness(float iThickness);
    float GetThickness() const;

    const FVector2D& GetCenter() const;
    int GetXRadius() const;
    int GetYRadius() const;

private:
    FVector2D mCenter;
    int mXRadius;
    int mYRadius;
    float mRotationDegrees = 0.f;
    FLinearColor mColor = FLinearColor::Black;
    float mOpacity = 1.f;
    float mThickness = 1.f;
};
