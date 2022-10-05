// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/FOdysseyLayerStackModel.h"

class IOdysseyBaseLayerNode;

/**
 * The tree structure that handle the content of the layerStack
 */
class FOdysseyLayerStackTree : public TSharedFromThis<FOdysseyLayerStackTree>
{
public: //CONSTRUCTION
    FOdysseyLayerStackTree( FOdysseyLayerStackModel& iLayerStack );

public: //PUBLIC API

    /** Empties the entire tree */
    void Empty();

    /** Updates the tree, returns the current index selected in the layer stack */
    int Update();

    /** Gets the parent layerStack of this tree */
    FOdysseyLayerStackModel& GetLayerStack();

    /** @return The root nodes of the tree */
    const TArray< TSharedRef<IOdysseyBaseLayerNode> >& GetRootNodes() const;

private:
    /** Root nodes */
    TArray< TSharedRef<IOdysseyBaseLayerNode> > mRootNodes;

    /** OdysseyLayerStack interface */
    FOdysseyLayerStackModel& mLayerStack;
};
