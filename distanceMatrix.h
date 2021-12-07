#include <iostream>
#include <vector>
// #include <assert.h>
using namespace std;

// void print_distance_matrix(vector<vector<int>> matrix)
// {
//   int n = matrix.size();
//   assert(n > 0 && (matrix.size() == matrix[0].size()));
//   for (int i = 0; i < n; i++)
//   {
//     for (int j = 0; j < n; j++)
//     {
//       cout << matrix[i][j] << " ";
//     }
//     cout << std::endl;
//   }
// }

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

  fstream inStatesFile;
  inStatesFile.open("./data/States.csv");
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
  for (int i = 1; i < (int)states.size(); i++)
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
  else if (stateNum < (int)states.size())
  {
    cout << "You choose to go to: " << states[stateNum] << "\n";
  }
  else
  {
    cerr << "Option out of bound\n";
    exit(0);
  }
  cout << "----- Welcome to " << states[stateNum] << " Tourism -----\n";
  fstream inFile;

  // cout << states[stateNum] << '\n';

  std::stringstream newss;

  newss << "./data/" << states[stateNum] << "/" << states[stateNum] << ".csv";

  string stateFilePath = newss.str();

  // cout << stateFilePath << "\n";
  inFile.open(stateFilePath);
  if (!inFile.is_open())
  {
    cerr << "****** File not found! *****\n";
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

// vector<vector<int>> generate_random_distance_matrix(int N)
// {
//   vector<vector<int>> matrix(N, vector<int>(N, 0));
//   for (int i = 0; i < N; ++i)
//   {
//     for (int j = i + 1; j < N; ++j)
//     {
//       if (i == j)
//       {
//         matrix[i][j] = 0;
//       }
//       else
//       {
//         matrix[i][j] = matrix[j][i] = rand() % 100 + 1;
//       }
//     }
//   }
//   return matrix;
// }

vector<vector<float>> generate_random_distance_matrix(int N)
{
  vector<vector<float>> matrix(N, vector<float>(N, 0));
  for (int i = 0; i < N; ++i)
  {
    for (int j = i + 1; j < N; ++j)
    {
      if (i == j)
      {
        matrix[i][j] = 0;
      }
      else
      {
        matrix[i][j] = matrix[j][i] = rand() % 100 + 1;
      }
    }
  }
  return matrix;
}

// int main() {
//     int N = 5;
//     cout << "Enter the size of distance matrix (N x N): ";
//     cin >> N;
//     vector<vector<int>> matrix = generate_random_distance_matrix(N);
//     print_distance_matrix(matrix);
// }