// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/FOdysseyLayerStackModel.h"
#include "OdysseyImageLayer.h"

class FOdysseyLayerStackModel;

/**
 * Implements each layer menu GUI
 */
class IOdysseyLayerEditor
{
public:
    explicit IOdysseyLayerEditor( TSharedRef<FOdysseyLayerStackModel> InLayerStack )
        : LayerStackRef(InLayerStack)
    {
    }

    /** Virtual destructor. */
    virtual ~IOdysseyLayerEditor() { }
public:
    /**
     * Add a new layer to the stack.
     */
    virtual FOdysseyImageLayer* AddLayer(IOdysseyLayer* FocusedLayer) = 0;

    /**
     * Builds up the Layer Stack Widget "Add Layer" menu.
     *
     * @param MenuBuilder The menu builder to change.
     */
    virtual void BuildAddLayerMenu(FMenuBuilder& MenuBuilder) = 0;

    /**
     * Builds the context menu for the layer.
     * @param MenuBuilder The menu builder to use to build the layer menu.
     */
    //virtual void BuildLayerContextMenu( FMenuBuilder& MenuBuilder, FOdysseyImageLayer* Layer ) = 0;


    /** Gets an icon brush for this layer editor */
    virtual const FSlateBrush* GetIconBrush() const { return nullptr; }

    /** Called when the instance of this layer editor is initialized */
    //virtual void OnInitialize() = 0;

    /** Called when the instance of this layer editor is released */
    //virtual void OnRelease() = 0;

    /** Allows the Layer editor to paint on a layer area. */
    //virtual int32 PaintTrackArea(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle) = 0;

    /**
     * Ticks this tool.
     *
     * @param DeltaTime The time since the last tick.
     */
    //virtual void Tick(float DeltaTime) = 0;

    /**
     * @return Whether this layer editor handles resize events
     */
    virtual bool IsResizable(FOdysseyImageLayer* InLayer) const
    {
        return false;
    }

    /**
     * Resize this Layer editor
     */
    virtual void Resize(float NewSize, FOdysseyImageLayer* InTrack)
    {

    }

    /**
     * @return The default expansion state of this layer editor
     */
    virtual bool GetDefaultExpansionState(FOdysseyImageLayer* InTrack) const
    {
        return false;
    }



protected:
    TSharedRef<FOdysseyLayerStackModel> LayerStackRef;

};

