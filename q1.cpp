// speculative_pathfinding.cpp
// Compile: g++ -fopenmp -std=c++17 speculative_pathfinding.cpp -o spec_path
// Run: ./spec_path

#include <bits/stdc++.h>
#include <omp.h>
#include <atomic>
using namespace std;

// ---------------- Basic Structures ----------------
struct Edge { int to; double w; };
struct Node { double x, y; }; // only for heuristic

struct Graph {
    int n;
    vector<Node> nodes;
    vector<vector<Edge>> adj;
    Graph(int n): n(n), nodes(n), adj(n) {}
    void add_edge(int u, int v, double w) {
        adj[u].push_back((Edge){v, w});
        adj[v].push_back((Edge){u, w});
    }
};

// Euclidean distance (for A*)
double heuristic(const Node &a, const Node &b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

// ---------------- Dijkstra ----------------
double dijkstra(const Graph &G, int s, int t, atomic<bool> *done=nullptr) {
    vector<double> dist(G.n, 1e9);
    dist[s] = 0;
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;
    pq.push(make_pair(0, s));

    while(!pq.empty()) {
        if (done && done->load()) return -1; // stop if other finished
        pair<double,int> top = pq.top(); pq.pop();
        double d = top.first;
        int u = top.second;
        if (u == t) return d;
        for (size_t i = 0; i < G.adj[u].size(); i++) {
            Edge e = G.adj[u][i];
            if (dist[e.to] > d + e.w) {
                dist[e.to] = d + e.w;
                pq.push(make_pair(dist[e.to], e.to));
            }
        }
    }
    return -1;
}

// ---------------- A* Search ----------------
double astar(const Graph &G, int s, int t, atomic<bool> *done=nullptr) {
    vector<double> g(G.n, 1e9);
    g[s] = 0;
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;
    pq.push(make_pair(heuristic(G.nodes[s], G.nodes[t]), s));

    while(!pq.empty()) {
        if (done && done->load()) return -1;
        pair<double,int> top = pq.top(); pq.pop();
        double f = top.first;
        int u = top.second;
        if (u == t) return g[u];
        for (size_t i = 0; i < G.adj[u].size(); i++) {
            Edge e = G.adj[u][i];
            double ng = g[u] + e.w;
            if (ng < g[e.to]) {
                g[e.to] = ng;
                double fval = ng + heuristic(G.nodes[e.to], G.nodes[t]);
                pq.push(make_pair(fval, e.to));
            }
        }
    }
    return -1;
}

// ---------------- Main ----------------
int main() {
    int n = 5;
    Graph G(n);
    srand(time(0));

    // Fixed node initialization to avoid narrowing warnings
    for (int i = 0; i < n; i++) {
        G.nodes[i].x = static_cast<double>(rand() % 10);
        G.nodes[i].y = static_cast<double>(rand() % 10);
    }

    // Sample graph
    G.add_edge(0,1,2); G.add_edge(1,2,2); G.add_edge(0,3,4);
    G.add_edge(3,4,1); G.add_edge(2,4,3);

    int s = 0, t = 4;

    cout << "Source: " << s << ", Target: " << t << "\n";

    // Sequential Dijkstra
    double t1 = omp_get_wtime();
    double d1 = dijkstra(G, s, t);
    double t2 = omp_get_wtime();
    cout << "Dijkstra (sequential): cost=" << d1 << " time=" << (t2 - t1) << "s\n";

    // Sequential A*
    double t3 = omp_get_wtime();
    double d2 = astar(G, s, t);
    double t4 = omp_get_wtime();
    cout << "A* (sequential): cost=" << d2 << " time=" << (t4 - t3) << "s\n";

    // Speculative: run both in parallel
    cout << "\nRunning speculative pathfinding...\n";
    atomic<bool> done(false);
    double d_dij = -1, d_ast = -1;

    double start = omp_get_wtime();
    #pragma omp parallel sections
    {
        #pragma omp section
        { d_dij = dijkstra(G, s, t, &done); done = true; }

        #pragma omp section
        { d_ast = astar(G, s, t, &done); done = true; }
    }
    double end = omp_get_wtime();

    cout << "Speculative run finished in " << (end - start) << "s\n";
    if (d_dij > 0 && (d_ast < 0 || d_dij <= d_ast))
        cout << "Winner: Dijkstra (cost=" << d_dij << ")\n";
    else if (d_ast > 0)
        cout << "Winner: A* (cost=" << d_ast << ")\n";
    else
        cout << "No path found.\n";

    return 0;
}

// g++ -fopenmp -std=c++17 q1.cpp -o q1.exe