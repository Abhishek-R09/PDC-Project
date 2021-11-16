#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

int main()
{
  ifstream inFile;

  inFile.open("ne.csv");
  if (inFile.is_open())
  {
    cout << "Opened\n";
  }
  else
  {
    cout << "Not opened\n";
  }

  string cityCSV;
  inFile >> cityCSV;
  stringstream citySS(cityCSV);
  vector<string> cities;
  string first;
  getline(citySS, first, ',');
  while (citySS.good())
  {
    string city;
    getline(citySS, city, ',');
    cities.push_back(city);
  }
  for (int i = 0; i < cities.size(); ++i)
  {
    cout << cities[i] << " ";
  }
  cout << "\n";
  string row;

  vector<vector<float>> distanceMatrix;

  while (!inFile.eof())
  {
    inFile >> row;
    vector<float> distances;

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
      distances.push_back(x);
    }
    distanceMatrix.push_back(distances);
  }
  inFile.close();
  for (int i = 0; i < cities.size(); i++)
  {
    for (int j = 0; j < cities.size(); j++)
      cout << distanceMatrix[i][j] << " ";
    cout << "\n";
  }
  return 0;
}