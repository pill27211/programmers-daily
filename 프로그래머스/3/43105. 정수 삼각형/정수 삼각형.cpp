#include<bits/stdc++.h>
using namespace std;

int solution(vector<vector<int>> triangle) {
    int h(triangle.size());
    
    vector <vector <int>> dp(505, vector<int>(505));
    for(int i(1); i<= h; i++)
        for(int j(1); j <= i; j++)
            dp[i][j] += max(dp[i - 1][j - 1], dp[i - 1][j]) + triangle[i - 1][j - 1];
                           
    return *max_element(dp[h].begin(), dp[h].end());
}