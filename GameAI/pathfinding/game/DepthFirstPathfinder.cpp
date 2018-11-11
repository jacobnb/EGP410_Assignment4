#include "DepthFirstPathfinder.h"
#include "Path.h"
#include "Connection.h"
#include "GridGraph.h"
#include "Game.h"
#include <PerformanceTracker.h>
#include <list>
#include <vector>
#include <algorithm>
#include "GameApp.h"

using namespace std;

DepthFirstPathfinder::DepthFirstPathfinder(Graph* pGraph)
	:GridPathfinder(dynamic_cast<GridGraph*>(pGraph))
{
#ifdef VISUALIZE_PATH
	mpPath = NULL;
#endif

}

DepthFirstPathfinder::~DepthFirstPathfinder()
{
#ifdef VISUALIZE_PATH
	delete mpPath;
#endif
}

Path* DepthFirstPathfinder::findPath(Node* pFrom, Node* pTo)
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
	//allocate nodes to visit list and place starting node in it
	list<Node*> nodesToVisit;
	nodesToVisit.push_front(pFrom);

#ifdef VISUALIZE_PATH
	delete mpPath;
	mVisitedNodes.clear();
	mVisitedNodes.push_back(pFrom);
#endif

	//create Path
	Path* pPath = new Path();

	Node* pCurrentNode = NULL;
	bool toNodeAdded = false;

	while (pCurrentNode != pTo && nodesToVisit.size() > 0)
	{
		//get current node from front of list
		pCurrentNode = nodesToVisit.front();
		//remove node from list
		nodesToVisit.pop_front();
		//add Node to Path
		pPath->addNode(pCurrentNode);

		//get the Connections for the current node
		vector<Connection*> connections = mpGraph->getConnections(pCurrentNode->getId());

		//add all toNodes in the connections to the "toVisit" list, if they are not already in the list
		for (unsigned int i = 0; i < connections.size(); i++)
		{
			Connection* pConnection = connections[i];
			Node* pTempToNode = connections[i]->getToNode();
			if (!toNodeAdded &&
				!pPath->containsNode(pTempToNode) &&
				find(nodesToVisit.begin(), nodesToVisit.end(), pTempToNode) == nodesToVisit.end())
			{
				nodesToVisit.push_front(pTempToNode);//uncomment me for depth-first search
				//nodesToVisit.push_back( pTempToNode );//uncomment me for breadth-first search
				pTempToNode->setPrevNode(pCurrentNode);
				if (pTempToNode == pTo)
				{
					toNodeAdded = true;
				}
#ifdef VISUALIZE_PATH
				mVisitedNodes.push_back(pTempToNode);
#endif

			}
		}
	}

	gpPerformanceTracker->stopTracking("path");
	mTimeElapsed = gpPerformanceTracker->getElapsedTime("path");

#ifdef VISUALIZE_PATH
	Node* lastPathNode = pTo;
	delete pPath;
	pPath = new Path();
	while (lastPathNode != pFrom) {
		pPath->addNode(lastPathNode);
		lastPathNode = lastPathNode->getPrevNode();
		//emergency case
		if (lastPathNode == nullptr) {
			lastPathNode = pFrom;
		}
	}

	static_cast<GameApp*>(gpGame)->CachePath(pFrom, pTo, pPath);

	mpPath = pPath;
#endif
	return pPath;

}

