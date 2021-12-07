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
// #include <chrono>
// #include <omp.h>
// #include <limits.h>
using namespace std;

// Number of vertices in the graph
// #define V 9

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int min_disance(vector<int> dist, vector<bool> spt_set, int V)
{
  // Initialize min value
  int min = INT_MAX, min_index = 0;

  for (int v = 0; v < V; v++)
    if (spt_set[v] == false && dist[v] <= min)
      min = dist[v], min_index = v;

  return min_index;
}

// A utility function to print the constructed distance array
void print_distances(vector<int> dist, int V)
{
  cout << "Vertex \t Distance from Source" << endl;
  for (int i = 0; i < V; i++)
    cout << i << " \t\t" << dist[i] << endl;
}

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency/distance matrix representation
vector<int> serial_dijkstra(vector<vector<int>> graph, int src)
{
  int V = graph.size();
  assert(graph.size() == graph[0].size());
  vector<int> dist(V, INT_MAX); // The output array. dist[i] will hold the shortest
  // distance from src to i

  vector<bool> spt_set(V, false); // spt_set[i] will be true if vertex i is included in shortest
  // path tree or shortest distance from src to i is finalized

  // Distance of source vertex from itself is always 0
  dist[src] = 0;

  // Find shortest path for all vertices
  for (int count = 0; count < V - 1; count++)
  {
    // Pick the minimum distance vertex from the set of vertices not
    // yet processed. u is always equal to src in the first iteration.
    int u = min_disance(dist, spt_set, V);

    // Mark the picked vertex as processed
    spt_set[u] = true;

    // Update dist value of the adjacent vertices of the picked vertex.
    for (int v = 0; v < V; v++)

      // Update dist[v] only if is not in spt_set, there is an edge from
      // u to v, and total weight of path from src to v through u is
      // smaller than current value of dist[v]
      if (!spt_set[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
        dist[v] = dist[u] + graph[u][v];
  }
  // print_distances(dist);
  return dist;
}

// int main() {
//     cout << "Serial implementation of Dijkstra's Algorithm\n";
//     vector<vector<int>> graph = {{0, 4, 0, 0, 0, 0, 0, 8, 0},
//                                  {4, 0, 8, 0, 0, 0, 0, 11, 0},
//                                  {0, 8, 0, 7, 0, 4, 0, 0, 2},
//                                  {0, 0, 7, 0, 9, 14, 0, 0, 0},
//                                  {0, 0, 0, 9, 0, 10, 0, 0, 0},
//                                  {0, 0, 4, 14, 10, 0, 2, 0, 0},
//                                  {0, 0, 0, 0, 0, 2, 0, 1, 6},
//                                  {8, 11, 0, 0, 0, 0, 1, 0, 7},
//                                  {0, 0, 2, 0, 0, 0, 6, 7, 0}};

//     serial_dijkstra(graph, 0);
//     return 0;
// }
