#include "item_functions.h"
// The majority of these functions are modification of code from the 
// mirt package, version 1.7.1. December 22, 2014

double antilogit(const double *x){
    double ret;
    if(*x > 998.0) ret = 1.0;
    else if(*x < -998.0) ret = 0.0;
    else ret = 1.0 / (1.0 + exp(-1.0 * (*x)));
    return(ret);
}

double vecsum(const vector<double> &x)
{
    double sum = 0.0;
    const int size = x.size();
    for(int i = 0; i < size; ++i)
        sum += x[i];
    return(sum);
}

SEXP vec2mat(vector<double> &x, const int &nrow, const int &ncol) {
  NumericVector output = wrap(x);
  output.attr("dim") = Dimension(nrow, ncol);
  return(output);
}

void itemTrace(vector<double> &P, vector<double> &Pstar, const vector<double> &a, const double *d,
        const NumericMatrix &Theta, const double *g, const double *u)
{
    const int N = Theta.nrow();
    const int nfact = Theta.ncol();

    if((*u - *g) > 0){
        for (int i = 0; i < N; ++i){
            double z = *d;
            for (int j = 0; j < nfact; ++j)
                z += a[j] * Theta(i,j);
            if(z > ABS_MAX_Z) z = ABS_MAX_Z;
            else if(z < -ABS_MAX_Z) z = -ABS_MAX_Z;
            Pstar[i] = 1.0 / (1.0 + exp(-z));
            P[i] = *g + (*u - *g) * Pstar[i];
        }
    }
}

void P_dich(vector<double> &P, const vector<double> &par, const NumericMatrix &Theta,
    const int &N, const int &nfact)
{
    const int len = par.size();
    const double utmp = par[len-1];
    const double gtmp = par[len-2];
    const double g = antilogit(&gtmp);
    const double u = antilogit(&utmp);
    const double d = par[len-3];

    if((u - g) > 0){
        for (int i = 0; i < N; ++i){
            double z = d;
            for (int j = 0; j < nfact; ++j)
                z += par[j] * Theta(i,j);
            if(z > ABS_MAX_Z) z = ABS_MAX_Z;
            else if(z < -ABS_MAX_Z) z = -ABS_MAX_Z;
            P[i+N] = g + (u - g) /(1.0 + exp(-z));
            P[i] = 1.0 - P[i + N];
        }
    }
}

void P_graded(vector<double> &P, const vector<double> &par,
    const NumericMatrix &Theta, const int &N,
    const int &nfact, const int &nint, const int &itemexp, const int &israting)
{
    const int parsize = par.size();
    vector<double> a(nfact);
    for(int i = 0; i < nfact; ++i) a[i] = par[i];
    vector<double> d(nint, 0.0);
    if(israting){
        const double t = par[parsize-1];
        for(int i = nfact; i < parsize - 1; ++i)
            d[i - nfact] = par[i] + t;
    } else {
        for(int i = nfact; i < parsize; ++i)
            d[i - nfact] = par[i];
    }
    int notordered = 0;
    for(int i = 1; i < nint; ++i)
        notordered += d[i-1] <= d[i];
    if(notordered){
        int P_size = P.size();
        for(int i = 0; i < P_size; ++i)
            P[i] = 0.0;
    } else {
        const double nullzero = 0.0, nullone = 1.0;
        NumericMatrix Pk(N, nint + 2);

        for(int i = 0; i < N; ++i)
            Pk(i,0) = 1.0;
        for(int i = 0; i < nint; ++i){
            vector<double> tmp1(N), tmp2(N);
            itemTrace(tmp1, tmp2, a, &d[i], Theta, &nullzero, &nullone);
            for(int j = 0; j < N; ++j)
                Pk(j,i+1) = tmp2[j];
        }
        if(itemexp){
            int which = N * (nint + 1) - 1;
            for(int i = (Pk.ncol()-2); i >= 0; --i){
                for(int j = (N-1); j >= 0; --j){
                    P[which] = Pk(j,i) - Pk(j,i+1);
                    if(P[which] < 1e-20) P[which] = 1e-20;
                    else if((1.0 - P[which]) < 1e-20) P[which] = 1.0 - 1e-20;
                    --which;
                }
            }
        } else {
            int which = 0;
            for(int i = 0; i < Pk.ncol(); ++i){
                for(int j = 0; j < Pk.nrow(); ++j){
                    P[which] = Pk(j,i);
                    ++which;
                }
            }
        }
    }
}

void P_nominal(vector<double> &P, const vector<double> &par,
    const NumericMatrix &Theta, const int &N,
    const int &nfact, const int &ncat, const int &returnNum,
    const int &israting)
{
    vector<double> a(nfact), ak(ncat), d(ncat);
    for(int i = 0; i < nfact; ++i)
        a[i] = par[i];
    for(int i = 0; i < ncat; ++i){
        ak[i] = par[i + nfact];
        if(israting){
            if(i)
                d[i] = par[i + nfact + ncat] + par[par.size()-1];
        } else {
            d[i] = par[i + nfact + ncat];
        }
    }
    NumericMatrix Num(N, ncat);
    vector<double> z(ncat);
    vector<double> Den(N, 0.0);
    vector<double> innerprod(N, 0.0);

    for(int i = 0; i < N; ++i)
        for(int j = 0; j < nfact; ++j)
            innerprod[i] += Theta(i,j) * a[j];
    for(int i = 0; i < N; ++i){
        for(int j = 0; j < ncat; ++j)
            z[j] = ak[j] * innerprod[i] + d[j];
        double maxz = *std::max_element(z.begin(), z.end());
        for(int j = 0; j < ncat; ++j){
            z[j] = z[j] - maxz;
            if(z[j] < -ABS_MAX_Z) z[j] = -ABS_MAX_Z;
            Num(i,j) = exp(z[j]);
            Den[i] += Num(i,j);
        }
    }
    int which = 0;
    if(returnNum){
        for(int j = 0; j < ncat; ++j){
            for(int i = 0; i < N; ++i){
                P[which] = Num(i,j);
                ++which;
            }
        }
    } else {
        for(int j = 0; j < ncat; ++j){
            for(int i = 0; i < N; ++i){
                P[which] = Num(i,j) / Den[i];
                ++which;
            }
        }
    }
}

void P_nested(vector<double> &P, const vector<double> &par,
    const NumericMatrix &Theta, const int &N, const int &nfact, const int &ncat,
    const int &correct)
{
	const int par_size = par.size();
    vector<double> dpar(nfact+3), npar(par_size - nfact - 3, 1.0);
    for(int i = 0; i < nfact+3; ++i)
        dpar[i] = par[i];
    for(int i = nfact+3; i < par_size; ++i)
        npar[i - (nfact+3) + nfact] = par[i];
    vector<double> Pd(N*2), Pn(N*(ncat-1));
    P_dich(Pd, dpar, Theta, N, nfact);
    P_nominal(Pn, npar, Theta, N, nfact, ncat-1, 0, 0);
    NumericMatrix PD = vec2mat(Pd, N, 2);
    NumericMatrix PN = vec2mat(Pn, N, ncat-1);

    int k = 0, which = 0;
    for(int i = 0; i < ncat; ++i){
        if((i+1) == correct){
            for(int j = 0; j < N; ++j){
                P[which] = PD(j,1);
                ++which;
            }
            --k;
        } else {
            for(int j = 0; j < N; ++j){
                P[which] = PD(j,0) * PN(j,k);
                ++which;
            }
        }
        ++k;
    }
}

void P_comp(vector<double> &P, const vector<double> &par,
    const NumericMatrix &Theta, const int &N, const int &nfact)
{
    vector<double> a(nfact), d(nfact);
    for(int j = 0; j < nfact; ++j){
        a[j] = par[j];
        d[j] = par[j+nfact];
    }
    const double gtmp = par[nfact*2];
    const double g = antilogit(&gtmp);
    for(int i = 0; i < N; ++i) P[i+N] = 1.0;
    for(int j = 0; j < nfact; ++j)
        for(int i = 0; i < N; ++i)
            P[i+N] = P[i+N] * (1.0 / (1.0 + exp(-(a[j] * Theta(i,j) + d[j]))));
    for(int i = 0; i < N; ++i){
        P[i+N] = g + (1.0 - g) * P[i+N];
        if(P[i+N] < 1e-20) P[i+N] = 1e-20;
        else if (P[i+N] > 1.0 - 1e-20) P[i+N] = 1.0 - 1e-20;
        P[i] = 1.0 - P[i+N];
    }
}

NumericMatrix ProbTrace(SEXP)