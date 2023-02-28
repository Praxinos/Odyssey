// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaPlayerFactory.h"
#include "Modules/ModuleInterface.h"

/**
 * The Animation Asset module.
 */
class FOdysseyAnimationModule
	: public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	// End of IModuleInterface interface

    void RegisterMedia();
    void UnregisterMedia();

	void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();

public:
    TSharedPtr<IAssetTypeActions> mIliadTypeActions;
    FOdysseyAnimationMediaPlayerFactory mAnimationMediaPlayerFactory;
};
