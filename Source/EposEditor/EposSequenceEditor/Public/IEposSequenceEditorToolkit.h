// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Toolkits/AssetEditorToolkit.h"

class ISequencer;

/**
 * Implements an Editor toolkit for epos sequences.
 */
class IEposSequenceEditorToolkit
    : public FAssetEditorToolkit
{
public:

    /**
     * Access the sequencer that is displayed on this asset editor UI
     */
    virtual TSharedPtr<ISequencer> GetSequencer() const = 0;
};
