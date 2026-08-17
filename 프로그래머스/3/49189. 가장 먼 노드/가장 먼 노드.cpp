#include<bits/stdc++.h>
using namespace std;

int solution(int n, vector<vector<int>> edge) {
    vector <vector<int>> gr(n + 1);
    for(auto eg : edge)
    {
        gr[eg[0]].push_back(eg[1]);
        gr[eg[1]].push_back(eg[0]);
    }
    
    vector <int> vis(n + 1);
    queue <int> q;
    vis[1] = 1;
    
    int ans(0), max_v(0);
    for(q.push(1); q.size();)
    {
        int now(q.front()); q.pop();
        
        if(vis[now] >= max_v)
        {
            if(vis[now] > max_v)
            {
                max_v = vis[now];
                ans = 0;
            }
            
            ans++;
        }
        
        for(int nxt : gr[now])
            if(!vis[nxt])
            {
                vis[nxt] = vis[now] + 1;
                q.push(nxt);
            }
    }
    
    return ans;
}