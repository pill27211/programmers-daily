#include<bits/stdc++.h>
using namespace std;

vector<int> solution(vector<int> sequence, int k) {
    int l(0), r(0), sum(sequence[0]);

    int ans_l(0), ans_r(1e9);
    while(l <= r)
    {
        if(r == sequence.size()) break;
        
        if(sum == k && r - l < ans_r - ans_l)
        {
            ans_l = l;
            ans_r = r;
        }

        if(sum >= k) sum -= sequence[l++];
        else sum += sequence[++r];
    }

    return vector <int>({ans_l, ans_r});
}