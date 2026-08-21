#include<bits/stdc++.h>
using namespace std;

string solution(vector<int> numbers) {
    vector <string> arr;
    for(int i : numbers) arr.push_back(to_string(i));
    
    sort(arr.begin(), arr.end(), [](auto& a, auto& b) { return a + b > b + a; });
    
    string ans;
    for(auto& i : arr) ans += i;
    
    return ans[0] > 48 ? ans : "0";
}