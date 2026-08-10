#include<bits/stdc++.h>
using namespace std;

int solution(int n, vector<vector<int>> q, vector<int> ans) {
    vector <int> sel(n);
    fill(sel.end() - 5, sel.end(), 1);

    int res{};
    do
    {
        int flag(1), i{};
        for(auto& cur : q)
        {
            int cnt{};
            for(int j{}; j < 5; j++)
                cnt += sel[cur[j] - 1];
            
            flag -= cnt ^ ans[i++];
        }
        
        res += flag == 1;
    } while(next_permutation(sel.begin(), sel.end()));

    return res;
}