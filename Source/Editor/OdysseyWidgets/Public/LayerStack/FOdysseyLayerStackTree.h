// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/FOdysseyLayerStackModel.h"

class OdysseyBaseLayerNode;

/**
 * The tree structure that handle the content of the layerStack
 */
class FOdysseyLayerStackTree : public TSharedFromThis<FOdysseyLayerStackTree>
{
public:
    DECLARE_MULTICAST_DELEGATE(FOnUpdated);

public:
    FOdysseyLayerStackTree( class FOdysseyLayerStackModel& InLayerStack )
        : LayerStack(InLayerStack)
    {}



public: //PUBLIC API

    /**
     * Empties the entire tree
     */
    void Empty();

    /**
     * Updates the tree, returns the current index selected in the layer stack
     */
    int Update();

    /** Gets the parent layerStack of this tree */
    FOdysseyLayerStackModel& GetLayerStack() {return LayerStack;}

    /**
     * @return The root nodes of the tree
     */
    const TArray< TSharedRef<OdysseyBaseLayerNode> >& GetRootNodes() const;

    /**
     * Set the single hovered node in the tree
     */
    void SetHoveredNode(const TSharedPtr<OdysseyBaseLayerNode>& InHoveredNode);

    /**
     * Get the single hovered node in the tree, possibly nullptr
     */
    const TSharedPtr<OdysseyBaseLayerNode>& GetHoveredNode() const;



public: //DELEGATES
    /*
     * Gets a multicast delegate which is called whenever the node tree has been updated.
     */
    FOnUpdated& OnUpdated() { return OnUpdatedDelegate; }



private:
    /** Root nodes */
    TArray< TSharedRef<OdysseyBaseLayerNode> > RootNodes;

    /** OdysseyLayerStack interface */
    FOdysseyLayerStackModel& LayerStack;

    /** Cardinal hovered node */
    TSharedPtr<OdysseyBaseLayerNode> HoveredNode;

    /** A multicast delegate which is called whenever the node tree has been updated. */
    FOnUpdated OnUpdatedDelegate;
};
