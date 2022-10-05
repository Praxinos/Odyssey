// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         ULIS.h
* @author       Clement Berthaud && Eric Scholl
* @brief        This file provides a unique way to include all the necessary library files.
* @license      Please refer to LICENSE.md
*/
#pragma once
// Animation
#include "Animation/AnimatedProperty.h"
#include "Animation/BoundedAnimatedProperty.h"
#include "Animation/HasKeys.h"
#include "Animation/Interpolation/AbstractInterpolation.h"
#include "Animation/Interpolation/BezierInterpolation.h"
#include "Animation/Interpolation/HoldInterpolation.h"
#include "Animation/Interpolation/LinearInterpolation.h"
// Context
#include "Context/Context.h"
// Image
#include "Image/Block.h"
#include "Image/Color.h"
#include "Image/ColorSpace.h"
#include "Image/Format.h"
#include "Image/Pixel.h"
#include "Image/Sample.h"
#include "Image/Gradient.h"
#include "Image/Kernel.h"
#include "Image/StructuringElement.h"
// Layer
#include "Layer/Layer/LayerBuilder.h"
#include "Layer/Layer/LayerTypedefs.h"
#include "Layer/AnimatedLayer/AnimatedLayerBuilder.h"
#include "Layer/AnimatedLayer/AnimatedLayerTypedefs.h"
// Font
#include "Font/Font.h"
#include "Font/FontEngine.h"
// Maths
#include "Math/Geometry/Matrix.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"
#include "Math/Geometry/Morton.h"
#include "Math/Interpolation/Bezier.h"
#include "Math/Interpolation/CatmullRom.h"
#include "Math/Interpolation/Spline.h"
#include "Math/ShapeGeneration/Arc.h"
#include "Math/ShapeGeneration/Circle.h"
#include "Math/ShapeGeneration/CircleInscribedRectangle.h"
#include "Math/ShapeGeneration/Ellipse.h"
#include "Math/ShapeGeneration/Line.h"
#include "Math/ShapeGeneration/Polygon.h"
#include "Math/ShapeGeneration/QuadraticBezier.h"
#include "Math/ShapeGeneration/Rectangle.h"
#include "Math/ShapeGeneration/RotatedEllipse.h"
#include "Math/Math.h"
// Memory
#include "Memory/Units.h"
#include "Memory/Array.h"
#include "Memory/Memory.h"
#include "Memory/ForwardList.h"
#include "Memory/Queue.h"
#include "Memory/Tree.h"
#include "Memory/ContainerAlgorithms.h"
#include "Memory/FixedAllocMemoryPool.h"
#include "Memory/ShrinkableAllocArena.h"
#include "Memory/ShrinkableAllocMemoryPool.h"
// String
#include "String/String.h"
#include "String/WString.h"
// System
#include "System/LibInfo.h"
#include "System/CPUInfo/CPUInfo.h"
#include "System/MemoryInfo/MemoryInfo.h"
#include "System/FilePathRegistry.h"
#include "System/ThreadPool/ThreadPool.h"
// Scheduling
#include "Scheduling/CommandQueue.h"
#include "Scheduling/Event.h"
#include "Scheduling/SchedulePolicy.h"
// Sparse
#include "Sparse/Tile.h"
#include "Sparse/TiledBlock.h"
#include "Sparse/TilePool.h"

