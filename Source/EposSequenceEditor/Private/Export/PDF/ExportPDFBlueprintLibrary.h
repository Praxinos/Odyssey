// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Export/ExportStruct.h"
#include "SequenceNameElements.h"

#include "ExportPDFBlueprintLibrary.generated.h"

//---

UCLASS()
class /*EPOSSEQUENCEEDITOR_API*/ UExportPDFBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Get the name of the storyboard
     *
     * @param The list of panels
     * @return The name of the storyboard
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static FText GetRootBoardName( const FExportStruct& ExportStruct );

    /**
     * Get the name of the studio
     *
     * @param The list of panels
     * @return The name of the studio
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static TArray<FSequenceNameElements> GetNameElementsOfPanels( const FExportStruct& ExportStruct );

public:
    /**
     * Get the number of panels
     *
     * @param The list of panels
     * @return The number of panels
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static int32 GetNumberOfPanels( const FExportStruct& ExportStruct );

    /**
     * Get the frame number of a panel as formatted string
     *
     * @param The list of panels
     * @param The index of the desired panel
     * @return The frame number as string (manage frame/timecode/second settings)
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static FString GetPanelFrameFormatted( const FExportStruct& ExportStruct, int32 PanelIndex );

    /**
     * Get the frame number of a panel
     *
     * @param The list of panels
     * @param The index of the desired panel
     * @return The frame number (in tick resolution)
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static FFrameNumber GetPanelFrame( const FExportStruct& ExportStruct, int32 PanelIndex );

    /**
     * Get the frame number of a panel
     *
     * @param The list of panels
     * @param The index of the desired panel
     * @return The frame number (in tick resolution)
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static int32 GetPanelDuration( const FExportStruct& ExportStruct, int32 PanelIndex );

    /**
     * Get the shot name of a panel
     *
     * @param The list of panels
     * @param The index of the desired panel
     * @return The name of the shot
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static FText GetPanelShotName( const FExportStruct& ExportStruct, int32 PanelIndex );

    /**
     * Get the shot name of a panel
     */
    // I don't know if it's a good idea to give access on the sequence and maybe modifying it during the loop ...
    //UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    //static const UMovieSceneSequence* GetPanelSequence( const FExportStruct& ExportStruct, int32 PanelIndex );

    /**
     * Get the render scene of a panel
     * The width of the texture is computed to match the camera ratio
     *
     * @param The list of panels
     * @param The index of the desired panel
     * @param The height (in px) of the wanted texture
     * @return The texture containing the scene
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static const UTexture2D* GetPanelTexture2D( const FExportStruct& ExportStruct, int32 PanelIndex, int32 Height, EViewModeIndex ViewMode = EViewModeIndex::VMI_Lit );

    /**
     * Get the notes of a panel
     * The notes are taken through the whole hierarchy
     *
     * @param The list of panels
     * @param The index of the desired panel
     * @return The array of all notes at the frame of the panel
     */
    UFUNCTION( BlueprintPure, Category="Epos Sequence Editor|Export PDF" )
    static TArray<UStoryNote*> GetPanelNotes( const FExportStruct& ExportStruct, int32 PanelIndex );
};
