#include<bits/stdc++.h>
using namespace std;

int solution(int n, int s, int a, int b, vector<vector<int>> fares) {
    vector <vector<int>> dp(n + 1, vector<int>(n + 1, 0x3f3f3f));
    for(int i(1); i <= n; i++) dp[i][i] = 0;
    
    for(auto eg : fares)
        dp[eg[0]][eg[1]] = dp[eg[1]][eg[0]] = eg[2];

    
    for(int k(1); k <= n; k++)
        for(int i(1); i <= n; i++)
            for(int j(1); j <= n; j++)
                dp[i][j] = min(dp[i][j], dp[i][k] + dp[k][j]);
    
    int ans(1e9);
    for(int i(1); i <= n; i++)
        ans = min({ans, dp[s][a] + dp[s][b], dp[s][i] + dp[i][a] + dp[i][b]});
    
    return ans;
}