#include<bits/stdc++.h>
using namespace std;

int solution(int distance, vector<int> rocks, int n) {
    rocks.insert(rocks.end(), {0, distance, (int)2e9}); // 일반성 유지
    sort(rocks.begin(), rocks.end());
    
    int l(1), r(1e9), len(rocks.size());
    while(l <= r)
    {
        int m(l + r >> 1), cnt{};
        for(int i{}; i < len - 1; i++)
        {
            int cur(rocks[i + 1] - rocks[i]);
            while(cur < m && i < len - 1)
            {
                i++;
                cur += rocks[i + 1] - rocks[i];
                cnt++;
            }
        }
        
        if(cnt > n) r = m - 1;
        else l = m + 1;
    }
    
    return r;
}