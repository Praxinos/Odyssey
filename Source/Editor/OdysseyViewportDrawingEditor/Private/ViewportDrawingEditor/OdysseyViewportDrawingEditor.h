// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyTextureEditor.h"
#include "OdysseyViewportDrawingEditorGUI.h"

struct FPaintableTexture;
struct FInstanceTexturePaintSettings;
class IMeshPaintGeometryAdapter;

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
    : public FOdysseyTextureEditor
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
    const TArray<UMeshComponent*>& SelectableComponents() const;
    const TArray<FPaintableTexture>& SelectableTextures() const;
    const TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>& ComponentToAdapterMap() const;

public:
    // Setters
    void Actor(AActor* iActor);
    void Component(UMeshComponent* iComponent);

public:
    // Overrides
    virtual FOdysseyViewportDrawingEditorGUI* GetGUI() override;
    virtual void OnPreTextureChange(UTexture2D* iNewTexture) override;
    virtual void OnPostTextureChange(UTexture2D* iOldTexture) override;
    
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
    void SelectDefaultTexture();

private:
	TSharedPtr<FOdysseyViewportDrawingEditorGUI> mGUI;
    AActor* mActor;
    UMeshComponent* mComponent;
    TArray<UMeshComponent*> mSelectableComponents;
    TArray<FPaintableTexture> mSelectableTextures;

	/** Map of geometry adapters for each selectable mesh component */
	TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>> mComponentToAdapterMap; //ES: I don't know exactly what this is

    //This one allows us to remember the selected settings for a given component (like knowing which texture of the component was selected)
	TMap<UMeshComponent*, FInstanceTexturePaintSettings> mComponentToTexturePaintSettingsMap; 
};

