﻿#pragma once

#include "Core/Utils/Math/Vector2.h"
#include "TempLine.h"

namespace Nav
{
    class Node;
}

class Path
{
public:
    std::vector<Ptr<Nav::Node>> lookPoints;
    std::vector<Ptr<Line>> turnBoundaries;
    int finishLineIndex;
    
    Path(std::vector<Ptr<Nav::Node>> wayPoints, FVector startPos, float turnDst);
    static FVector2 V3ToV2(FVector v3);
};
