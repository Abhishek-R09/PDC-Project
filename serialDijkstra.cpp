// A C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <omp.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits.h>
using namespace std;

// Number of vertices in the graph
#define V 9

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int minDistance(int dist[], bool sptSet[])
{

  // Initialize min value
  int min = INT_MAX, min_index;

  for (int v = 0; v < V; v++)
    if (sptSet[v] == false && dist[v] <= min)
      min = dist[v], min_index = v;

  return min_index;
}

// A utility function to print the constructed distance array
void printSolution(int dist[])
{
  cout << "Vertex \t Distance from Source" << endl;
  for (int i = 0; i < V; i++)
    cout << i << " \t\t" << dist[i] << endl;
}

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(int graph[V][V], int src)
{
  int dist[V]; // The output array. dist[i] will hold the shortest
  // distance from src to i

  bool sptSet[V]; // sptSet[i] will be true if vertex i is included in shortest
  // path tree or shortest distance from src to i is finalized

  // Initialize all distances as INFINITE and stpSet[] as false
  for (int i = 0; i < V; i++)
    dist[i] = INT_MAX, sptSet[i] = false;

  // Distance of source vertex from itself is always 0
  dist[src] = 0;

  // Find shortest path for all vertices
  for (int count = 0; count < V - 1; count++)
  {
    // Pick the minimum distance vertex from the set of vertices not
    // yet processed. u is always equal to src in the first iteration.
    int u = minDistance(dist, sptSet);

    // Mark the picked vertex as processed
    sptSet[u] = true;

    // Update dist value of the adjacent vertices of the picked vertex.
    for (int v = 0; v < V; v++)

      // Update dist[v] only if is not in sptSet, there is an edge from
      // u to v, and total weight of path from src to v through u is
      // smaller than current value of dist[v]
      if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
        dist[v] = dist[u] + graph[u][v];
  }

  // print the constructed distance array
  printSolution(dist);
}

// driver program to test above function
int main()
{

  // float infinite = 2147483647;

  // ifstream inStatesFile;
  // inStatesFile.open(".\\data\\States.csv");
  // if (!inStatesFile.is_open())
  // {
  //   cerr << "States File not found!\n";
  //   exit(1);
  // }
  // vector<string> states;
  // string state;
  // while (!inStatesFile.eof())
  // {
  //   getline(inStatesFile, state, '\n');
  //   if (!inStatesFile.good())
  //   {
  //     break;
  //   }
  //   states.push_back(state);
  // }
  // inStatesFile.close();
  // cout << "List of Available States for Travel: \n";
  // for (int i = 1; i < states.size(); i++)
  // {
  //   cout << "\t" << i << ". " << states[i] << "\n";
  // }
  // cout << "We also have Pan India Travel option\n";
  // cout << "Choose a state where you are planning to travel with ";
  // cout << "the corresponding row number or Choose '0' if you plan to travel "
  //      << "across India: ";
  // int stateNum;
  // cin >> stateNum;
  // if (stateNum == 0)
  // {
  //   cout << "You choose to travel Pan India\n";
  // }
  // else if (stateNum < states.size())
  // {
  //   cout << "You choose to go to: " << states[stateNum] << "\n";
  // }
  // else
  // {
  //   cerr << "Option out of bound\n";
  //   exit(0);
  // }
  // cout << "----- Welcome to " << states[stateNum] << " Tourism -----\n";
  // ifstream inFile;

  // inFile.open(".\\data\\" + states[stateNum] + "\\" +
  //             states[stateNum] + ".csv");
  // if (!inFile.is_open())
  // {
  //   cerr << "File not found!\n";
  //   exit(1);
  // }

  // string cityCSV;
  // inFile >> cityCSV;
  // stringstream citySS(cityCSV);
  // string first;
  // getline(citySS, first, ',');
  // while (citySS.good())
  // {
  //   string city;
  //   getline(citySS, city, ',');
  //   cities.push_back(city);
  // }
  // string row;

  // vector<vector<float>> distanceMatrix;

  // while (!inFile.eof())
  // {
  //   inFile >> row;
  //   vector<float> distances;
  //   if (!inFile.good())
  //   {
  //     break;
  //   }
  //   stringstream ss(row);
  //   string firstCity;
  //   getline(ss, firstCity, ',');
  //   while (ss.good())
  //   {
  //     string substr;
  //     getline(ss, substr, ',');
  //     stringstream dist(substr);
  //     float x;
  //     dist >> x;
  //     if (x == -1)
  //     {
  //       distances.push_back(infinite);
  //     }
  //     else
  //     {
  //       distances.push_back(x);
  //     }
  //   }
  //   distanceMatrix.push_back(distances);
  // }
  // inFile.close();

  /* Let us create the example graph discussed above */
  int graph[V][V] = {{0, 4, 0, 0, 0, 0, 0, 8, 0},
                     {4, 0, 8, 0, 0, 0, 0, 11, 0},
                     {0, 8, 0, 7, 0, 4, 0, 0, 2},
                     {0, 0, 7, 0, 9, 14, 0, 0, 0},
                     {0, 0, 0, 9, 0, 10, 0, 0, 0},
                     {0, 0, 4, 14, 10, 0, 2, 0, 0},
                     {0, 0, 0, 0, 0, 2, 0, 1, 6},
                     {8, 11, 0, 0, 0, 0, 1, 0, 7},
                     {0, 0, 2, 0, 0, 0, 6, 7, 0}};

  dijkstra(graph, 0);

  return 0;
}

// This code is contributed by shivanisinghss2110
