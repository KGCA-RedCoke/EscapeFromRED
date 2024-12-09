#pragma once
#include "Core/Utils/Math/Vector.h"

namespace NAV
{
    class Node
    {
    public:
        Node(bool _walkable, FVector _worldPos, int _gridX, int _gridY) :
            Walkable(_walkable), WorldPos(_worldPos), GridX(_gridX), GridY(_gridY)
        {
            GridPos.x = GridX;
            GridPos.y = GridY;
        };
    public:
        bool Walkable;
        FVector WorldPos;
        FVector2 GridPos;
        int GridX;
        int GridY;
        int GCost = 0;
        int HCost = 0;

        WPtr<Node> Parent;
        std::vector<WPtr<Node>> Children;

    public:
        int FCost(){ return GCost + HCost; }
    };
}
