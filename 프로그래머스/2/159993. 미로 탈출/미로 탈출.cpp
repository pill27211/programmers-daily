#include<bits/stdc++.h>
using namespace std;

struct P{ int y, x; };

int bfs(auto& maps, int n, int m, P st, P ed)
{
    int dy[] = {-1, 1, 0, 0}, dx[] = {0, 0, -1, 1};
    
    vector <vector <int>> visits(n, vector<int>(m));
    queue <pair<P, int>> Q;
    visits[st.y][st.x] = 1;
    Q.push({st, 0});
    
    while(Q.size())
    {
        auto e(Q.front()); Q.pop();
        int cur_y = e.first.y;
        int cur_x = e.first.x;
        
        if(maps[cur_y][cur_x] == maps[ed.y][ed.x])
            return e.second;
        
        for(int i{}; i < 4; i++)
        {
            int ny = cur_y + dy[i];
            int nx = cur_x + dx[i];
                
            if(ny == n || ny < 0 || nx == m || nx < 0)
                continue;
            
            if(maps[ny][nx] != 'X' && !visits[ny][nx])
            {
                visits[ny][nx] = 1;
                Q.push({P{ny, nx}, e.second + 1});
            }
        }
    }
    return -1e9;
}
int solution(vector<string> maps) {
    int n(maps.size()), m(maps[0].size());
    
    P S{0, 0}, E{0, 0}, L{0, 0};
    
    for(int i{}; i < n; i++)
        for(int j{}; j < m; j++)
        {
            if(maps[i][j] == 'S') S = {i, j};
            else if(maps[i][j] == 'E') E = {i, j};
            else if(maps[i][j] == 'L') L = {i, j};
        }
    
    int ans = bfs(maps, n, m, S, L) + bfs(maps, n, m, L, E);
    
    return max(-1, ans);
}