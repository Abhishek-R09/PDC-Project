#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits.h>

using namespace std;
using namespace std::chrono;

int minDistance(vector<float> dist, vector<bool> sptSet)
{
  int V = dist.size();
  int min = INT_MAX, min_index;

  for (int v = 0; v < V; v++)
    if (sptSet[v] == false && dist[v] <= min)
      min = dist[v], min_index = v;

  return min_index;
}

void printSolution(vector<float> dist)
{
  int V = dist.size();
  cout << "Vertex \t Distance from Source" << endl;
  for (int i = 0; i < V; i++)
    cout << i << " \t\t" << dist[i] << endl;
}

void dijkstra(vector<vector<float>> graph, int src)
{
  int V = graph.size();
  vector<float> dist(V);

  vector<bool> sptSet(V);
  for (int i = 0; i < V; i++)
    dist[i] = INT_MAX, sptSet[i] = false;

  dist[src] = 0;

  auto start = high_resolution_clock::now();
  for (int count = 0; count < V - 1; count++)
  {
    int u = minDistance(dist, sptSet);

    sptSet[u] = true;

    for (int v = 0; v < V; v++)
      if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
        dist[v] = dist[u] + graph[u][v];
  }
  auto stop = high_resolution_clock::now();

  auto timeTaken = duration_cast<microseconds>(stop - start).count();
  cout << "Total time taken for serial Dijkstra computation = " << timeTaken;
  cout << " microseconds\n";
  printSolution(dist);
}

int main()
{

  float infinite = 2147483647;

  ifstream inFile;

  inFile.open(".\\data\\India\\India.csv");
  if (!inFile.is_open())
  {
    cerr << "File not found!\n";
    exit(1);
  }

  string cityCSV;
  inFile >> cityCSV;

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

  int V = distanceMatrix.size();

  dijkstra(distanceMatrix, 0);

  return 0;
}