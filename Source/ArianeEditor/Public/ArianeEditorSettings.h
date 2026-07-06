// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "ArianeEditorSettings.generated.h"


/**
 * Implements the Editor's user settings.
 */
UCLASS(config=Ariane, defaultconfig)
class ARIANEEDITOR_API UArianeEditorSettings : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    static UArianeEditorSettings* Get();

public:
#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

public:
    DECLARE_MULTICAST_DELEGATE( FOnGridSizeChanged );

    /** Gets the layer grid size. */
    double GetGridSize() const;
    /** Sets the layer grid size. */
    void SetGridSize( double InGridSize );
    /** Gets the layer grid opacity. */
    double GetGridOpacity() const;
    /** Sets the layer grid opacity. */
    void SetGridOpacity( double InGridOpacity );
    /** Gets the layer grid's color. */
    FLinearColor GetGridColor() const;
    /** Sets the layer grid color. */
    void SetGridColor( FLinearColor InGridColor );
    /** Gets the layer grid X-Axis color. */
    FLinearColor GetGridXAxisColor() const;
    /** Sets the layer grid X-Axis color. */
    void SetGridXAxisColor( FLinearColor InGridXAxisColor );
    /** Gets the layer grid Y-Axis color. */
    FLinearColor GetGridYAxisColor() const;
    /** Sets the layer grid Y-Axis color. */
    void SetGridYAxisColor( FLinearColor InGridYAxisColor );
    /** Gets the distance to a new Paintingg 3D Actor. */
    double GetDistanceToNewActor() const;
    /** Sets the distance to a new Paintingg 3D Actor. */
    void SetDistanceToNewActor( double InDistanceToNewActor );
    /** Gets the HUD foreground color. */
    FColor GetHUDForegroundColor() const;
    /** Sets the default matezrial to use for the path drawing tool */
    void SetDefaultPathDrawingMaterial( UMaterial* InDefaultPathDrawingMaterial );
    /** Gets the default matezrial used by the path drawing tool */
    UMaterial* GetDefaultPathDrawingMaterial() const;

    //FOnGridSizeChanged& GetOnGridSizeChanged();

private:
    /** The distance to a newly created Painting 3D Actor. */
    UPROPERTY(config, EditAnywhere, Category=General, DisplayName = "The distance to the camera when a new Painting 3D Actor is created")
    double DistanceToNewActor;

    UPROPERTY(EditAnywhere, Category=General, DisplayName = "The material to use by default for the drawing tool")
    UMaterial* DefaultPathDrawingMaterial;

    /** The layer grid size. */
    UPROPERTY(config, EditAnywhere, Category=Grid, DisplayName = "The layer grid size")
    double GridSize;

    /** The layer grid opacity. */
    UPROPERTY(config, EditAnywhere, Category=Grid, DisplayName = "The layer grid opacity")
    double GridOpacity;

    /** The layer grid color. */
    UPROPERTY(config, EditAnywhere, Category=Grid, DisplayName = "The layer grid color")
    FLinearColor GridColor;

    /** The layer grid X-Axis color. */
    UPROPERTY(config, EditAnywhere, Category=Grid, DisplayName = "The layer grid X-Axis color")
    FLinearColor GridXAxisColor;

    /** The layer grid Y-Axis color. */
    UPROPERTY(config, EditAnywhere, Category=Grid, DisplayName = "The layer grid Y-Axis color")
    FLinearColor GridYAxisColor;

    /** HUD Foreground Color */
    UPROPERTY(config, EditAnywhere, Category=Grid, DisplayName = "The HUD foreground Color")
    FColor HUDForegroundColor;

private:
    //FOnGridSizeChanged OnGridSizeChanged;
};
