// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

enum class EOdysseyRenderingType
{
    Render, //renders only the expected final render result
    RenderOutOfPegs,
    Editor, //renders what is expected in an editor (can render the animation lighttable for example)
};
