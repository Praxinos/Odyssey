// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerImageVector.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationLayerImageVector"

UOdysseyAnimationLayerImageVector::~UOdysseyAnimationLayerImageVector()
{

}

UOdysseyAnimationLayerImageVector::UOdysseyAnimationLayerImageVector()
    : mVEngine(nullptr)
    , mScene(nullptr)
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Vector Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
} 

FOdysseyVectorEngine*
UOdysseyAnimationLayerImageVector::GetEngine()
{
    return mVEngine;
}

FOdysseyVectorScene*
UOdysseyAnimationLayerImageVector::GetScene()
{
    return mScene;
}

#undef LOCTEXT_NAMESPACE