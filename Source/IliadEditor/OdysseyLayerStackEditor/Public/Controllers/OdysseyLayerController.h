// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

template<class T>
class ODYSSEYLAYERSTACKEDITOR_API TOdysseyLayerController
{
public:
    TOdysseyLayerController(T* iLayer)
        : mLayer(iLayer)
    {
    }

public:
    T* GetLayer() const
    {
        return mLayer;
    }

protected:
    T* mLayer;
};
