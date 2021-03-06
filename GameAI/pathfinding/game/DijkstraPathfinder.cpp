#include "DijkstraPathfinder.h"
#include "Path.h"
#include "Connection.h"
#include "GridGraph.h"
#include "Game.h"
#include <PerformanceTracker.h>
#include "PriorityQueue.h"
#include "Compare.h"
#include "GameApp.h"

DijkstraPathfinder::DijkstraPathfinder(Graph * pGraph) :
	GridPathfinder(dynamic_cast<GridGraph*>(pGraph))
{
#ifdef VISUALIZE_PATH
	mpPath = NULL;
#endif

}

DijkstraPathfinder::~DijkstraPathfinder()
{
#ifdef VISUALIZE_PATH
	delete mpPath;
#endif
}

Path * DijkstraPathfinder::findPath(Node * pFrom, Node * pTo)
{
	//== This chunk of code handles if the user clicks on the black + ==//
	if (!isEndNodeValid(pTo)) {
		return nullptr;
	}
	//== If user clicks on the same node ==//
	if (pFrom == pTo) {
		return nullptr;
	}

	if(static_cast<GameApp*>(gpGame)->FindPath(pFrom, pTo)){
		return static_cast<GameApp*>(gpGame)->FindPath(pFrom, pTo);
	}

	gpPerformanceTracker->clearTracker("path");
	gpPerformanceTracker->startTracking("path");
	//set up Open queue and add starting node.
	PriorityQueue<NodeStruct*, std::vector<NodeStruct*>, CompareDijkstraStruct> nodesToVisit;
	NodeStruct* pCurrentNodeStruct = new NodeStruct(pFrom);
	nodesToVisit.push(pCurrentNodeStruct);

#ifdef VISUALIZE_PATH
	mVisitedNodes.clear(); //This tracks # of nodes processed
	mVisitedNodes.push_back(pFrom);
#endif

	std::vector<NodeStruct*> closedList;
	bool notInClosedList = true; //used in search later.
	bool TempToNodeIsInOpenList = false;
	//set when end node added.
	NodeStruct* toNodeStruct = nullptr;

	//start iterating through nodes
	while (pCurrentNodeStruct->mpThisNode != pTo && nodesToVisit.size() > 0) {
		pCurrentNodeStruct = nodesToVisit.top(); //access the top element
		nodesToVisit.pop(); //remove node, doesn't return it

		closedList.push_back(pCurrentNodeStruct);

		//get connections from current Node
		std::vector<Connection*> connections = mpGraph->getConnections(pCurrentNodeStruct->mpThisNode->getId());
		NodeStruct* pTempToNodeStruct;

		for (unsigned int i = 0; i < connections.size(); i++) { 
			//for each neighbor of current node
			Connection* pConnection = connections[i];

			//set up node.
			auto structIter = nodesToVisit.findStruct(pConnection->getToNode());
			if (structIter == nodesToVisit.end()) { //if not in open list then init
				TempToNodeIsInOpenList = false;
				pTempToNodeStruct = new NodeStruct(pConnection->getToNode());
			}
			else {
				TempToNodeIsInOpenList = true;
				pTempToNodeStruct = *structIter;
			}

			auto cost = pConnection->getCost() + pCurrentNodeStruct->mCost;
			notInClosedList = true; //set later if not is not in open list.

			//if node is in open list update it
			//Need to update pTempToNodeStruct so that it points at the same node.
			if (TempToNodeIsInOpenList) {
				if (pTempToNodeStruct->mCost > cost) { //if shorter path has been found.
					pTempToNodeStruct->mCost = cost; //set cost
					pTempToNodeStruct->mpPrevNodeStruct = pCurrentNodeStruct; //set previous node
				}
			}
			//if node is not in closed or open list, set values
			else {
				//Check for node in closed list.
				std::vector<NodeStruct*>::iterator iter = closedList.begin();
				while (iter != closedList.end()) {
					if ((*iter)->mpThisNode == pTempToNodeStruct->mpThisNode) {
						iter == closedList.end() - 1;
						notInClosedList = false;
					}
					iter++;
				}
				//Set values
				if (notInClosedList) {
					pTempToNodeStruct->mCost = cost;
					pTempToNodeStruct->mpPrevNodeStruct = pCurrentNodeStruct;
				}
			}

			//If end not found, not in closed list, and not in open list.
			//Add to open list and check if it's the end.
			if (toNodeStruct == nullptr &&
				notInClosedList && 
				!TempToNodeIsInOpenList
				) {
				nodesToVisit.push(pTempToNodeStruct);
				if (pTempToNodeStruct->mpThisNode->getId() == pTo->getId()) { //found node, not 100% shortest path.
					toNodeStruct = pTempToNodeStruct;
				}
#ifdef VISUALIZE_PATH
				mVisitedNodes.push_back(pTempToNodeStruct->mpThisNode);
#endif
			}
		}
	}

#ifdef VISUALIZE_PATH
	//== This handles if the user selects the same node ==//
	if (toNodeStruct == nullptr) {
		std::cout << "You picked a node too close to the start.";
		return nullptr;
	}

	delete mpPath;
	Path* pPath = new Path();
	while (toNodeStruct->mpThisNode != pFrom) {
		pPath->addNode(toNodeStruct->mpThisNode);
		toNodeStruct = toNodeStruct->mpPrevNodeStruct;
		if (toNodeStruct == nullptr) {
			throw "Broken Path in Dijkstra";
		}
	}
#endif

	static_cast<GameApp*>(gpGame)->CachePath(pFrom, pTo, pPath);

	gpPerformanceTracker->stopTracking("path");
	mTimeElapsed = gpPerformanceTracker->getElapsedTime("path");

#ifdef VISUALIZE_PATH
		mpPath = pPath;
#endif
	for (int i = 0; i < closedList.size(); i++) {
		delete closedList[i];
	}
	nodesToVisit.clear();
	return pPath;
}
