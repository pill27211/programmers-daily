#include<bits/stdc++.h>
using namespace std;

int f(char ch) { return  ch >= 97 && ch <= 122; }
map <string, int> g(string str)
{
    for(char& ch : str)
        ch += (ch >= 65 && ch <= 90) * 32;
    
    map <string, int> m;
    for(int i{}; i + 1 < str.size(); i++)
        if(f(str[i]) && f(str[i + 1]))
            m[str.substr(i, 2)]++;
    
    return m;
}
int solution(string str1, string str2) {
    map <string, int> m1(g(str1)), m2(g(str2)), m3;
    
    if(!m1.size() && !m2.size()) return 65536;
    
    int v1{}, v2{};
    for(char i('a'); i <= 'z'; i++)
        for(char j('a'); j <= 'z'; j++)
        {
            string cur({i, j});
            
            auto [min_, max_](minmax(m1[cur], m2[cur]));
            v1 += min_;
            v2 += max_;
        }
        
    return (int)((double)v1 * 65536 / v2);
}