// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

namespace EOdysseyRenderingType
{
    enum Type
    {
        Render = 0, //renders only the expected final render result
        Editor = 1 << 0, //renders what is expected in an editor (can render the animation lighttable for example)
        OutOfPegs = 1 << 1, //explicitly asks to render the cells using their out of pegs transform
    };
}
