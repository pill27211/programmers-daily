#include<bits/stdc++.h>
using namespace std;

using ll = long long;
using vi = vector <int>;
using vl = vector <ll>;

void dfs(vector<vi>& gr, vi& visit, vl& a, int cur, ll& res)
{
    for(int next : gr[cur])
        if(!visit[next])
        {
            visit[next] = 1;
            dfs(gr, visit, a, next, res);
            res += abs(a[next]);
            
            a[cur] += a[next];
            a[next] = 0;
        }
}
ll solution(vi a, vector<vi> edges) {
    int n(a.size());
    
    vl a_(n);
    for(int i{}; i < n; i++)
        a_[i] = a[i];
    
    vector <vi> gr(n);
    for(auto& edge : edges)
    {
        int u(edge[0]), v(edge[1]);
        gr[u].push_back(v);
        gr[v].push_back(u);
    }
    
    vi visit(n);
    visit[0] = 1;
    
    ll res(0);
    dfs(gr, visit, a_, 0, res);
    
    for(int i{}; i < n; i++)
        if(a_[i])
            return -1;
    
    return res;
}