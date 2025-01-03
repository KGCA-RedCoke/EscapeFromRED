#pragma once
#include "common_include.h"
#include "Core/Manager/Manager_Base.h"
#include "Core/Utils/Math/MathFwd.h"
#include "Node.h"
#include "AStar.h"
#include "Core/Graphics/Texture/JTexture.h"

using namespace DirectX;
using namespace Nav;

#define G_NAV_MAP NavTest::Get()


class TestGrid;

class NavTest : public TSingleton<NavTest>
{
public:
    void Initialize();
    void Render();

public:
    void DrawNode(FVector2 grid, FXMVECTOR InColor);
    FVector WorldPosFromGridPos(int row, int col);
    FVector WorldPosFromGridPos(FVector2 worldGrid);
    FVector2 GridFromWorldPoint(FVector worldPos);
    void SetGraph(std::vector<std::vector<Ptr<Node>>>& graph, EFloorType FloorType);
    void SetObstacle(std::vector<std::vector<Ptr<Node>>>& graph, JTexture* MapFile,
        std::vector<FVector2>& Obstacles);
    void SetChildNode(std::vector<std::vector<Ptr<Node>>>& graph, int i, int j);
    void DrawUnWalkable(std::vector<FVector2>& Obstacles);
    
    // FVector PlayerPos = FVector::ZeroVector;
    float PlayerHeight = 0.0f;
    FVector NewPPos;
    FVector NpcPos;
    // FVector NewNPos;
    // std::vector<FVector> Npcs;
    // std::vector<FVector> NewNpcs;
    std::vector<FVector2> FirstFloorObstacle;
    std::vector<FVector2> SecondFloorObstacle;
    
    FVector2 GridWorldSize;
    float NodeRadius;
    FVector NodeCenter;
    float NodeDiameter;
    FVector2 GridDivs;
    FVector2 NodeScale;
    FVector GridCenter = FVector::ZeroVector;
    FVector GridTopLeft = FVector::ZeroVector;
    bool firstRun = true;
    int ObstacleScale  = 1;
    JTexture* FirstFloorMap;
    JTexture* SecondFloorMap;
    Ptr<Nav::Node> Stair1_2;
    Ptr<Nav::Node> Stair2_1;

    long long currentFrame = 0;
    
    std::unordered_set<JBoxComponent*> ColliderTarget;
    std::unordered_set<JBoxComponent*> GroundColliders;
    
    std::vector<std::vector<Ptr<Node>>> mGridGraph;
    std::vector<std::vector<Ptr<Node>>> m2ndFloor;
    
    std::vector<Ptr<Nav::Node>> tempPath;
    
    
    // std::function<Node*(int, int)> GetNodeInSubGrid;
    //
    // void CreateDynamicMapper(const std::vector<std::vector<Ptr<Node>>>& mGridGraph,
    //                          int playerRow, int playerCol);
    
};
