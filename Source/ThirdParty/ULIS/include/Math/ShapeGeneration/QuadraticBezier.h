// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         QuadraticBezier.h
* @author       Thomas Schmitt
* @brief        This file provides the Quadratic Bezier points generation methods
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN

static inline void InternalGenerateQuadraticBezierSegPoints(
      int x0
    , int y0
    , int x1
    , int y1
    , int x2
    , int y2
    , float w
    , TArray<FVec2I>& ioQuadraticBezierPoints
    , bool iInvertArray //If true, we invert the array we would have normally by inserting or pushing back elements correctly
    , int iCustomIndexToPush = 0
)
{
    int sx = x2-x1,sy = y2-y1;
    double dx = x0-x2,dy = y0-y2,xx = x0-x1,yy = y0-y1;
    double xy = xx*sy+yy*sx,cur = xx*sy-yy*sx,err;
    bool pushBack = true;
    int indexEndArray = static_cast< int >( ioQuadraticBezierPoints.Size() );

    if(xx*sx > 0.0 || yy*sy > 0.0)
    {
        return;
    }

    if(cur != 0.0 && w > 0.0) 
    {
        if(sx*(long)sx+sy*(long)sy > xx*xx+yy*yy) 
        {
            pushBack = !pushBack;
            x2 = x0; x0 -= int(dx); y2 = y0; y0 -= int(dy); cur = -cur;
        }
        xx = 2.0*(4.0*w*sx*xx+dx*dx);
        yy = 2.0*(4.0*w*sy*yy+dy*dy);
        sx = x0 < x2 ? 1 : -1;
        sy = y0 < y2 ? 1 : -1;
        xy = -2.0*sx*sy*(2.0*w*xy+dx*dy);
        if(cur*sx*sy < 0.0)
        {
            xx = -xx; yy = -yy; xy = -xy; cur = -cur;
        }
        dx = 4.0*w*(x1-x0)*sy*cur+xx/2.0+xy;
        dy = 4.0*w*(y0-y1)*sx*cur+yy/2.0+xy;
        if(w < 0.5 && dy > dx)
        {
            cur = -(w+1.0)/2.0;
            w = sqrt(w);
            xy = 1.0/(w+1.0);
            sx = int(floor((x0+2.0*w*x1+x2)*xy/2.0+0.5));
            sy = int(floor((y0+2.0*w*y1+y2)*xy/2.0+0.5));
            dx = floor((w*x1+x0)*xy+0.5);
            dy = floor((y1*w+y0)*xy+0.5);
            InternalGenerateQuadraticBezierSegPoints(x0,y0,int(dx),int(dy),sx,sy,float(cur),ioQuadraticBezierPoints, iInvertArray, iCustomIndexToPush);
            dx = floor((w*x1+x2)*xy+0.5);
            dy = floor((y1*w+y2)*xy+0.5);
            InternalGenerateQuadraticBezierSegPoints(sx,sy,int(dx),int(dy),x2,y2,float(cur),ioQuadraticBezierPoints, iInvertArray, iCustomIndexToPush);
            return;
        }
        err = dx+dy-xy;
        
        while(dy <= xy && dx >= xy)
        {
            if( pushBack ) 
                if( !iInvertArray)
                    ioQuadraticBezierPoints.PushBack(FVec2I(x0, y0));
                else
                    ioQuadraticBezierPoints.Insert( iCustomIndexToPush, FVec2I(x0, y0) );
            else //if pushBack
                if (!iInvertArray)
                    ioQuadraticBezierPoints.Insert( indexEndArray, FVec2I(x0, y0) );
                else
                    ioQuadraticBezierPoints.Insert( iCustomIndexToPush++, FVec2I(x0, y0) );

            if(x0 == x2 && y0 == y2)
                return;
            x1 = 2 * err > dy; y1 = 2*(err + yy) < -dy;
            if(2 * err < dx || y1) { y0 += sy; dy += xy; err += dx += xx; }
            if(2 * err > dx || x1) { x0 += sx; dx += xy; err += dy += yy; }
        }
    }

    if( pushBack ) 
        if( !iInvertArray)
            GenerateLinePoints(FVec2I(x0, y0), FVec2I(x2, y2), ioQuadraticBezierPoints );
        else
            GenerateLinePoints(FVec2I(x0, y0), FVec2I(x2, y2), ioQuadraticBezierPoints, iCustomIndexToPush);
    else //if pushBack
        if (!iInvertArray)
            GenerateLinePoints(FVec2I(x0, y0), FVec2I(x2, y2), ioQuadraticBezierPoints, indexEndArray);
        else
            GenerateLinePoints(FVec2I(x2, y2), FVec2I(x0, y0), ioQuadraticBezierPoints, iCustomIndexToPush); //But reversed
}

static inline void GenerateQuadraticBezierPoints(
      const FVec2I&                   iCtrlPt0
    , const FVec2I&                   iCtrlPt1
    , const FVec2I&                   iCtrlPt2
    , const float                     iWeight
    , TArray<FVec2I>& ioQuadraticBezierPoints
    )
{
    int x = iCtrlPt0.x - 2 * iCtrlPt1.x + iCtrlPt2.x;
    int y = iCtrlPt0.y - 2 * iCtrlPt1.y + iCtrlPt2.y;
    double dx = iCtrlPt0.x - iCtrlPt1.x;
    double dy = iCtrlPt0.y - iCtrlPt1.y;
    double dWeight;
    double dt;
    double dq;

    FVec2I pt0 = iCtrlPt0;
    FVec2I pt1 = iCtrlPt1;
    FVec2I pt2 = iCtrlPt2;

    float weight = iWeight;

    bool invertArray = false;

    if(weight < 0) //Can't draw a bezier curve with a weight < 0
        return;

    if(dx * (pt2.x - pt1.x) > 0)
    {
        if(dy * (pt2.y - pt1.y) > 0)
        {
            if(FMath::Abs(dx * y) > FMath::Abs(dy * x))
            {
                pt0.x = pt2.x;
                pt2.x = int(dx) + pt1.x;
                pt0.y = pt2.y;
                pt2.y = int(dy) + pt1.y;
                invertArray = true;
            }
        }
        if(pt0.x == pt2.x || weight == 1.0f)
        {
            dt = (pt0.x - pt1.x) / (double)x;
        } 
        else
        {
            dq = std::sqrt(4.0f * weight * weight * (pt0.x - pt1.x) * (pt2.x - pt1.x) + (pt2.x - pt0.x) * (long)(pt2.x - pt0.x));

            if(pt1.x < pt0.x)
                dq = -dq;

            dt = (2.0f * weight * (pt0.x - pt1.x) - pt0.x + pt2.x + dq) / (2.0f * (1.0f - weight) * (pt2.x - pt0.x));
        }
        dq = 1.0f / (2.0f * dt * (1.0f - dt) * (weight - 1.0f) + 1.0f);
        dx = (dt * dt * (pt0.x - 2.0f * weight * pt1.x + pt2.x) + 2.0f * dt * (weight * pt1.x - pt0.x) + pt0.x) * dq;
        dy = (dt * dt * (pt0.y - 2.0f * weight * pt1.y + pt2.y) + 2.0f * dt * (weight * pt1.y - pt0.y) + pt0.y) * dq;
        dWeight = dt * (weight - 1.0f) + 1.0f;
        dWeight *= (dWeight * dq);
        weight = float(((1.0f - dt) * (weight - 1.0f) + 1.0f) * std::sqrt(dq));
        x = int(std::floor(dx + 0.5f));
        y = int(std::floor(dy + 0.5f));
        dy = (dx - pt0.x) * (pt1.y - pt0.y) / (pt1.x - pt0.x) + pt0.y;
        InternalGenerateQuadraticBezierSegPoints(pt0.x,pt0.y,x,int(std::floor(dy + 0.5f)),x,y,float(dWeight),ioQuadraticBezierPoints, invertArray);
        dy = (dx - pt2.x) * (pt1.y - pt2.y) / (pt1.x - pt2.x) + pt2.y;
        pt1.y = int(std::floor(dy + 0.5f));
        pt0.x = pt1.x = x;
        pt0.y = y;

    }

    if((pt0.y - pt1.y) * (long)(pt2.y - pt1.y) > 0)
    {
        if(pt0.y == pt2.y || iWeight == 1.0f)
        {
            dt = (pt0.y - pt1.y) / (pt0.y - 2.0f * pt1.y + pt2.y);
        } 
        else
        {
            dq = std::sqrt(4.0f * weight * weight * (pt0.y - pt1.y) * (pt2.y - pt1.y) + (pt2.y - pt0.y) * (long)(pt2.y - pt0.y));

            if(pt1.y < pt0.y)
                dq = -dq;

            dt = (2.0f * weight * (pt0.y - pt1.y) - pt0.y + pt2.y + dq) / (2.0f * (1.0f - weight) * (pt2.y - pt0.y));
        }
        dq = 1.0f / (2.0f * dt * (1.0f - dt) * (weight - 1.0f) + 1.0f);
        dx = (dt * dt * (pt0.x - 2.0f * weight * pt1.x + pt2.x) + 2.0f * dt * (weight * pt1.x - pt0.x) + pt0.x) * dq;
        dy = (dt * dt * (pt0.y - 2.0f * weight * pt1.y + pt2.y) + 2.0f * dt * (weight * pt1.y - pt0.y) + pt0.y) * dq;
        dWeight = dt * (weight - 1.0f) + 1.0f;
        dWeight *= (dWeight * dq);
        weight = float(((1.0f - dt) * (weight - 1.0f) + 1.0f) * std::sqrt(dq));
        x = int(std::floor(dx + 0.5f));
        y = int(std::floor(dy + 0.5f));
        dx = (pt1.x - pt0.x) * (dy - pt0.y) / (pt1.y - pt0.y) + pt0.x;
        InternalGenerateQuadraticBezierSegPoints(pt0.x,pt0.y,int(std::floor(dx + 0.5f)),y,x,y,float(dWeight),ioQuadraticBezierPoints, invertArray);

        dx = (pt1.x - pt2.x) * (dy - pt2.y) / (pt1.y - pt2.y) + pt2.x;
        pt1.x = int(std::floor(dx + 0.5f));
        pt0.x = x;
        pt0.y = pt1.y = y;

    }
    InternalGenerateQuadraticBezierSegPoints( pt0.x, pt0.y, pt1.x, pt1.y, pt2.x, pt2.y, weight * weight, ioQuadraticBezierPoints, invertArray);
}

ULIS_NAMESPACE_END

