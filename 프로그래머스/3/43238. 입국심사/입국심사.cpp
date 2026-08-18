#include<bits/stdc++.h>
using namespace std;

using ll = long long;

ll solution(int n, vector<int> times) {
    ll l(0), r(1e18);
    while(l <= r)
    {
        ll m(l + r >> 1), n_(n);
        
        for(ll i{}; i < times.size(); i++)
            n_ -= m / times[i];
            
        if(n_ > 0) l = m + 1;
        else r = m - 1;
    }
    
    return l;
}