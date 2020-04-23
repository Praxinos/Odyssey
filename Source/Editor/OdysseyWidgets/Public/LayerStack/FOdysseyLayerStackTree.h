// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/FOdysseyLayerStackModel.h"

class IOdysseyBaseLayerNode;

/**
 * The tree structure that handle the content of the layerStack
 */
class FOdysseyLayerStackTree : public TSharedFromThis<FOdysseyLayerStackTree>
{
public:
    DECLARE_MULTICAST_DELEGATE(FOnUpdated);

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



public: //DELEGATES
    /** Gets a multicast delegate which is called whenever the node tree has been updated.*/
    FOnUpdated& OnUpdated() { return mOnUpdatedDelegate; }



private:
    /** Root nodes */
    TArray< TSharedRef<IOdysseyBaseLayerNode> > mRootNodes;

    /** OdysseyLayerStack interface */
    FOdysseyLayerStackModel& mLayerStack;

    /** A multicast delegate which is called whenever the node tree has been updated. */
    FOnUpdated mOnUpdatedDelegate;
};
