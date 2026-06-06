#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <algorithm>

// ----------------- Maze Setup --------------------
const int N = 5;
int mazeArr[N][N] = {
    {0,1,0,0,0},
    {0,1,0,1,0},
    {0,0,0,1,0},
    {1,1,0,1,0},
    {0,0,0,0,0}
};

struct Cell { int r, c; };

// Directions
int dR[4] = {-1,1,0,0};
int dC[4] = {0,0,-1,1};

// ----------------- Backtracking --------------------
bool validBT(int r,int c, bool vis[N][N]) {
    return r>=0 && r<N && c>=0 && c<N && mazeArr[r][c]==0 && !vis[r][c];
}

bool runBT(int r, int c, bool vis[N][N], std::vector<Cell> &path, Cell goal)
{
    if (r == goal.r && c == goal.c) { path.push_back({r,c}); return true; }
    vis[r][c] = true;
    path.push_back({r,c});
    for (int i=0;i<4;i++){
        int nr=r+dR[i], nc=c+dC[i];
        if (validBT(nr,nc,vis) && runBT(nr,nc,vis,path,goal)) return true;
    }
    path.pop_back();
    return false;
}

// ----------------- Dijkstra --------------------
bool validD(int r,int c){ return r>=0 && r<N && c>=0 && c<N && mazeArr[r][c]==0; }

std::vector<Cell> runDijkstra(Cell start, Cell goal)
{
    std::vector<std::vector<int>> dist(N, std::vector<int>(N, 1e9));
    std::vector<std::vector<Cell>> parent(N, std::vector<Cell>(N, {-1,-1}));

    struct Node{ int r,c,d; };
    auto cmp = [](const Node &a, const Node &b){ return a.d > b.d; };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);

    dist[start.r][start.c] = 0;
    pq.push({start.r,start.c,0});

    while(!pq.empty()){
        auto [r,c,d] = pq.top(); pq.pop();
        if (r==goal.r && c==goal.c) break;
        for (int i=0;i<4;i++){
            int nr=r+dR[i], nc=c+dC[i];
            if(validD(nr,nc) && dist[nr][nc] > d+1){
                dist[nr][nc] = d+1;
                parent[nr][nc] = {r,c};
                pq.push({nr,nc,dist[nr][nc]});
            }
        }
    }

    std::vector<Cell> path;
    if (dist[goal.r][goal.c] >= 1e9) return path;
    Cell cur = goal;
    while (!(cur.r==-1 && cur.c==-1)){
        path.push_back(cur);
        cur = parent[cur.r][cur.c];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// ----------------- MAIN GUI --------------------
int main()
{
    const int CELL = 80;
    const unsigned W = unsigned(CELL*N), H = unsigned(CELL*N);
    sf::RenderWindow window(sf::VideoMode({W,H}), "Maze Solver GUI (SFML 3)");
    window.setFramerateLimit(60);

    Cell start{0,0}, goal{4,4};
    std::vector<Cell> path;  // path to draw

    while(window.isOpen())
    {
        while (auto ev = window.pollEvent())
        {
            const sf::Event &e = *ev;

            if (e.is<sf::Event::Closed>()) { window.close(); }

            if (auto *k = e.getIf<sf::Event::KeyPressed>()){
                if (k->code == sf::Keyboard::Key::B) {
                    bool vis[N][N] = {false};
                    path.clear();
                    runBT(start.r,start.c,vis,path,goal);
                } else if (k->code == sf::Keyboard::Key::D) {
                    path = runDijkstra(start,goal);
                } else if (k->code == sf::Keyboard::Key::R) {
                    path.clear();
                }
            }
        }

        // ----------- DRAW -------------
        window.clear(sf::Color::Black);

        sf::RectangleShape cell(sf::Vector2f(float(CELL-2), float(CELL-2)));
        for(int r=0;r<N;r++){
            for(int c=0;c<N;c++){
                cell.setFillColor(mazeArr[r][c]==1 ? sf::Color(70,70,70) : sf::Color(200,200,200));
                cell.setPosition(sf::Vector2f(float(c*CELL+1), float(r*CELL+1)));
                window.draw(cell);
            }
        }

        sf::RectangleShape pathCell(sf::Vector2f(float(CELL-10), float(CELL-10)));
        pathCell.setFillColor(sf::Color::Green);
        for (auto &p : path) {
            pathCell.setPosition(sf::Vector2f(float(p.c*CELL+5), float(p.r*CELL+5)));
            window.draw(pathCell);
        }

        sf::CircleShape startC(float(CELL)/3.f);
        startC.setFillColor(sf::Color::Blue);
        startC.setPosition(sf::Vector2f(float(start.c*CELL+CELL/6), float(start.r*CELL+CELL/6)));
        window.draw(startC);

        sf::CircleShape endC(float(CELL)/3.f);
        endC.setFillColor(sf::Color::Red);
        endC.setPosition(sf::Vector2f(float(goal.c*CELL+CELL/6), float(goal.r*CELL+CELL/6)));
        window.draw(endC);

        window.display();
    }
    return 0;
}
