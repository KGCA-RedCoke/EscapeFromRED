#include "NavTest.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"

void NavTest::Initialize()
{
    LoadMapFile("rsc\\GameResource\\GridMap\\GridMap.png");
    MapFile;
    NodeRadius = 50.0f;
    NodeCenter = FVector(NodeRadius, 0, - NodeRadius);
    GridDivs = FVector2(200, 200);
    GridCenter = FVector(0, 100, 0); // x, y, z
    
    NodeDiameter = NodeRadius * 2.0f;
    GridWorldSize = FVector2(GridDivs.x * NodeDiameter, GridDivs.y * NodeDiameter);
    NodeScale = FVector2(GridDivs.x * NodeRadius, GridDivs.y * NodeRadius); // col, row
    GridTopLeft = FVector(GridCenter.x - GridWorldSize.x/2,
                            GridCenter.y,
                            GridCenter.z + GridWorldSize.y/2);
    ObstacleScale = 50 / NodeRadius;




    
    for (int row = 0; row < GridDivs.y; row++)
    {
        for (int col = 0; col < GridDivs.x; col++)
        {
            if (MapFile->mRGBAData[row * GridDivs.x + col] > 200)
                // mGridGraph[row][col]->Walkable = false;
                Obstacles.push_back(FVector2(col, row));

        }
    }
    SetGraph();
}

void NavTest::Update(float DeltaTime)
{
    for(auto& actor : GetWorld.LevelManager->GetActiveLevel()->mActors)
    {
        /*if (firstRun && actor.get()->GetName().starts_with("S"))
        {
            FVector worldPos = actor.get()->GetWorldLocation();
            FVector2 targetGrid = FVector2(GridFromWorldPoint(worldPos).x, GridFromWorldPoint(worldPos).y);
            for (int row = -ObstacleScale; row <= ObstacleScale; row++)
            {
                for (int col = -ObstacleScale; col <= ObstacleScale; col++)
                {
                    Obstacles.push_back(FVector2(targetGrid.x + col, targetGrid.y + row));
                }
            }
        }
        else */if (actor.get()->GetName() == "Test Player")
        {
            PlayerPos = actor.get()->GetWorldLocation();
        }
        else if (firstRun && actor.get()->GetName().starts_with("SM_BigZombie"))
        {
            static int32_t enemyNum = 0;
            JText enemyCompName = std::format("Enemy_{}", enemyNum++);
            actor.get()->CreateDefaultSubObject<AStar>(enemyCompName);
        }
    }
    if (firstRun)
    {
        firstRun = false;
        SetObstacle();
    }
}

void NavTest::Render()
{
    auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
    G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
    {
        G_DebugBatch.DrawGrid_Implement(
            {NodeScale.x, 0, 0},
            {0, 0, NodeScale.y},
            GridCenter,
            GridDivs.x,
            GridDivs.y,
            Colors::Gray
        );
        DrawUnWalkable();
        DrawNode(GridFromWorldPoint(PlayerPos), Colors::Cyan);
        // for (auto grid : tempPath)
        // {
        //     DrawNode(FVector2(grid->GridX, grid->GridY), Colors::Cyan);
        // }
        
        /*DrawNode(GridFromWorldPoint(WorldPosFromGridPos(0, 1)), Colors::Pink);
        DrawNode(GridFromWorldPoint(WorldPosFromGridPos(0, 2)), Colors::Pink);
        DrawNode(GridFromWorldPoint(WorldPosFromGridPos(1, 5)), Colors::Pink);
        
        DrawNode(mGridGraph[0][0]->GridPos, Colors::Pink);
        DrawNode(mGridGraph[0][1]->GridPos, Colors::Pink);
        DrawNode(mGridGraph[0][2]->GridPos, Colors::Pink);*/
    }
    G_DebugBatch.PostRender();
}

void NavTest::Release()
{
}

void NavTest::DrawNode(FVector2 grid, FXMVECTOR InColor)
{
    G_DebugBatch.DrawQuad_Implement(
        // tl - tr - br - bl
        {GridTopLeft.x + grid.x * NodeDiameter, 0.1, GridTopLeft.z - grid.y * NodeDiameter},
        {GridTopLeft.x + (grid.x + 1) * NodeDiameter, 0.1, GridTopLeft.z - grid.y * NodeDiameter},
        {GridTopLeft.x + (grid.x + 1) * NodeDiameter, 0.1, GridTopLeft.z - (grid.y + 1) * NodeDiameter},
        {GridTopLeft.x + grid.x * NodeDiameter, 0.1, GridTopLeft.z - (grid.y + 1) * NodeDiameter},
        InColor
    );
}

FVector NavTest::WorldPosFromGridPos(int col, int row)
{
    float x = GridTopLeft.x + col * NodeDiameter;
    float z = (GridTopLeft.z - row * NodeDiameter);
    return FVector(x, GridTopLeft.y, z);
}

FVector NavTest::WorldPosFromGridPos(FVector2 GridPos)
{
    int col = GridPos.x;
    int row = GridPos.y;
    return WorldPosFromGridPos(col, row);
}

FVector2 NavTest::GridFromWorldPoint(FVector worldPos)
{
    int gridX = floor((worldPos.x - GridTopLeft.x) / NodeDiameter);
    int gridY = -ceil((worldPos.z - GridTopLeft.z) / NodeDiameter);
    return FVector2(gridX, gridY);
}

void NavTest::SetGraph()
{
    mGridGraph.resize(GridDivs.y);
    for (int row = 0; row < GridDivs.y; row++)
    {
        mGridGraph[row].resize(GridDivs.x);
        for (int col = 0; col < GridDivs.x; col++)
        {
            mGridGraph[row][col] = MakePtr<Node>(true, WorldPosFromGridPos(col, row), col, row);
        }
    }
}

void NavTest::SetObstacle()
{
    for (auto node : Obstacles) // node : col, row
    {
        if (node.x >= 0 && node.x < GridDivs.x && node.y >= 0 && node.y < GridDivs.y)
           mGridGraph[node.y][node.x]->Walkable = false;
    }
    for (int row = 0; row < GridDivs.y; row++)
    {
        for (int col = 0; col < GridDivs.x; col++)
        {
            SetChildNode(row, col);
        }
    }
}

void NavTest::SetChildNode(int row, int col)
{
    // if (mGridGraph[row][col]->Walkable == false)
    //     return;
    int ChildIdx[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            {0, -1},  {0, 1},
            {1, -1}, {1, 0}, {1, 1}
        };
    for (auto node : ChildIdx)
    {
        if (row + node[0] >= 0 && row + node[0] < GridDivs.y && col + node[1] >= 0 && col + node[1] < GridDivs.x )
        {
            if (mGridGraph[row + node[0]][col + node[1]]->Walkable)
                mGridGraph[row][col]->Children.push_back(mGridGraph[row + node[0]][col + node[1]]);
        }
    }
}

void NavTest::DrawUnWalkable()
{
    for (auto location : Obstacles)
    {
        DrawNode(location, Colors::Red);
    }
}

void NavTest::LoadMapFile(const JText& FileName)
{
    // MapFile = GetWorld.TextureManager->Load(FileName);
    MapFile = new JTexture(FileName, true);
    // MapFile = MakePtr<JTexture>(FileName, true).get();
}

