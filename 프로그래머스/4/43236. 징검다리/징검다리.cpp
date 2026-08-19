#include<bits/stdc++.h>
using namespace std;

int solution(int distance, vector<int> rocks, int n) {
    rocks.push_back(0);
    rocks.push_back(distance);
    sort(rocks.begin(), rocks.end());
    
    int l(1), r(1e9), len(rocks.size());
    rocks.push_back(2e9); // 일반성 유지
    while(l <= r)
    {
        int m(l + r >> 1), cnt{}, flag{};
        for(int i{}; i < len - 1;)
        {
            int cur(rocks[i + 1] - rocks[i]);
            if(cur >= m) i++;
            else
            {
                while(cur < m && i < len - 1)
                {
                    i++;
                    cur += rocks[i + 1] - rocks[i];
                    cnt++;
                }
                
                flag += cur < m;
                i++;
            }
        }
        
        if(cnt > n || flag) r = m - 1;
        else l = m + 1;
    }
    
    return r;
}