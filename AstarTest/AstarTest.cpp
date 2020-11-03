﻿#include <iostream>
#include <vector>
#include "Node.h"
#include "AstarNode.h"
#include "BinaryHeap.h"
#include <cmath>
#include <float.h>
#include "BinaryHeap.cpp"
using namespace std;

static vector<AstarNode> initVec;
static BinaryHeap<AstarNode> openNodesList(initVec);
static vector<AstarNode> closeNodesList;
vector<vector<Node>> nodeMap;
static shared_ptr<Node> starNode;
static shared_ptr<Node> endNode;
shared_ptr<AstarNode> curCheckNode;

void CalculatePath(int map[][5], int width, int height, int resultX[], int resultY[]);
float DisOfNode(shared_ptr<Node> node1, shared_ptr<Node> node2);
void CheckNeighborNode(shared_ptr<AstarNode> centerNode, int width, int height);
shared_ptr<AstarNode> UpdateOpenCloseNodeList();
bool IsContains(AstarNode node, vector<AstarNode>& aStarNodeList);

void CalculatePath(int map[][5], int width, int height, int resultX[], int resultY[]) {
	closeNodesList.clear();

	int count = 0;

	for (int i = 0; i < width; i++)
	{
		vector<Node> nodeRow;
		nodeMap.push_back(nodeRow);
		for (int j = 0; j < height; j++) {
			int type = map[i][j];
			Node curNode = *new Node(type, i, j);
			nodeMap[i].push_back(curNode);
			if (type == 1) {
				starNode = make_shared<Node>(type, i, j);
			}
			else if (type == 2) {
				endNode = make_shared<Node>(type, i, j);
			}
		}
	}

	AstarNode startAstarNode((float)0, DisOfNode(starNode, endNode), nullptr, starNode->x, starNode->y);
	AstarNode endAstarNode(DisOfNode(starNode, endNode), 0, nullptr, endNode->x, endNode->y);

	closeNodesList.push_back(startAstarNode);
	curCheckNode = make_shared<AstarNode>(startAstarNode);

	while (curCheckNode != nullptr && (curCheckNode->x != endAstarNode.x || curCheckNode->y != endAstarNode.y))
	{
		CheckNeighborNode(curCheckNode, width, height);
		curCheckNode = UpdateOpenCloseNodeList();
	}
	cout << "closeNodesList" << endl;
	for (auto itBegin = closeNodesList.begin(); itBegin != closeNodesList.end(); itBegin++) {
		cout << (*itBegin).x << "," << (*itBegin).y << endl;
	}

	//结果
	int resultCount = 0;
	if (closeNodesList.back().x == endNode->x && closeNodesList.back().y == endNode->y) {
		shared_ptr<AstarNode> curNode = closeNodesList.back().parentNode;
		while (curNode->x != starNode->x || curNode->y != starNode->y)
		{
			resultX[resultCount] = curNode->x;
			resultY[resultCount] = curNode->y;
			resultCount++;
			curNode = curNode->parentNode;
		}
	}
}

/// <summary>
/// 检查周围邻居节点
/// </summary>
/// <param name="centerNode"></param>
void CheckNeighborNode(shared_ptr<AstarNode> centerNode, int width, int height) {
	int centerX = (*centerNode).x;
	int centerY = (*centerNode).y;
	bool isTopObstacle = false;
	bool isBottomObstacle = false;
	bool isLeftObstacle = false;
	bool isRightObstacle = false;

	//下
	if (centerY + 1 < height) {
		auto it = nodeMap[centerX].begin() + centerY + 1;
		shared_ptr<Node> bottom = make_shared<Node>(*it);
		if (bottom->type != 3) {
			float disFromStart = centerNode->g + 1;
			float disFromEnd = DisOfNode(bottom, endNode);
			AstarNode aStarNodeBottom(disFromStart, disFromEnd, centerNode, centerX, centerY + 1);

			if (!IsContains(aStarNodeBottom, closeNodesList) && !openNodesList.IsContains(aStarNodeBottom)) {
				openNodesList.Insert(aStarNodeBottom);
			}
			
		}
		else
		{
			isTopObstacle = true;
		}
	}

	//上
	if (centerY - 1 >= 0) {
		auto it = nodeMap[centerX].begin() + centerY - 1;
		shared_ptr<Node> top = make_shared<Node>(*it);;
		if (top->type != 3) {
			float disFromStart = centerNode->g + 1;
			float disFromEnd = DisOfNode(top, endNode);
			AstarNode aStarNodeTop(disFromStart, disFromEnd, centerNode, centerX, centerY - 1);

			if (!IsContains(aStarNodeTop, closeNodesList) && !openNodesList.IsContains(aStarNodeTop)) {
				openNodesList.Insert(aStarNodeTop);
			}
		}
		else
		{
			isBottomObstacle = true;
		}
	}

	//左
	if (centerX - 1 >= 0) {
		auto it = nodeMap[centerX-1].begin() + centerY;
		shared_ptr<Node> left = make_shared<Node>(*it);
		if (left->type != 3) {
			float disFromStart = centerNode->g + 1;
			float disFromEnd = DisOfNode(left, endNode);
			AstarNode aStarNodeLeft(disFromStart, disFromEnd, centerNode, centerX - 1, centerY);

			if (!IsContains(aStarNodeLeft, closeNodesList) && !openNodesList.IsContains(aStarNodeLeft)) {
				openNodesList.Insert(aStarNodeLeft);
			}
		}
		else
		{
			isLeftObstacle = true;
		}
	}

	//右
	if (centerX + 1 < width) {
		auto it = nodeMap[centerX + 1].begin() + centerY;
		shared_ptr<Node> right = make_shared<Node>(*it);
		if (right->type != 3) {
			float disFromStart = centerNode->g + 1;
			float disFromEnd = DisOfNode(right, endNode);
			AstarNode aStarNodeRight(disFromStart, disFromEnd, centerNode, centerX + 1, centerY);

			if (!IsContains(aStarNodeRight, closeNodesList) && !openNodesList.IsContains(aStarNodeRight)) {
				openNodesList.Insert(aStarNodeRight);
			}
		}
		else
		{
			isRightObstacle = true;
		}
	}

	//左下
	if (centerX - 1 >= 0 && centerY + 1 < height && !(isTopObstacle && isLeftObstacle)) {
		auto it = nodeMap[centerX - 1].begin() + centerY + 1;
		shared_ptr<Node> leftBottom = make_shared<Node>(*it);
		if (leftBottom->type != 3) {
			float disFromStart = centerNode->g + 1.4f;
			float disFromEnd = DisOfNode(leftBottom, endNode);
			AstarNode aStarNodeLeftBottom(disFromStart, disFromEnd, centerNode, centerX - 1, centerY + 1);

			if (!IsContains(aStarNodeLeftBottom, closeNodesList) && !openNodesList.IsContains(aStarNodeLeftBottom)) {
				openNodesList.Insert(aStarNodeLeftBottom);
			}
		}
	}

	//左上
	if (centerX - 1 >= 0 && centerY - 1 >= 0 && !(isBottomObstacle && isLeftObstacle)) {
		auto it = nodeMap[centerX - 1].begin() + centerY - 1;
		shared_ptr<Node> leftTop = make_shared<Node>(*it);
		if (leftTop->type != 3) {
			float disFromStart = centerNode->g + 1.4f;
			float disFromEnd = DisOfNode(leftTop, endNode);
			AstarNode aStarNodeLeftTop(disFromStart, disFromEnd, centerNode, centerX - 1, centerY - 1);

			if (!IsContains(aStarNodeLeftTop, closeNodesList) && !openNodesList.IsContains(aStarNodeLeftTop)) {
				openNodesList.Insert(aStarNodeLeftTop);
			}
		}
	}

	//右下
	if (centerX + 1 < width && centerY + 1 < height && !(isTopObstacle && isRightObstacle)) {
		auto it = nodeMap[centerX + 1].begin() + centerY + 1;
		shared_ptr<Node> rightBottom = make_shared<Node>(*it);
		if (rightBottom->type != 3) {
			float disFromStart = centerNode->g + 1.4f;
			float disFromEnd = DisOfNode(rightBottom, endNode);
			AstarNode aStarNodeRightBottom(disFromStart, disFromEnd, centerNode, centerX + 1, centerY + 1);

			if (!IsContains(aStarNodeRightBottom, closeNodesList) && !openNodesList.IsContains(aStarNodeRightBottom)) {
				openNodesList.Insert(aStarNodeRightBottom);
			}
		}
	}

	//右上
	if (centerX + 1 < width && centerY - 1 >= 0 && !(isBottomObstacle && isRightObstacle)) {
		auto it = nodeMap[centerX + 1].begin() + centerY - 1;
		shared_ptr<Node> rightTop = make_shared<Node>(*it);
		if (rightTop->type != 3) {
			float disFromStart = centerNode->g + 1.4f;
			float disFromEnd = DisOfNode(rightTop, endNode);
			AstarNode aStarNodeLeftTop(disFromStart, disFromEnd, centerNode, centerX + 1, centerY - 1);

			if (!IsContains(aStarNodeLeftTop, closeNodesList) && !openNodesList.IsContains(aStarNodeLeftTop)) {
				openNodesList.Insert(aStarNodeLeftTop);
			}
		}
	}
}

/// <summary>
/// 更新关闭列表和开启列表
/// </summary>
/// <param name="curCheckNode"></param>
shared_ptr<AstarNode> UpdateOpenCloseNodeList() {
	AstarNode& min = openNodesList.DeleteMin();
	shared_ptr<AstarNode> mincostNode = make_shared<AstarNode>(min);
	closeNodesList.push_back(min);
	return mincostNode;
}

/// <summary>
/// 两个节点的距离
/// </summary>
/// <param name="node1"></param>
/// <param name="node2"></param>
/// <returns></returns>
float DisOfNode(shared_ptr<Node> node1, shared_ptr<Node> node2) {
	float result = abs(node1->x - node2->x) + abs(node1->y - node2->y);
	return result;
}

/// <summary>
/// 是否包含
/// </summary>
/// <param name="node"></param>
/// <param name="aStarNodeList"></param>
/// <returns></returns>
bool IsContains(AstarNode node, vector<AstarNode>& aStarNodeList) {
	for (size_t i = 0; i < aStarNodeList.size(); i++) {
		if (node.x == aStarNodeList[i].x && node.y == aStarNodeList[i].y) {
			return true;
		}
	}
	return false;
}

int main()
{
	int map[5][5] = { {0,0,0,0,0},
					  {0,1,0,0,0},
					  {0,3,3,3,0},
					  {0,0,0,3,0},
					  {0,2,0,0,0} };
	int resultX[25] = {0};
	int resultY[25] = { 0};
	CalculatePath(map,5,5, resultX, resultY);
	for (int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			cout << nodeMap[i][j].type<<" ";
		}
		cout << endl;
	}
	cout << "结果" << endl;
	for (int i = 0; i < 25; i++) {
		if (resultX[i] == 0 && resultY[i] == 0) {
			continue;
		}
		cout << "(" << resultX[i] << "," << resultY[i] << ")"<<endl;
	}
}