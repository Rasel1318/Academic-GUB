#include <bits/stdc++.h>

using namespace std;
using ll = long long int;
using lld = long double;
using pii = pair<int,int>;
using vi = vector<int>;
using vl = vector<ll>;
using vii = vector<pii>;

#define fastio() ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL)
#define mod 1000000007
#define INF 1e18
#define endl "\n"
#define pb push_back
#define ppb pop_back
#define mp make_pair
#define rep(i, j, n) for(int i=(j);i<(n);++i)
#define ff first
#define ss second
#define PI 3.141592653589793238462
#define set_bits __builtin_popcountll
#define all(x) (x).begin(), (x).end()
#define sz(x) ((int)(x).size())
#define no cout<<"NO\n"
#define yes cout<<"YES\n"


/*---------------------------------------------------------------------------------------------------------------------------*/
ll gcd(ll a, ll b) {if (b > a) {return gcd(b, a);} if (b == 0) {return a;} return gcd(b, a % b);}
ll expo(ll a, ll b, ll m=mod) {ll res = 1; while(b > 0) {if(b&1) res = (res*a)%m; a = (a*a)%m; b = b>>1;} return res;}
ll mminvprime(ll a, ll b) {return expo(a, b - 2, b);}
ll mod_add(ll a, ll b, ll m=mod) {a = a % m; b = b % m; return (((a + b) % m) + m) % m;}
ll mod_mul(ll a, ll b, ll m=mod) {a = a % m; b = b % m; return (((a * b) % m) + m) % m;}
ll mod_sub(ll a, ll b, ll m=mod) {a = a % m; b = b % m; return (((a - b) % m) + m) % m;}
ll mod_div(ll a, ll b, ll m=mod) {a = a % m; b = b % m; return (mod_mul(a, mminvprime(b, m), m) + m) % m;}  //only for prime m
/*---------------------------------------------------------------------------------------------------------------------------*/

void solve(){
    ll n;
    cin >> n;

    vl a(n), b;
    for(auto &i:a) {cin>>i; b.pb(i);}

    ll total = 0;
    reverse(all(b));

    vl preM(n, INT_MAX);
    preM[0] = b[0];
    for(int i = 1; i<n; i++){
        preM[i] = min(b[i], preM[i-1]);
    }
    for(int i = n-1; i>=0; i--){
        total += b[i]-preM[i];
    }

    

}

int32_t main(){
    fastio();
    // freopen("txt.in", "r", stdin);
    // freopen("txt.out", "w", stdout);
    // cout<<fixed<<std::setprecision(10);
    
    int _ = 1;
    cin >> _;
    while(_--){
        solve();
    }
    return 0;
}

