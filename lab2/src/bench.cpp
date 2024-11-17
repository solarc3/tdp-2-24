#include <bits/stdc++.h>
#include <chrono>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace std::chrono;
using namespace __gnu_pbds;

// Estructura para representar un grafo de prueba
struct TestGraph {
    string name;
    int V;
    int E;
    TestGraph(string n, int v, int e) : name(n), V(v), E(e) {}
};

// Grafo con vector (baseline)
class VectorGraph {
    private:
    int V;
    vector<vector<int>> adj;

    public:
    VectorGraph(int v) : V(v) { adj.resize(V); }
    void addEdge(int v, int w) { adj[v].push_back(w); }
    bool hasEdge(int v, int w) {
        return find(adj[v].begin(), adj[v].end(), w) != adj[v].end();
    }
};

// Grafo con gp_hash_table (baseline)
class GPHashGraph {
    private:
    int V;
    vector<gp_hash_table<int, null_type>> adj;

    public:
    GPHashGraph(int v) : V(v) { adj.resize(V); }
    void addEdge(int v, int w) { adj[v].insert(w); }
    bool hasEdge(int v, int w) { return adj[v].find(w) != adj[v].end(); }
};

// Nueva implementación: Grafo con cc_hash_table
class CCHashGraph {
    private:
    int V;
    vector<cc_hash_table<int, null_type>> adj;

    public:
    CCHashGraph(int v) : V(v) { adj.resize(V); }
    void addEdge(int v, int w) { adj[v].insert(w); }
    bool hasEdge(int v, int w) { return adj[v].find(w) != adj[v].end(); }
};

// Nueva implementación: Grafo con tree
class TreeGraph {
    private:
    int V;
    // Usar tree con política de árbol rb_tree_tag y sin datos adicionales
    typedef tree<int, null_type, less<int>, rb_tree_tag,
                 tree_order_statistics_node_update>
        ordered_set;
    vector<ordered_set> adj;

    public:
    TreeGraph(int v) : V(v) { adj.resize(V); }
    void addEdge(int v, int w) { adj[v].insert(w); }
    bool hasEdge(int v, int w) { return adj[v].find(w) != adj[v].end(); }
};

// Nueva implementación: Grafo híbrido (tree para vertices con muchas aristas,
// hash para el resto)
class HybridGraph {
    private:
    int V;
    static const int DENSITY_THRESHOLD = 50; // Umbral para cambiar a tree
    typedef tree<int, null_type, less<int>, rb_tree_tag,
                 tree_order_statistics_node_update>
        ordered_set;
    vector<pair<bool, void *>> adj; // bool: is_tree

    public:
    HybridGraph(int v) : V(v) {
        adj.resize(V, {false, nullptr});
        // Inicialmente todos son hash_tables
        for (int i = 0; i < V; i++) {
            adj[i].second = new gp_hash_table<int, null_type>();
        }
    }

    ~HybridGraph() {
        for (int i = 0; i < V; i++) {
            if (adj[i].first) {
                delete static_cast<ordered_set *>(adj[i].second);
            } else {
                delete static_cast<gp_hash_table<int, null_type> *>(
                    adj[i].second);
            }
        }
    }

    void addEdge(int v, int w) {
        if (adj[v].first) {
            auto tree = static_cast<ordered_set *>(adj[v].second);
            tree->insert(w);
            if (tree->size() < DENSITY_THRESHOLD) {
                // Convertir de vuelta a hash si el tamaño baja del umbral
                auto hash = new gp_hash_table<int, null_type>();
                for (int x : *tree) {
                    hash->insert(x);
                }
                delete tree;
                adj[v] = {false, hash};
            }
        } else {
            auto hash =
                static_cast<gp_hash_table<int, null_type> *>(adj[v].second);
            hash->insert(w);
            if (hash->size() >= DENSITY_THRESHOLD) {
                // Convertir a tree si el tamaño supera el umbral
                auto tree = new ordered_set();
                for (auto x : *hash) {
                    tree->insert(x);
                }
                delete hash;
                adj[v] = {true, tree};
            }
        }
    }

    bool hasEdge(int v, int w) {
        if (adj[v].first) {
            auto tree = static_cast<ordered_set *>(adj[v].second);
            return tree->find(w) != tree->end();
        } else {
            auto hash =
                static_cast<gp_hash_table<int, null_type> *>(adj[v].second);
            return hash->find(w) != hash->end();
        }
    }
};

template <typename Func> long long measureTime(Func func) {
    auto start = high_resolution_clock::now();
    func();
    auto stop = high_resolution_clock::now();
    return duration_cast<microseconds>(stop - start).count();
}

void runBenchmark(const TestGraph &graph) {
    vector<pair<int, int>> edges;
    vector<pair<int, int>> queries;

    mt19937 gen(random_device{}());
    uniform_int_distribution<> dis(0, graph.V - 1);

    for (int i = 0; i < graph.E; i++) {
        edges.push_back({dis(gen), dis(gen)});
    }

    int Q = min(5000000, 5 * graph.E);
    for (int i = 0; i < Q; i++) {
        queries.push_back({dis(gen), dis(gen)});
    }

    // Vector baseline
    VectorGraph vg(graph.V);
    auto vector_insert = measureTime([&]() {
        for (auto [v, w] : edges)
            vg.addEdge(v, w);
    });

    auto vector_query = measureTime([&]() {
        for (auto [v, w] : queries)
            vg.hasEdge(v, w);
    });

    // GP Hash baseline
    GPHashGraph gh(graph.V);
    auto gp_hash_insert = measureTime([&]() {
        for (auto [v, w] : edges)
            gh.addEdge(v, w);
    });

    auto gp_hash_query = measureTime([&]() {
        for (auto [v, w] : queries)
            gh.hasEdge(v, w);
    });

    // CC Hash
    CCHashGraph ch(graph.V);
    auto cc_hash_insert = measureTime([&]() {
        for (auto [v, w] : edges)
            ch.addEdge(v, w);
    });

    auto cc_hash_query = measureTime([&]() {
        for (auto [v, w] : queries)
            ch.hasEdge(v, w);
    });

    // Tree
    TreeGraph tg(graph.V);
    auto tree_insert = measureTime([&]() {
        for (auto [v, w] : edges)
            tg.addEdge(v, w);
    });

    auto tree_query = measureTime([&]() {
        for (auto [v, w] : queries)
            tg.hasEdge(v, w);
    });

    // Hybrid
    HybridGraph hg(graph.V);
    auto hybrid_insert = measureTime([&]() {
        for (auto [v, w] : edges)
            hg.addEdge(v, w);
    });

    auto hybrid_query = measureTime([&]() {
        for (auto [v, w] : queries)
            hg.hasEdge(v, w);
    });

    double edges_per_vertex = (double)graph.E / graph.V;

    cout << "\nGrafo: " << graph.name << endl;
    cout << "Vértices: " << graph.V << ", Aristas: " << graph.E << endl;
    cout << "Aristas/vértice: " << fixed << setprecision(2) << edges_per_vertex
         << endl;
    cout << "Consultas realizadas: " << Q << endl;

    // Mostrar resultados de todas las implementaciones
    vector<pair<string, long long>> results = {
        {"Vector<vector<int>>", vector_insert + vector_query  },
        {"gp_hash_table",       gp_hash_insert + gp_hash_query},
        {"cc_hash_table",       cc_hash_insert + cc_hash_query},
        {"tree",                tree_insert + tree_query      },
        {"hybrid",              hybrid_insert + hybrid_query  }
    };

    // Mostrar tiempos detallados para cada implementación
    cout << "\nVector<vector<int>>:" << endl;
    cout << "Inserción: " << vector_insert << " μs" << endl;
    cout << "Consulta:  " << vector_query << " μs" << endl;
    cout << "Total:     " << vector_insert + vector_query << " μs" << endl;

    cout << "\ngp_hash_table:" << endl;
    cout << "Inserción: " << gp_hash_insert << " μs" << endl;
    cout << "Consulta:  " << gp_hash_query << " μs" << endl;
    cout << "Total:     " << gp_hash_insert + gp_hash_query << " μs" << endl;

    cout << "\ncc_hash_table:" << endl;
    cout << "Inserción: " << cc_hash_insert << " μs" << endl;
    cout << "Consulta:  " << cc_hash_query << " μs" << endl;
    cout << "Total:     " << cc_hash_insert + cc_hash_query << " μs" << endl;

    cout << "\ntree:" << endl;
    cout << "Inserción: " << tree_insert << " μs" << endl;
    cout << "Consulta:  " << tree_query << " μs" << endl;
    cout << "Total:     " << tree_insert + tree_query << " μs" << endl;

    cout << "\nhybrid:" << endl;
    cout << "Inserción: " << hybrid_insert << " μs" << endl;
    cout << "Consulta:  " << hybrid_query << " μs" << endl;
    cout << "Total:     " << hybrid_insert + hybrid_query << " μs" << endl;

    // Encontrar el ganador
    auto winner = min_element(
        results.begin(), results.end(),
        [](const auto &a, const auto &b) { return a.second < b.second; });

    cout << "\nGANADOR: " << winner->first << " (";
    auto slowest = max_element(
        results.begin(), results.end(),
        [](const auto &a, const auto &b) { return a.second < b.second; });
    cout << fixed << setprecision(2) << (double)slowest->second / winner->second
         << "x más rápido)" << endl;
    cout << "=======================" << endl;
}

int main() {
    vector<TestGraph> graphs = {
        // Grafos sparse
        TestGraph("myciel3.col", 11, 20),    // muy sparse
        TestGraph("queen5_5.col", 25, 160),  // sparse
        TestGraph("miles250.col", 128, 387), // sparse

        // Grafos de densidad media
        TestGraph("le450_5a.col", 450, 5714),     // medio-sparse
        TestGraph("school1_nsh.col", 352, 14612), // medio
        TestGraph("le450_25c.col", 450, 17343),   // medio

        // Grafos densos
        TestGraph("DSJC500.5.col.b", 500, 125249),   // denso
        TestGraph("DSJR500.1c.col.b", 500, 242550),  // muy denso
        TestGraph("DSJC1000.9.col.b", 1000, 1200000) // extremadamente denso
    };

    for (const auto &graph : graphs) {
        runBenchmark(graph);
    }

    return 0;
}
