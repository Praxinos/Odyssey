// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "OdysseyTextureWrapper.h"
#include <ULIS3>

class FOdysseySurfaceTexture2DEditable;
class FOdysseyLayerStack;
class UOdysseyTextureAssetUserData;

/** 
 * High level wrapper class to modify or read a UTexture2D
*/
class ODYSSEYTEXTURE_API FOdysseyTexture2DWrapper : public FOdysseyTextureWrapper<UTexture2D>
{
public:
    /** The destructor */
	~FOdysseyTexture2DWrapper();

    /** The constructor */
	FOdysseyTexture2DWrapper(UTexture2D* iTexture);
    
public:
    //Public FOdysseyTextureWrapper interface
    virtual FOdysseyLayerStack* LayerStack() const override;

private:
    //Private FOdysseyTextureWrapper interface

    //Manage Texture Synchronization with Surface
    virtual void UpdateTextureFromSurface() override;

    //Prepares Texture for use in an OdysseyEditor
	virtual void SetTextureProperties() override;

    //Restores Original Texture properties
	virtual void RestoreTextureProperties() override;

    //Destroys properly the surface
    virtual void DestroySurface() override;

    //Creates the appropriate surface
    virtual IOdysseySurfaceEditable* CreateSurface() override;

private:
    //Creates Texture UserData holding the layerstack for example and returns it
    UOdysseyTextureAssetUserData* CreateTextureUserData(UTexture2D* iTexture) const;

private:
    //tmp
    int mPropertyCompressionNone;
};