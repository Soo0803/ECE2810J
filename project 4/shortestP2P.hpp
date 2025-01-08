#include<iostream>
#include<list>
#include<vector>
#include<climits>
// You are not allowed to include additional libraries

#define INF INT_MAX

using namespace std;

class ShortestP2P {
  public:
      ShortestP2P() {}

      /* Read the graph from stdin
       * The input has the following format:
       *
       * Firstline: 1 unsigned int, the total number of verticies, X.
       * Secondline: 1 unsigned int, the total number of edges between vertices, Y.
       * A set of Y lines, each line contains tuple of 2 unsigned int and 1 int (not unsigned int!), in the format of: A(point 1, unsigned) B(point 2, unsigned) dist(distance, int)
       *
       * Example:
       * 4
       * 4
       * 0 1 5
       * 1 2 -1
       * 2 3 4
       * 3 1 6
       * 
       * 
       * 
       *
       * Vertices that are not connected have an infinitly large distance. You may use INF (previously defined at the top of this cope snippet) for infinitly large distance.
       * 
       * Special: when the graph is not proper, where there exist any pair of vertices whose minimum distance does not exist, terminate immediately by printing:
       * cout << "Invalid graph. Exiting." << endl;
       *
       * Note: vertex pairs that are not connected, which have infinitely large distances are not considered cases where "minimum distances do not exist".
       */
      void readGraph();

      /* Input: 2 vertices A and B
       * Output: distance between them.
       * cout << dist << endl;
       *
       * When the A and B are not connected print INF:
       * cout << "INF" << endl;
       */
      void distance(unsigned int A, unsigned int B);

      /*
        implement floydwarshall algorithm to calculate the shortest path distance
      */
      void floydWarshall();

  private:
    // internal data and functions.
    int num_of_nodes;
    int num_of_edges;
    vector<vector<int> > matrix; // adjacent matrix to encode the graph connections

};

void ShortestP2P::readGraph(){
  cin >> num_of_nodes;
  cin >> num_of_edges;

  //resize the adjacent matrix base on the cin nodes number 
  matrix.resize((unsigned long)num_of_nodes, vector<int>((unsigned long)num_of_nodes, INF));


  for (int i = 0; i < num_of_edges; i ++) {
    size_t start_node, end_node;
    int weight;

    cin >> start_node >> end_node >> weight;
    if (start_node == end_node) {
      matrix[start_node][end_node] = 0;
      continue;
    }
    if (start_node >= (size_t)num_of_nodes || end_node >= (size_t)num_of_nodes) {
      cout << "Invalid graph. Exiting." << endl;
      exit(0);
    }
    // assigned the weight in the adjecent matrix
    matrix[start_node][end_node] = weight;

  }

  //find the shortest path for each nodes path 
  floydWarshall();

}

void ShortestP2P::floydWarshall() {
    // Apply Floyd-Warshall
    for (size_t k = 0; k < (size_t)num_of_nodes; ++k) {
      if (matrix[k][k] < 0) {
        // negative nodes exist 
        cout << "Invalid graph. Exiting." << endl;
        exit(0);
      }
      for (size_t i = 0; i < (size_t)num_of_nodes; ++i) {
        if (matrix[i][k] == INF) continue; 
        if (matrix[i][i] < 0) {
          // negative nodes exist 
          cout << "Invalid graph. Exiting." << endl;
          exit(0);
        }
        for (size_t j = 0; j < (size_t)num_of_nodes; ++j) {
          if (matrix[k][j] == INF) continue; // No edge between node i k and node k and j
          if (matrix[j][j] < 0) {
            // negative nodes exist 
            cout << "Invalid graph. Exiting." << endl;
            exit(0);
          }
          int new_weight = matrix[i][k] + matrix[k][j];
          if (new_weight < matrix[i][j]) {
              matrix[i][j] = new_weight;
          }
        }
      }
    }

}

void ShortestP2P::distance(unsigned int A, unsigned int B) {

  if (A == B) {
    cout << 0 << endl;
    return;
  }
  int dist = matrix[A][B];
  if (dist == INF) {
    cout << "INF" << endl;
    return;
  } 
  else {
    cout << dist << endl;
    return;
  }
}