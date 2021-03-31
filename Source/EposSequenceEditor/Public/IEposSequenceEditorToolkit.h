// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
