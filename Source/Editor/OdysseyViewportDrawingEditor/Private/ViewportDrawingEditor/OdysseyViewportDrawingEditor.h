// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyTexture2DEditor.h"
#include "OdysseyViewportDrawingEditorGUI.h"

struct FPaintableTexture;
struct FInstanceTexturePaintSettings;
class IMeshPaintGeometryAdapter;
class FOdysseyViewportDrawingEditorModeToolbar;

/** Struct representing the selected settings for a mesh
 *	It allow us to remember which settings were selected when we come back to a previously selected actor
 */
struct FInstanceTexturePaintSettings
{

    FInstanceTexturePaintSettings()
        : mSelectedTexture(nullptr)
    {}

    FInstanceTexturePaintSettings(UTexture2D* iSelectedTexture)
        : mSelectedTexture(iSelectedTexture)
    {}

    void operator=(const FInstanceTexturePaintSettings& iSrcSettings)
    {
        mSelectedTexture = iSrcSettings.mSelectedTexture;
    }

    UTexture2D* mSelectedTexture;
};

/**
 * Implements an Editor for textures.
 */
class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditor
    : public FOdysseyTexture2DEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditor();
    FOdysseyViewportDrawingEditor();

public:
    // Initialization
    virtual void InitData() override;

public:
    // Getters
    AActor* Actor() const;
    UMeshComponent* Component() const;
    UMaterialInterface* Material() const;
    const TArray<UMeshComponent*>& SelectableComponents() const;
    void  SelectableMaterials( TArray<UMaterialInterface*>& ioSelectedMaterials ) const;
    const TArray<FPaintableTexture>& SelectableTextures() const;
    const TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>& ComponentToAdapterMap() const;
    FOdysseyViewportDrawingEditorModeToolbar* GetToolbar() const;


public:
    // Setters
    void SetActor(AActor* iActor);
    void SetComponent(UMeshComponent* iComponent);
    void SetMaterial(UMaterialInterface* iMaterial);
    void SetTexture(UTexture2D* iTexture);

public:
    // Overrides
    virtual FOdysseyViewportDrawingEditorGUI* GetGUI() override;
    virtual void OnPreTextureChange() override;
    virtual void OnPostTextureChange() override;
    TSharedPtr<FWorkspaceItem> RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    void UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;

private:
    // Listeners
    void OnObjectPropertyChanged(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent);

private:
    // Private Methods
    void ClearSelectableComponents();
    void UpdateSelectableComponents();
    void SelectDefaultComponent();

    void ClearSelectableTextures();
    void UpdateSelectableTextures();
    void SelectDefaultMaterial();
    void SelectDefaultTexture();


private:
    TSharedPtr<FOdysseyViewportDrawingEditorGUI> mGUI;
    TSharedPtr<FOdysseyViewportDrawingEditorModeToolbar> mToolbar;
    AActor* mActor;
    UMeshComponent* mComponent;
    UMaterialInterface* mMaterial; //Storage purposes only for the GUI -> Get path for it
    TArray<UMeshComponent*> mSelectableComponents;
    TArray<FPaintableTexture> mSelectableTextures;

	/** Map of geometry adapters for each selectable mesh component */
	TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>> mComponentToAdapterMap; //ES: I don't know exactly what this is

    //This one allows us to remember the selected settings for a given component (like knowing which texture of the component was selected)
	TMap<UMeshComponent*, FInstanceTexturePaintSettings> mComponentToTexturePaintSettingsMap;

};

