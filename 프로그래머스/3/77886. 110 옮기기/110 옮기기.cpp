#include<bits/stdc++.h>
using namespace std;

vector<string> solution(vector<string> s) {
    vector<string> answer;
    
    for(auto str : s)
    {
        string res;
        int token(0);
        for(auto ch : str)
        {
            res += ch;
            if(res.size() > 2 && res.substr(res.size() - 3) == "110")
            {
                res.erase(res.length() - 3); 
                token++;
            }
        }
        
        string temp;
        for(int i{}; i < token; i++) temp += "110";
        
        for(int i(1); i < res.size(); i++)
            if(res[i] + res[i - 1] == 98)
            {
                token = 0;
                res = res.substr(0, i - 1) + temp + res.substr(i - 1);
                break;
            }
        
        for(int i(res.size() - 1); i > -1; i--)
            if(res[i] == '0' && token > 0)
            {
                token = 0;
                res = res.substr(0, i + 1) + temp + res.substr(i + 1);
                break;
            }
        
        if(token > 0) res = temp + res;
        
        answer.push_back(res);
    }
    return answer;
}