#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;
using namespace std;

class Graph {
    private:
    using AdjList = cc_hash_table<int, null_type, hash<int>>;
    int vertexCount;
    vector<AdjList> adj;
    size_t edgeCount;

    public:
    Graph(int vertices);
    void addEdge(int v, int w);
    bool hasEdge(int v, int w) const;
    vector<int> getNeighbors(int v) const;
    void removeEdge(int v, int w);
    int getVertexCount() const;
    int getDegree(int v) const;
    bool isEmpty() const;
    void clear();
};
