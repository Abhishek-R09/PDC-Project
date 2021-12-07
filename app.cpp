#include <bits/stdc++.h>
#include <omp.h>

#include "parallelDijkstra.h"
#include "parallel_dijkstra.h"
#include "serial_dijkstra.h"
#include "distanceMatrix.h"

// const int COUT_PRECISION = 16;
// const int START_N = 500;
// const int END_N = 10000;
// const int STEP_N = 1000;

// const vector<int> num_threads = {2, 4, 6, 8, 10, 12};

using namespace std;
using namespace std::chrono;

template <typename T>
ostream &operator<<(ostream &out, const vector<T> &v)
{
  out << "{";
  size_t last = v.size() - 1;
  for (size_t i = 0; i < v.size(); ++i)
  {
    out << v[i];
    if (i != last)
      out << ", ";
  }
  out << "}";
  return out;
}

// vector<vector<float>> initializeData(vector<string> &cities);

void printPath(vector<int> parent, int j, vector<string> cities, vector<pair<string, int>> &route, map<string, int> daysMap);

void findCombinations(vector<vector<pair<string, int>>> &combs, vector<pair<string, int>> route, int start, int end, vector<pair<string, int>> currComb);

/*

Main method which calls the required functions to get data, do computations on
the data and print the results.

Params: number of arguments, argument list
Returns: integer

*/
int main(int argc, char **argv)
{
  // int i;

  int choice;
  cout << "Welcome!\n";
  cout << "How do you want to run?\n";
  cout << "\t1. Application Mode\n";
  cout << "\t2. Benchmarking Mode\n";
  cout << "Enter your choice: ";
  cin >> choice;

  if (choice == 1)
  {

    // int infinite = 2147483647;
    // int j;

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
    // cout << "---------- Distance matrix ----------\n";
    // cout << "\n";
    // cout << "  " << setw(3) << "  " << setw(15) << "Maharashtra";
    // for (int i = 0; i < nodeCount; i++)
    // {
    //   cout << "  " << setw(cities[i].length()) << cities[i];
    // }
    // cout << "\n";
    // for (int i = 0; i < nodeCount; i++)
    // {
    //   cout << i + 1 << "." << setw(3) << "  " << setw(15) << cities[i];
    //   for (int j = 0; j < nodeCount; j++)
    //   {
    //     if (distanceMatrix[i][j] == infinite)
    //     {
    //       cout << "  " << setw(cities[j].length()) << "Inf";
    //     }
    //     else
    //     {
    //       cout << "  " << setw(cities[j].length()) << distanceMatrix[i][j];
    //     }
    //   }
    //   cout << "\n";
    // }

    for (int i = 0; i < nodeCount; i++)
    {
      cout << "\t" << i + 1 << ". " << cities[i] << "\n";
    }

    cout << "\nWhich cities would you like to consider in your travel plan?\n";
    cout << "Enter the corresponding numbers separated by comma from the "
         << "above list: ";
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
    for (int i = 0; i < (int)toVisit.size(); i++)
    {
      cout << "\t" << (i + 1) << ". " << cities[toVisit[i]] << "\n";
    }

    vector<int> parent(nodeCount);
    parent[0] = -1;

    vector<float> minimumDistances = parallelDijkstra(distanceMatrix, parent, 6);

    fstream inFile;
    string daysFilePath = "./data/days.csv";

    inFile.open(daysFilePath);
    if (!inFile.is_open())
    {
      cerr << "**** File not found! *****\n";
      exit(1);
    }

    map<string, int> daysMap;
    string row;

    while (!inFile.eof())
    {
      inFile >> row;
      if (!inFile.good())
      {
        break;
      }
      stringstream newss(row);
      string firstCity;
      getline(newss, firstCity, ',');
      string substr;
      getline(newss, substr, ',');
      stringstream cityDays(substr);
      int x;
      cityDays >> x;
      daysMap[firstCity] = x;
    }
    inFile.close();

    // for (auto it1 = daysMap.begin(); it1 != daysMap.end(); ++it1)
    //   cout << it1->first << "->" << it1->second << endl;

    int constraints;
    cout << "Any constraints on your travel plan?\n";
    cout << "\tYes = 1; No = 0: ";
    cin >> constraints;

    int daysConstraint = 0;
    if (constraints)
    {
      cout << "For how many days you can stay? ";
      cin >> daysConstraint;
    }

    cout << "\n";
    cout << "----- The Best Travel Plans for your desired cities is given below"
         << " -----\n";
    cout << "Here are the shortest paths to travel to your desired destinations "
         << "spending less time travelling and more time enjoying your stay!\n";
    // cout << "The Minimum distances from " << cities[0] << " To -\n";

    for (int i = 0; i < (int)toVisit.size(); i++)
    {
      vector<pair<string, int>> route;
      cout << "\t" << i + 1 << ". " << cities[0] << " -> " << cities[toVisit[i]]
           << " = " << minimumDistances[toVisit[i]] << " KM\n";
      cout << "\tTourist places on your way: " << cities[0] << " -> ";
      route.push_back(make_pair(cities[0], daysMap[cities[0]]));
      printPath(parent, toVisit[i], cities, route, daysMap);
      cout << "END\n";
      if (constraints)
      {
        vector<vector<pair<string, int>>> combs;
        for (int z = 0; z < (int)route.size(); ++z)
        {
          vector<pair<string, int>> currComb;
          currComb.push_back(make_pair(route[z].first, route[z].second));
          combs.push_back(currComb);
          // cout << "z: " << z << "\n";
          findCombinations(combs, route, z + 1, (int)route.size(), currComb);
        }
        cout << "Suggesting you best possible travel places...\n";
        int found = false;
        int plans = 0;
        for (int j = 0; j < (int)combs.size(); ++j)
        {
          int totalDays = 0;
          for (int k = 0; k < (int)combs[j].size(); ++k)
          {
            totalDays += combs[j][k].second;
            // cout << combs[j][k].first << "->" << combs[j][k].second << ", ";
          }
          if (totalDays <= daysConstraint)
          {
            ++plans;
            found = true;
            cout << "\tPlan " << plans << ":\n";
            int dayCount = 0;
            for (int k = 0; k < (int)combs[j].size(); ++k)
            {
              dayCount += combs[j][k].second;
              cout << "\t\t" << combs[j][k].first << ": " << combs[j][k].second << " Days\n";
            }
            cout << "\t\t" << string(15, '-') << "\n";
            cout << "\t\tTotal Days = " << dayCount << "\n";
          }
          // cout << "\n";
        }
        if (!found)
        {
          cout << "Sorry! Can't find suitable travel plan for the given constraints. :(\n";
        }
        else
        {
          cout << "Found " << plans << " plans! Happy Holidays :)\n";
        }
        // for (auto it1 = route.begin(); it1 != route.end(); ++it1)
        //   cout << it1->first << "->" << it1->second << endl;
      }
    }
  }
  else if (choice == 2)
  {
    cout << "----- Benchmarking! -----\n";
    const vector<int> num_threads = {2, 4, 6, 8, 10, 12};
    const int COUT_PRECISION = 16;
    int START_N = 500;
    cout << "Enter number of nodes to start with: ";
    cin >> START_N;
    int END_N = 10000;
    cout << "Enter number of nodes upto which benchmarking has to be done: ";
    cin >> END_N;
    int STEP_N = 1000;
    cout << "Enter the increment value: ";
    cin >> STEP_N;
    ofstream dijkstra_time;
    cout << setprecision(COUT_PRECISION);

    // dijkstra_time.open("dijkstra_time.csv");  // for [500, 25000] with increment of 500
    // dijkstra_time.open("dijkstra_time_small.csv");  // for [10, 500] with increment of 10
    dijkstra_time.open("sample_d.csv"); // for [500, 25000] with increment of 500
    // dijkstra_time.open("test.csv");  // for [500, 25000] with increment of 500

    dijkstra_time << "Number of nodes (N),Serial Time (\u03BCs)";
    for (int num_thread : num_threads)
    {
      dijkstra_time << ","
                    << "Parallel time with " << num_thread << " threads (\u03BCs)";
    }
    dijkstra_time << endl;

    printf(
        "Starting to calculate Dijkstra's algorithm for number of nodes in the range [%d, %d] "
        "with an increment of %d\n",
        START_N, END_N, STEP_N);
    cout << "Also, testing for the following number of threads: " << num_threads << endl;

    for (int i = START_N; i <= END_N; i += STEP_N)
    {
      vector<vector<float>> matrix = generate_random_distance_matrix(i);
      int nodeCount = matrix.size();

      // cout << "The randomly generated distance matrix is: \n";
      // print_distance_matrix(matrix);

      vector<int> serialParent(nodeCount);
      serialParent[0] = -1;
      auto start = high_resolution_clock::now();
      // vector<int> serial_shortest_path = serial_dijkstra(matrix, 0);
      vector<float> serial_shortest_path = parallelDijkstra(matrix, serialParent, 1);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);

      // cout << "The shortest path with Dijkstra (Serial) is: " << serial_shortest_path << endl;
      dijkstra_time << i << "," << duration.count();

      for (int num_thread : num_threads)
      {
        vector<int> parent(nodeCount);
        parent[0] = -1;
        start = high_resolution_clock::now();
        // vector<int> parallel_dijkstra_path = parallel_dijkstra(matrix, num_thread);
        vector<float> parallel_dijkstra_path = parallelDijkstra(matrix, parent, num_thread);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);

        dijkstra_time << "," << duration.count();
        // cout << "The shortest path with Dijkstra (Parallel with " << num_thread << " threads) is: " <<
        // parallel_dijkstra_path << endl;
        assert(serial_shortest_path == parallel_dijkstra_path);
      }
      dijkstra_time << endl;
    }
    cout << "Finished computations and the results are stored in 'sample_d.csv'\n";
    cout << "Open Plot.ipynb to visualize the results...\n";
    // system("python plot_serial_vs_parallel.py");
  }
  else
  {
    cout << "Invalid choice!\n";
  }

  return 0;
}

/*

Function to print the path from starting node to the given node.

Params: parent vector, destination node, cities vector
Returns: void

*/
void printPath(vector<int> parent, int j, vector<string> cities, vector<pair<string, int>> &route, map<string, int> daysMap)
{
  if (parent[j] == -1)
    return;

  printPath(parent, parent[j], cities, route, daysMap);

  cout << cities[j] << " -> ";
  // route.insert(route.begin() + 1, make_pair(cities[j], daysMap[cities[j]]));
  route.push_back(make_pair(cities[j], daysMap[cities[j]]));
}

void findCombinations(vector<vector<pair<string, int>>> &combs, vector<pair<string, int>> route, int start, int end, vector<pair<string, int>> currComb)
{
  for (int i = start; i < end; ++i)
  {
    currComb.push_back(make_pair(route[i].first, route[i].second));
    combs.push_back(currComb);
    findCombinations(combs, route, i + 1, end, currComb);
    currComb.pop_back();
  }
}