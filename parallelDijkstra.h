#include <bits/stdc++.h>
// #include <cstdlib>
// #include <iostream>
// #include <iomanip>
// #include <ctime>
// #include <omp.h>
// #include <fstream>
// #include <vector>
// #include <string>
// #include <sstream>
#include <chrono>

using namespace std;

vector<float> parallelDijkstra(vector<vector<float>> &distanceMatrix,
                               vector<int> &parent, int num_threads);
void findNearestNode(int startIdx, int endIdx, vector<float> &minimumDistances,
                     vector<bool> &connected, float *threadMinimumDistance,
                     int *threadNearestNode);
void updateMinimumDistance(int startIdx, int endIdx, int nearestNode,
                           vector<bool> &connected,
                           vector<vector<float>> &distanceMatrix,
                           vector<float> &minimumDistances,
                           vector<int> &parent);

/*

Function which calculates the shortest path using Dijkstra's parallel algorithm.

It uses OpenMP constructs to parallelize the computation and returns the
minimum distance vector

Params: distance matrix, parent vector reference
Returns: Minimum distances vector

*/
vector<float> parallelDijkstra(vector<vector<float>> &distanceMatrix,
                               vector<int> &parent, int num_threads)
{

  omp_set_num_threads(num_threads);

  int i;
  float infinite = 2147483647;
  float minDistance;
  int nearestNode;
  int threadFirst;
  int threadID;
  int threadLast;
  float threadMinDistance;
  int threadNearestNode;
  int threadIterationCount;
  int numOfThreads;

  int nodeCount = distanceMatrix.size();
  vector<bool> connected(nodeCount);
  connected[0] = true;
  for (i = 1; i < nodeCount; i++)
  {
    connected[i] = false;
  }
  vector<float> minimumDistances(nodeCount);

  for (i = 0; i < nodeCount; i++)
  {
    minimumDistances[i] = distanceMatrix[0][i];
  }

#pragma omp parallel private(threadFirst, threadID, threadLast, threadMinDistance, threadNearestNode, threadIterationCount) \
    shared(connected, minDistance, minimumDistances, nearestNode, numOfThreads, distanceMatrix)
  {
    // cout << omp_get_num_threads() << "\n";
    threadID = omp_get_thread_num();
    numOfThreads = omp_get_num_threads();
    threadFirst = (threadID * nodeCount) / numOfThreads;
    threadLast = ((threadID + 1) * nodeCount) / numOfThreads - 1;

    //  Attach one more node on each iteration.
    for (threadIterationCount = 1; threadIterationCount < nodeCount; threadIterationCount++)
    {
      /*
        Before we compare the results of each thread, set the shared variable
        MD to a big value.  Only one thread needs to do this.
      */
#pragma omp single
      {
        minDistance = infinite;
        nearestNode = -1;
      }
      /*
        Each thread finds the nearest unconnected node in its part of the
        graph.
        Some threads might have no unconnected nodes left.
      */
      findNearestNode(threadFirst, threadLast, minimumDistances, connected,
                      &threadMinDistance, &threadNearestNode);
      /*
        In order to determine the minimum of all the MY_MD's, we must insist
        that only one thread at a time execute this block!
      */
#pragma omp critical
      {
        if (threadMinDistance < minDistance)
        {
          minDistance = threadMinDistance;
          nearestNode = threadNearestNode;
        }
      }
      /*
        This barrier means that ALL threads have executed the critical
        block, and therefore MD and MV have the correct value.  Only then
        can we proceed.
      */
#pragma omp barrier
      /*
        If MV is -1, then NO thread found an unconnected node, so we're done
        early.
        OpenMP does not like to BREAK out of a parallel region, so we'll just
       have
        to let the iteration run to the end, while we avoid doing any more
       updates.
        Otherwise, we connect the nearest node.
      */
#pragma omp single
      {
        if (nearestNode != -1)
        {
          connected[nearestNode] = true;
        }
      }
      /*
        Again, we don't want any thread to proceed until the value of
        CONNECTED is updated.
      */
#pragma omp barrier
      /*
        Now each thread should update its portion of the MIND vector,
        by checking to see whether the trip from 0 to MV plus the step
        from MV to a node is closer than the current record.
      */
      if (nearestNode != -1)
      {
        updateMinimumDistance(threadFirst, threadLast, nearestNode, connected,
                              distanceMatrix, minimumDistances, parent);
      }
      /*
        Before starting the next step of the iteration, we need all threads
        to complete the updating, so we set a BARRIER here.
      */
#pragma omp barrier
    }
    //  Once all the nodes have been connected, we can exit.
  }
  return minimumDistances;
}

/*

Function to find the nearest unconnected node for the current thread.

It updates the current thread's minimum distance and unconnected node found by
the thread (if any) .

Params: start index, end index, minimum distances vector, connected vector,
        thread's minimum distance, thread's nearest unconnected node
Returns: void

*/
void findNearestNode(int startIdx, int endIdx, vector<float> &minimumDistances,
                     vector<bool> &connected,
                     float *threadMinimumDistance, int *threadNearestNode)
{
  int i;
  float infinite = 2147483647;

  *threadMinimumDistance = infinite;
  *threadNearestNode = -1;
  for (i = startIdx; i <= endIdx; i++)
  {
    if (!connected[i] && minimumDistances[i] < *threadMinimumDistance)
    {
      *threadMinimumDistance = minimumDistances[i];
      *threadNearestNode = i;
    }
  }
  return;
}

/*

Function to update the minimum distance in the shared minimum distances vector.

Also updates the parent node in the parent list.

Params: start index, end index, nearest node, connected vector, distance
        matrix, minimum distances vector, parent vector
Returns: void

*/
void updateMinimumDistance(int startIdx, int endIdx, int nearestNode,
                           vector<bool> &connected,
                           vector<vector<float>> &distanceMatrix,
                           vector<float> &minimumDistances, vector<int> &parent)
{
  int i;
  float infinite = 2147483647;

  for (i = startIdx; i <= endIdx; i++)
  {
    if (!connected[i])
    {
      if (distanceMatrix[nearestNode][i] < infinite)
      {
        if (minimumDistances[nearestNode] +
                distanceMatrix[nearestNode][i] <
            minimumDistances[i])
        {
          parent[i] = nearestNode;
          minimumDistances[i] = minimumDistances[nearestNode] +
                                distanceMatrix[nearestNode][i];
        }
      }
    }
  }
  return;
}