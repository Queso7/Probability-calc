#ifndef COMBINATORICS_H
#define COMBINATORICS_H

#include <vector>
#define ll long long
#define vll std::vector<ll>

class Combinatorics {
    private:
        vll fact;
        vll invFact;
        ll mod;

        // Fast Exponentiation [Exponenciacion rápida]
        ll bin_pow(ll base, ll exp) {
            ll ans = 1;
            base %= mod;
            while(exp) {
                if(exp & 1) ans = (ans*base)%mod;
                base = (base*base)%mod;
                exp >>= 1;
            }
            return ans;
        }
        // Fermat's little Theorem [Pequeño Teorema de Fermat xd]
        ll modInverse(ll n) {
            return bin_pow(n, mod - 2);
        }

    public:
        // Precomp of factorial and inverses [Precomputo de factoriales e inversos]
        void init(int max_n, ll m) {
            mod = m;
            fact.resize(2*max_n + 1);
            invFact.resize(2*max_n + 1);

            fact[0] = 1;
            for(int i = 1; i <= 2*max_n; i++) fact[i] = (fact[i-1] * i) % mod;

            invFact[2*max_n] = modInverse(fact[2*max_n]);
            for(int i = 2*max_n - 1; i >= 0; i--) invFact[i] = (invFact[i+1] * (i + 1)) % mod;
        }

        // Combinations [Combinaciones]
        ll nCr(int n, int r) {
            if (r < 0 || r > n) return 0;
            return (((fact[n] * invFact[r]) % mod) * invFact[n - r]) % mod;
        }
        // Variations [Variaciones]
        ll nPr(int n, int r){
            if(r < 0 || r > n) return 0;
            return (fact[n]*invFact[n-r]) % mod;
        }
        // Combinations with Repetition [Combinaciones con Repetición]
        ll nCr_rep(int n, int r) {
            if (n == 0 && r == 0) return 1;
            if (n == 0) return 0;
            return nCr(n + r - 1, r);
        }

        // Variations with Repetition [Variaciones con Repetición]
        ll nPr_rep(int n, int r) {
            return bin_pow(n, r);
        }
};

#endif