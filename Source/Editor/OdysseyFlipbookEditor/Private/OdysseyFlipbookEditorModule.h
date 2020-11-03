// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyFlipbookEditorModule.h"

class FOdysseyFlipbookEditorModule
	: public IOdysseyFlipbookEditorModule
{
public:
    virtual TSharedRef<FOdysseyFlipbookEditorToolkit> CreateOdysseyFlipbookEditor( const EToolkitMode::Type iMode, const TSharedPtr< IToolkitHost >& iInitToolkitHost, UPaperFlipbook* iFlipbook ) override;
    
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
