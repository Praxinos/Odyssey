// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

/**
 * Holds the UI commands for the OdysseyViewport widget.
 */
class ODYSSEYWIDGETS_API FOdysseyViewportCommands
    : public TCommands<FOdysseyViewportCommands>
{
public:
    /**
     * Default constructor.
     */
    FOdysseyViewportCommands();

public:
    // TCommands interface
    virtual void RegisterCommands() override;

public:
    //Viewport Actions

    /** Action to pan/Zoom the viewport */
    TSharedPtr<FUICommandInfo> PanZoomViewport;

    /** Action to rotate the viewport*/
    TSharedPtr<FUICommandInfo> RotateViewport;

    /** Action to reset the viewport to 100%, 0°, centered */
    TSharedPtr<FUICommandInfo> ResetViewport1On1;
    /** Action to reset the viewport to fit its content to viewport (compute the best zoom) and centered */
    TSharedPtr<FUICommandInfo> ResetViewportFit;
    /** Action to reset the viewport to fit its content to viewport (compute the best zoom), centered, 0°, no flip X/Y */
    TSharedPtr<FUICommandInfo> ResetViewportAll;

    /** Action to reset the viewport to its original position */
    TSharedPtr<FUICommandInfo> ResetViewportPosition;
    /** Action to reset the viewport rotation */
    TSharedPtr<FUICommandInfo> ResetViewportRotation;
    /** Action to reset the viewport zoom */
    TSharedPtr<FUICommandInfo> ResetViewportZoom;

    /** Action to rotate the viewport left */
    TSharedPtr<FUICommandInfo> RotateViewportLeft;

    /** Action to rotate the viewport right */
    TSharedPtr<FUICommandInfo> RotateViewportRight;

    /** Action to set the rotation of the viewport at -135° */
    TSharedPtr<FUICommandInfo> SetRotationMinus135;
    /** Action to set the rotation of the viewport at -90° */
    TSharedPtr<FUICommandInfo> SetRotationMinus90;
    /** Action to set the rotation of the viewport at -45° */
    TSharedPtr<FUICommandInfo> SetRotationMinus45;
    /** Action to set the rotation of the viewport at 0° */
    TSharedPtr<FUICommandInfo> SetRotation0;
    /** Action to set the rotation of the viewport at 45° */
    TSharedPtr<FUICommandInfo> SetRotation45;
    /** Action to set the rotation of the viewport at 90° */
    TSharedPtr<FUICommandInfo> SetRotation90;
    /** Action to set the rotation of the viewport at 135° */
    TSharedPtr<FUICommandInfo> SetRotation135;
    /** Action to set the rotation of the viewport at 180° */
    TSharedPtr<FUICommandInfo> SetRotation180;

    /** Action to flip the viewport horizontally */
    TSharedPtr<FUICommandInfo> FlipViewportHorizontally;

    /** Action to flip the viewport vertically */
    TSharedPtr<FUICommandInfo> FlipViewportVertically;

    /** Action to set zoom percentage at 10% */
    TSharedPtr<FUICommandInfo> SetZoom10Percent;
    /** Action to set zoom percentage at 25% */
    TSharedPtr<FUICommandInfo> SetZoom25Percent;
    /** Action to set zoom percentage at 50% */
    TSharedPtr<FUICommandInfo> SetZoom50Percent;
    /** Action to set zoom percentage at 75% */
    TSharedPtr<FUICommandInfo> SetZoom75Percent;
    /** Action to set zoom percentage at 100% */
    TSharedPtr<FUICommandInfo> SetZoom100Percent;
    /** Action to set zoom percentage at 200% */
    TSharedPtr<FUICommandInfo> SetZoom200Percent;
    /** Action to set zoom percentage at 400% */
    TSharedPtr<FUICommandInfo> SetZoom400Percent;
    /** Action to set zoom percentage at 800% */
    TSharedPtr<FUICommandInfo> SetZoom800Percent;

    /** Action to set zoom at fit to screen */
    TSharedPtr<FUICommandInfo> SetZoomFitScreen;

    /** Action to zoom in*/
    TSharedPtr<FUICommandInfo> ZoomInExponential;

    /** Action to set zoom out*/
    TSharedPtr<FUICommandInfo> ZoomOutExponential;
};
