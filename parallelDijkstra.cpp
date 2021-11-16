#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <omp.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char **argv);
vector<float> dijkstraAlgorithm(vector<vector<float>> &distanceMatrix,
                                vector<int> &parent);
void findNearestNode(int startIdx, int endIdx, vector<float> &minimumDistances,
                     vector<bool> &connected, float *threadMinimumDistance,
                     int *threadNearestNode);
vector<vector<float>> initializeData(vector<string> &cities);
void updateMinimumDistance(int startIdx, int endIdx, int nearestNode,
                           vector<bool> &connected,
                           vector<vector<float>> &distanceMatrix,
                           vector<float> &minimumDistances,
                           vector<int> &parent);
void printPath(vector<int> parent, int j, vector<string> cities);

/* 

Main method which calls the required functions to get data, do computations on
the data and print the results.

Params: number of arguments, argument list
Returns: integer

*/
int main(int argc, char **argv)
{
  int i;
  int infinite = 2147483647;
  int j;

  cout << "----- Welcome to Travel Partner -----\n";

  vector<string> cities;
  vector<vector<float>> distanceMatrix = initializeData(cities);

  int nodeCount = distanceMatrix.size();

  cout << "List of Available cities under this plan: \n";
  for (int i = 0; i < nodeCount; ++i)
  {
    cout << "\t" << i + 1 << ". " << cities[i] << "\n";
  }

  cout << "Which city would you like to start from?\n";
  cout << "Enter the corresponding row number: ";
  int cityIdx;
  cin >> cityIdx;
  --cityIdx;
  cout << "You have chosen '" << cities[cityIdx] << "' to start with.\n";

  string temp = cities[cityIdx];
  cities[cityIdx] = cities[0];
  cities[0] = temp;
  for (int i = 0; i < nodeCount; ++i)
  {
    float tempVal = distanceMatrix[i][cityIdx];
    distanceMatrix[i][cityIdx] = distanceMatrix[i][0];
    distanceMatrix[i][0] = tempVal;
  }

  for (int i = 0; i < nodeCount; ++i)
  {
    float tempVal = distanceMatrix[cityIdx][i];
    distanceMatrix[cityIdx][i] = distanceMatrix[0][i];
    distanceMatrix[0][i] = tempVal;
  }

  cout << "\n";
  cout << "---------- Distance matrix ----------\n";
  cout << "\n";
  cout << "  " << setw(3) << "  " << setw(15) << "Maharashtra";
  for (i = 0; i < nodeCount; i++)
  {
    cout << "  " << setw(cities[i].length()) << cities[i];
  }
  cout << "\n";
  for (i = 0; i < nodeCount; i++)
  {
    cout << i + 1 << "." << setw(3) << "  " << setw(15) << cities[i];
    for (j = 0; j < nodeCount; j++)
    {
      if (distanceMatrix[i][j] == infinite)
      {
        cout << "  " << setw(cities[j].length()) << "Inf";
      }
      else
      {
        cout << "  " << setw(cities[j].length()) << distanceMatrix[i][j];
      }
    }
    cout << "\n";
  }

  cout << "\nWhich cities would you like to consider in your travel plan?\n";
  cout << "Enter the corresponding row numbers separated by comma from the "
       << "above matrix: ";
  vector<int> toVisit;
  string visitIdx;
  cin >> visitIdx;
  stringstream ss(visitIdx);
  while (ss.good())
  {
    string substr;
    getline(ss, substr, ',');
    stringstream dist(substr);
    int x;
    dist >> x;
    --x;
    toVisit.push_back(x);
  }

  cout << "You would like to visit:\n";
  for (int i = 0; i < toVisit.size(); i++)
  {
    cout << "\t" << (i + 1) << ". " << cities[toVisit[i]] << "\n";
  }

  vector<int> parent(nodeCount);
  parent[0] = -1;

  vector<float> minimumDistances = dijkstraAlgorithm(distanceMatrix, parent);

  cout << "\n";
  cout << "----- The Best Travel Plans for your desired cities is given below"
       << " -----\n";
  cout << "Here are the shortest paths to travel to your desired destinations "
       << "spending less time travelling and more time enjoying your stay!\n";
  // cout << "The Minimum distances from " << cities[0] << " To -\n";

  for (i = 0; i < toVisit.size(); i++)
  {
    cout << "\t" << i + 1 << ". " << cities[0] << " -> " << cities[toVisit[i]]
         << " = " << minimumDistances[toVisit[i]] << " KM\n";
    cout << "\tTourist places on your way: " << cities[0] << " -> ";
    printPath(parent, toVisit[i], cities);
    cout << "END\n";
  }

  return 0;
}

/* 

Function which calculates the shortest path using Dijkstra's parallel algorithm.

It uses OpenMP constructs to parallelize the computation and returns the 
minimum distance vector

Params: distance matrix, parent vector reference
Returns: Minimum distances vector

*/
vector<float> dijkstraAlgorithm(vector<vector<float>> &distanceMatrix,
                                vector<int> &parent)
{
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

  double start = omp_get_wtime();

#pragma omp parallel private(threadFirst, threadID, threadLast, threadMinDistance, threadNearestNode, threadIterationCount) \
    shared(connected, minDistance, minimumDistances, nearestNode, numOfThreads, distanceMatrix)
  {
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
  double end = omp_get_wtime();
  double timeTaken = end - start;
  cout << "Total time taken for parallel Dijkstra computation = " << timeTaken;
  cout << "\n";
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

Function to take the state as user input and fetch the required distances data
from data/{stateName}.csv file.

Updates the cities list and constructs the distance matrix.

Params: cities vector reference
Returns: Distance matrix

*/
vector<vector<float>> initializeData(vector<string> &cities)
{
  float infinite = 2147483647;

  ifstream inStatesFile;
  inStatesFile.open(".\\data\\States.csv");
  if (!inStatesFile.is_open())
  {
    cerr << "States File not found!\n";
    exit(1);
  }
  vector<string> states;
  string state;
  while (!inStatesFile.eof())
  {
    getline(inStatesFile, state, '\n');
    if (!inStatesFile.good())
    {
      break;
    }
    states.push_back(state);
  }
  inStatesFile.close();
  cout << "List of Available States for Travel: \n";
  for (int i = 1; i < states.size(); i++)
  {
    cout << "\t" << i << ". " << states[i] << "\n";
  }
  cout << "We also have Pan India Travel option\n";
  cout << "Choose a state where you are planning to travel with ";
  cout << "the corresponding row number or Choose '0' if you plan to travel "
       << "across India: ";
  int stateNum;
  cin >> stateNum;
  if (stateNum == 0)
  {
    cout << "You choose to travel Pan India\n";
  }
  else if (stateNum < states.size())
  {
    cout << "You choose to go to: " << states[stateNum] << "\n";
  }
  else
  {
    cerr << "Option out of bound\n";
    exit(0);
  }
  cout << "----- Welcome to " << states[stateNum] << " Tourism -----\n";
  ifstream inFile;

  inFile.open(".\\data\\" + states[stateNum] + "\\" +
              states[stateNum] + ".csv");
  if (!inFile.is_open())
  {
    cerr << "File not found!\n";
    exit(1);
  }

  string cityCSV;
  inFile >> cityCSV;
  stringstream citySS(cityCSV);
  string first;
  getline(citySS, first, ',');
  while (citySS.good())
  {
    string city;
    getline(citySS, city, ',');
    cities.push_back(city);
  }
  string row;

  vector<vector<float>> distanceMatrix;

  while (!inFile.eof())
  {
    inFile >> row;
    vector<float> distances;
    if (!inFile.good())
    {
      break;
    }
    stringstream ss(row);
    string firstCity;
    getline(ss, firstCity, ',');
    while (ss.good())
    {
      string substr;
      getline(ss, substr, ',');
      stringstream dist(substr);
      float x;
      dist >> x;
      if (x == -1)
      {
        distances.push_back(infinite);
      }
      else
      {
        distances.push_back(x);
      }
    }
    distanceMatrix.push_back(distances);
  }
  inFile.close();

  return distanceMatrix;
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

/* 

Function to print the path from starting node to the given node.

Params: parent vector, destination node, cities vector
Returns: void

*/
void printPath(vector<int> parent, int j, vector<string> cities)
{
  if (parent[j] == -1)
    return;

  printPath(parent, parent[j], cities);

  cout << cities[j] << " -> ";
}