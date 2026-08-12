#include<bits/stdc++.h>
using namespace std;

int solution(vector<vector<int>> scores) {
    int n(scores.size()), a(scores[0][0]), b(scores[0][1]);
    
    sort(scores.begin(), scores.end());
    
    vector <int> flag(n);
    set <int> s;
    for(int i(n - 1); ~i;)
    {
        vector <int> v;
        do
        {
            auto it(s.lower_bound(scores[i][1] + 1));
            flag[i] = it != s.end();
            v.push_back(scores[i--][1]);
        } while(~i && scores[i][0] == scores[i + 1][0]);
        
        for(int x : v) s.insert(x);
    }
    
    int res(1);
    for(int i{}; i < n; i++)
    {
        if(a < scores[i][0] && b < scores[i][1])
            return -1;
        
        if(!flag[i])
            res += a + b < scores[i][0] + scores[i][1];
    }
    
    return res;
}