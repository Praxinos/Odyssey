// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "IOdysseyTextureEditorModule.h"

class FOdysseyTextureEditorModule
	: public IOdysseyTextureEditorModule
{
public:
    virtual TSharedRef<FOdysseyTextureEditorToolkit> CreateOdysseyTextureEditor( const EToolkitMode::Type iMode, const TSharedPtr< IToolkitHost >& iInitToolkitHost, UTexture2D* iTexture ) override;

	TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override;

public:
    // IModuleInterface interface
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;
    
private:
    void RegisterAssetTypeAction(IAssetTools& ioAssetTools, TSharedRef<IAssetTypeActions> iAction);

    
private:
	/** All created asset type actions. Cached here so that we can unregister them during shutdown. */
	TArray< TSharedPtr<IAssetTypeActions> > mCreatedAssetTypeActions;

	EAssetTypeCategories::Type mOdysseyPainterCategory;

	// Holds the menu extensibility manager.
	TSharedPtr<FExtensibilityManager> mMenuExtensibilityManager;
};
