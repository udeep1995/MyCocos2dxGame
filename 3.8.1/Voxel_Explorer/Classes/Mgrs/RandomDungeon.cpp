//
//  RandomDungeon.cpp
//  Voxel_Explorer
//
//  Created by wang haibo on 15/10/21.
//
//

#include "RandomDungeon.hpp"
#include "UtilityHelper.h"
#include "GameScene.h"
USING_NS_CC;

DUNGEON_TYPE DT_SelectGroup1[] = { DT_SEWER, DT_PRISON, DT_TEMPLE, DT_PIT, DT_CAVE, DT_TOMB };
DUNGEON_TYPE DT_SelectGroup2[] = { DT_DWARF_CASTLE, DT_MAGA_TOWER, DT_ORC_FORTRESS, DT_ELF_FOREST,DT_TROLL_TEMPLE };
DUNGEON_TYPE DT_SelectGroup3[] = { DT_BEHOLDER_CASTLE, DT_WARP_SPACE, DT_DRAGON_LAIR, DT_LICH_TOMB };

DUNGEON_TYPE DT_SelectAll[] = { DT_SEWER, DT_PRISON, DT_TEMPLE, DT_PIT, DT_CAVE, DT_TOMB, DT_DWARF_CASTLE, DT_MAGA_TOWER, DT_ORC_FORTRESS, DT_ELF_FOREST,DT_TROLL_TEMPLE, DT_BEHOLDER_CASTLE, DT_WARP_SPACE, DT_DRAGON_LAIR, DT_LICH_TOMB };


std::vector<DUNGEON_TYPE> RandomDungeon::SELECT_GROUP1(DT_SelectGroup1, DT_SelectGroup1+ sizeof(DT_SelectGroup1)/sizeof(DUNGEON_TYPE));
std::vector<DUNGEON_TYPE> RandomDungeon::SELECT_GROUP2(DT_SelectGroup2, DT_SelectGroup2+ sizeof(DT_SelectGroup2)/sizeof(DUNGEON_TYPE));
std::vector<DUNGEON_TYPE> RandomDungeon::SELECT_GROUP3(DT_SelectGroup3, DT_SelectGroup3+ sizeof(DT_SelectGroup3)/sizeof(DUNGEON_TYPE));
std::vector<DUNGEON_TYPE> RandomDungeon::SELECT_ALL(DT_SelectAll, DT_SelectAll+ sizeof(DT_SelectAll)/sizeof(DUNGEON_TYPE));


RandomDungeon* g_pRandomDungeonInstance = nullptr;
RandomDungeon* RandomDungeon::getInstance()
{
    if( g_pRandomDungeonInstance == nullptr )
        g_pRandomDungeonInstance = new RandomDungeon();
    
    return g_pRandomDungeonInstance;
}
RandomDungeon::RandomDungeon()
{
    m_pCurrentNode = nullptr;
    m_nCurrentSelectGroup = 0;
    m_nDifficultClass = 1;
}
RandomDungeon::~RandomDungeon()
{
}

bool RandomDungeon::build()
{
    if(m_pCurrentNode == nullptr)
    {
        DUNGEON_TYPE type = generateDungeonNodeType();
        if(type == DT_UNKNOWN)
            return false;
        m_pCurrentNode = new (std::nothrow) DungeonNode();
        if(!m_pCurrentNode)
            return false;
        m_pCurrentNode->m_Type = type;
        m_pCurrentNode->m_nNodeDepth = 1;
        assignedDungeonNode(m_pCurrentNode);
        m_pCurrentNode->autorelease();
        generateNextDungeonNode();
    }
    return true;
}
void RandomDungeon::selectDungeonNode(bool left)
{
    if(left)
    {
        CCASSERT(m_pCurrentNode != nullptr && m_pCurrentNode->m_pLeftNode != nullptr, "m_pCurrentNode != nullptr && m_pCurrentNode->m_pLeftNode != nullptr");
        if(m_pCurrentNode->m_pRightNode != nullptr)
        {
            m_UnSelected.push_back(m_pCurrentNode->m_pRightNode->m_Type);
            m_pCurrentNode->m_pRightNode = nullptr;
        }
        m_pCurrentNode = m_pCurrentNode->m_pLeftNode;
        generateNextDungeonNode();
    }
    else
    {
        CCASSERT(m_pCurrentNode != nullptr && m_pCurrentNode->m_pRightNode != nullptr && m_pCurrentNode->m_pLeftNode != nullptr, "m_pCurrentNode != nullptr && m_pCurrentNode->m_pRightNode != nullptr && m_pCurrentNode->m_pLeftNode != nullptr");
        if(m_pCurrentNode->m_pLeftNode != nullptr)
        {
            m_UnSelected.push_back(m_pCurrentNode->m_pLeftNode->m_Type);
            m_pCurrentNode->m_pLeftNode = nullptr;
        }
        m_pCurrentNode = m_pCurrentNode->m_pRightNode;
        generateNextDungeonNode();
    }
    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(scene);
}
bool RandomDungeon::generateNextDungeonNode()
{
    if(!m_pCurrentNode)
        return false;
    DUNGEON_TYPE type = generateDungeonNodeType();
    if(type == DT_UNKNOWN)
    {
        type = generateDungeonNodeType();
        if(type == DT_UNKNOWN)
            return false;
    }
    if(m_pCurrentNode->m_pLeftNode == nullptr)
    {
        m_pCurrentNode->m_pLeftNode = new (std::nothrow) DungeonNode();
        if(!m_pCurrentNode->m_pLeftNode)
            return false;
        m_pCurrentNode->m_pLeftNode->m_Type = type;
        m_pCurrentNode->m_pLeftNode->m_nNodeDepth = m_pCurrentNode->m_nNodeDepth + 1;
        assignedDungeonNode(m_pCurrentNode->m_pLeftNode);
        m_pCurrentNode->m_pLeftNode->autorelease();
    }
    type = generateDungeonNodeType();
    if( (type != DT_UNKNOWN) && (m_pCurrentNode->m_pRightNode == nullptr) )
    {
        m_pCurrentNode->m_pRightNode = new (std::nothrow) DungeonNode();
        if(!m_pCurrentNode->m_pRightNode)
            return false;
        m_pCurrentNode->m_pRightNode->m_Type = type;
        m_pCurrentNode->m_pRightNode->m_nNodeDepth = m_pCurrentNode->m_nNodeDepth + 1;
        assignedDungeonNode(m_pCurrentNode->m_pRightNode);
        m_pCurrentNode->m_pRightNode->autorelease();
    }
    return true;
}
DUNGEON_TYPE RandomDungeon::generateDungeonNodeType()
{
    DUNGEON_TYPE ret = DT_UNKNOWN;
    if (m_nCurrentSelectGroup == 0 && m_UnSelected.empty()) {
        m_UnSelected = SELECT_GROUP1;
        m_nCurrentSelectGroup = 1;
    }
    
    if(m_nCurrentSelectGroup == 1)
    {
        if(m_UnSelected.empty())
        {
            m_UnSelected = SELECT_GROUP1;
            m_nCurrentSelectGroup = 1;
            m_nDifficultClass = m_nDifficultClass +1;
            return generateDungeonNodeType();
        }
        else
        {
            int rand = cocos2d::random(0, (int)m_UnSelected.size()-1);
            ret = m_UnSelected[rand];
            m_UnSelected.erase(m_UnSelected.begin() + rand);
        }
    }
    ///此版本只做6关，后续先注释掉
//    if(m_nCurrentSelectGroup == 1)
//    {
//        if(m_UnSelected.empty())
//        {
//            m_UnSelected = SELECT_GROUP2;
//            m_nCurrentSelectGroup = 2;
//            return ret;
//        }
//        else
//        {
//            int rand = cocos2d::random(0, (int)m_UnSelected.size()-1);
//            ret = m_UnSelected[rand];
//            m_UnSelected.erase(m_UnSelected.begin() + rand);
//        }
//    }
//    if(m_nCurrentSelectGroup == 2)
//    {
//        if(m_UnSelected.empty())
//        {
//            m_UnSelected = SELECT_GROUP3;
//            m_nCurrentSelectGroup = 3;
//            return ret;
//        }
//        else
//        {
//            int rand = cocos2d::random(0, (int)m_UnSelected.size()-1);
//            ret = m_UnSelected[rand];
//            m_UnSelected.erase(m_UnSelected.begin() + rand);
//        }
//    }
//    if(m_nCurrentSelectGroup == 3)
//    {
//        if(m_UnSelected.empty())
//        {
//            m_UnSelected = SELECT_ALL;
//            m_nCurrentSelectGroup = 4;
//            m_nDifficultClass = m_nDifficultClass +1;
//            return ret;
//        }
//        else
//        {
//            int rand = cocos2d::random(0, (int)m_UnSelected.size()-1);
//            ret = m_UnSelected[rand];
//            m_UnSelected.erase(m_UnSelected.begin() + rand);
//        }
//    }
//    if(m_nCurrentSelectGroup == 4)
//    {
//        if(m_UnSelected.empty())
//        {
//            m_UnSelected = SELECT_ALL;
//            m_nCurrentSelectGroup = 4;
//            m_nDifficultClass = m_nDifficultClass +1;
//            return generateDungeonNodeType();
//        }
//        else
//        {
//            int rand = cocos2d::random(0, (int)m_UnSelected.size()-1);
//            ret = m_UnSelected[rand];
//            m_UnSelected.erase(m_UnSelected.begin() + rand);
//        }
//    }
    return ret;
}
void RandomDungeon::assignedDungeonNode(DungeonNode* node)
{
    if(!node)
        return;
    switch (node->m_Type) {
        case DT_SEWER:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_SEWER");
                node->m_nTotalNum = cocos2d::random(4, 6);
            }
            break;
        case DT_PRISON:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_PRISON");
                node->m_nTotalNum = cocos2d::random(4, 6);
            }
            break;
        case DT_TEMPLE:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_TEMPLE");
                node->m_nTotalNum = cocos2d::random(5, 8);
            }
            break;
        case DT_PIT:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_PIT");
                node->m_nTotalNum = cocos2d::random(5, 8);
            }
            break;
        case DT_CAVE:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_CAVE");
                node->m_nTotalNum = cocos2d::random(6, 10);
            }
            break;
        case DT_TOMB:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_TOMB");
                node->m_nTotalNum = cocos2d::random(6, 10);
            }
            break;
        case DT_DWARF_CASTLE:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_DWARF_CASTLE");
                node->m_nTotalNum = cocos2d::random(6, 10);
            }
            break;
        case DT_MAGA_TOWER:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_MAGA_TOWER");
                node->m_nTotalNum = cocos2d::random(6, 10);
            }
            break;
        case DT_ORC_FORTRESS:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_ORC_FORTRESS");
                node->m_nTotalNum = cocos2d::random(8, 12);
            }
            break;
        case DT_ELF_FOREST:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_ELF_FOREST");
                node->m_nTotalNum = cocos2d::random(8, 12);
            }
            break;
        case DT_TROLL_TEMPLE:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_TROLL_TEMPLE");
                node->m_nTotalNum = cocos2d::random(8, 12);
            }
            break;
        case DT_BEHOLDER_CASTLE:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_BEHOLDER_CASTLE");
                node->m_nTotalNum = cocos2d::random(10, 15);
            }
            break;
        case DT_WARP_SPACE:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_WARP_SPACE");
                node->m_nTotalNum = cocos2d::random(10, 15);
            }
            break;
        case DT_DRAGON_LAIR:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_DRAGON_LAIR");
                node->m_nTotalNum = cocos2d::random(10, 15);
            }
            break;
        case DT_LICH_TOMB:
            {
                node->m_strDungeonName = UtilityHelper::getLocalString("DT_LICH_TOMB");
                node->m_nTotalNum = cocos2d::random(10, 15);
            }
            break;
        default:
            break;
    }
}
void RandomDungeon::load()
{
}
void RandomDungeon::save()
{
}