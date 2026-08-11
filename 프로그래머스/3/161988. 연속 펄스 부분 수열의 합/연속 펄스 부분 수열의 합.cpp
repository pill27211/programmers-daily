#include <string>
#include <vector>
using ll = long long;
using namespace std;

ll f(vector <int> arr, int n)
{
    vector <vector<ll>> dp(n, vector <ll>(2, -1e18));
    dp[0][1] = arr[0];
    
    for(int i(1); i < n; i++)
    {
        dp[i][0] = max(dp[i - 1][0], dp[i - 1][1]);
        dp[i][1] = max(dp[i - 1][1] + arr[i], (ll)arr[i]);
    }
    return max(dp[n - 1][0], dp[n - 1][1]);
}
ll solution(vector<int> sequence) {
    auto sub1(sequence), sub2(sequence);
    int n(sub1.size());
    
    for(int i{}; i < n; i++)
    {
        sub1[i] *= i & 1 ? 1 : -1;
        sub2[i] *= i & 1 ? -1 : 1;
    }
    
    return max(f(sub1, n), f(sub2, n));
}