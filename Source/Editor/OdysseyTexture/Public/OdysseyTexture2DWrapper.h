// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

#include "OdysseyTextureWrapper.h"
#include "OdysseySurfaceTexture2DEditable.h"

#include <ULIS>

class FOdysseyLayerStack;
class UOdysseyTextureAssetUserData;

/** 
 * High level wrapper class to modify or read a UTexture2D
*/
class ODYSSEYTEXTURE_API FOdysseyTexture2DWrapper : public FOdysseyTextureWrapper
{
public:
    /** The destructor */
	~FOdysseyTexture2DWrapper();

    /** The constructor */
	FOdysseyTexture2DWrapper(UTexture2D* iTexture);

public:
    //Public FOdysseyTextureWrapper interface
    virtual UTexture2D* Texture() const override;
    virtual FOdysseySurfaceTexture2DEditable* Surface() const override;
    virtual FOdysseyLayerStack* LayerStack() const override;

public:
    //Setters
    void SetTexture(UTexture2D* iTexture);

public:
    //Public FOdysseyTextureWrapper interface
    virtual void Finalize() override;

private:
    //Private FOdysseyTextureWrapper interface

    //Manage Texture Synchronization with Surface
    virtual void UpdateTextureFromSurface() override;

    //Prepares Texture for use in an OdysseyEditor
	virtual void SetTextureProperties() override;

    //Restores Original Texture properties
	virtual void RestoreTextureProperties() override;

    //Surface Initialization
    virtual void InitializeSurface()  override;

    //Surface Finalization
    virtual void FinalizeSurface() override;

    //LayerStack Initialization
    virtual void InitializeLayerStack() override;

    //LayerStack Finalization
    virtual void FinalizeLayerStack() override;

private:
    //Creates Texture UserData holding the layerstack for example and returns it
    UOdysseyTextureAssetUserData* FindOrCreateTextureUserData() const;

private:
    UTexture2D* mTexture;
    FOdysseySurfaceTexture2DEditable* mSurface;
    FOdysseyLayerStack* mLayerStack;

    //tmp
    int mPropertyCompressionNone;
};