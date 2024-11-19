#include <cmath>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <vector>

using namespace __gnu_pbds;

class Graph {
    public:
    // ordered set red black tree
    template <typename T>
    using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag,
                             tree_order_statistics_node_update>;

    // DANGER heuristic parameters
    struct DangerParameters {
        // Node danger parameters
        double C = 1.0;    // Base constant
        double k = 1.0;    // Exponente para criticality
        double ku = 0.025; // Peso para uncolored neighbors
        double ka = 0.33;  // Peso para share/avail ratio

        // Color danger parameters
        double k1 = 1.0;   // Base constant para diff_neighbors
        double k2 = 1.0;   // Exponente para diff_neighbors
        double k3 = 0.5;   // Peso para uncolored(nc)
        double k4 = 0.025; // Peso para frequency
    };

    int numVertices;
    std::vector<ordered_set<int>> adjacencyList;

    tree<std::pair<int, int>, null_type, std::less<std::pair<int, int>>,
         rb_tree_tag, tree_order_statistics_node_update>
        degreeOrder;

    Graph(int n) : numVertices(n), adjacencyList(n) {
        for (int i = 0; i < n; i++) {
            degreeOrder.insert({0, i});
        }
    }

    void addEdge(int v1, int v2) {
        if (!isAdjacent(v1, v2)) {
            degreeOrder.erase({getDegree(v1), v1});
            degreeOrder.erase({getDegree(v2), v2});

            adjacencyList[v1].insert(v2);
            adjacencyList[v2].insert(v1);

            degreeOrder.insert({getDegree(v1), v1});
            degreeOrder.insert({getDegree(v2), v2});
        }
    }

    bool isAdjacent(int v1, int v2) const {
        return adjacencyList[v1].find(v2) != adjacencyList[v1].end();
    }

    const ordered_set<int> &getNeighbors(int v) const {
        return adjacencyList[v];
    }

    int getDegree(int v) const { return adjacencyList[v].size(); }

    // Obtiene los k vertices de mayor grado
    std::vector<int> getTopKByDegree(int k) const {
        std::vector<int> result;
        auto it = degreeOrder.rbegin();
        while (result.size() < k && it != degreeOrder.rend()) {
            result.push_back(it->second);
            ++it;
        }
        return result;
    }

    int getMaxDegree() const {
        if (degreeOrder.empty())
            return 0;
        return degreeOrder.rbegin()->first;
    }

    int getNumVertices() const { return numVertices; }

    // Constructor desde archivo
    static Graph fromFile(const string &filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        // Primera linea contiene numero de vertices y aristas
        int n, m;
        file >> n >> m;

        Graph g(n);

        // Lee las aristas
        for (int i = 0; i < m; i++) {
            int v1, v2;
            file >> v1 >> v2;
            g.addEdge(v1, v2);
        }

        return g;
    }
};
