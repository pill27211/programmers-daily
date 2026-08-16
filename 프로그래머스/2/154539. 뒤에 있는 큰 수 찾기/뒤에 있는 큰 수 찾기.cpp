#include<bits/stdc++.h>
using namespace std;

vector<int> solution(vector<int> numbers) {
    int n(numbers.size());
    
    vector <int> ans(n, -1), st;
    for(int i(n - 1); ~i; i--)
    {
        while(st.size() && st.back() <= numbers[i])
            st.pop_back();
        
        if(st.size()) ans[i] = st.back();
        st.push_back(numbers[i]);
    }

    return ans;
}