/**************************************************************************************************
*                                                                                                 *
* This file is part of HPIPM.                                                                     *
*                                                                                                 *
* HPIPM -- High-Performance Interior Point Method.                                                *
* Copyright (C) 2019 by Gianluca Frison.                                                          *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* The 2-Clause BSD License                                                                        *
*                                                                                                 *
* Redistribution and use in source and binary forms, with or without                              *
* modification, are permitted provided that the following conditions are met:                     *
*                                                                                                 *
* 1. Redistributions of source code must retain the above copyright notice, this                  *
*    list of conditions and the following disclaimer.                                             *
* 2. Redistributions in binary form must reproduce the above copyright notice,                    *
*    this list of conditions and the following disclaimer in the documentation                    *
*    and/or other materials provided with the distribution.                                       *
*                                                                                                 *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND                 *
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED                   *
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                          *
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR                 *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES                  *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;                    *
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND                     *
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT                      *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS                   *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                    *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/

/**
*
* Basic example of solving an OCP QP with data provided in the format generated by the routine
* d_ocp_qp_codegen(...);
*
**/

#include <stdlib.h>
#include <stdio.h>

#include <blasfeo_d_aux_ext_dep.h>

#include <hpipm_d_ocp_qp_solver.h>
#include <hpipm_d_ocp_qp_dim.h>
#include <hpipm_d_ocp_qp.h>
#include <hpipm_d_ocp_qp_sol.h>
#include <hpipm_d_ocp_qp_utils.h>
#include <hpipm_timing.h>



// qp data as global data
extern int N;
extern int *nx;
extern int *nu;
extern int *nbu;
extern int *nbx;
extern int *ng;
extern int *nsbx;
extern int *nsbu;
extern int *nsg;
extern int *nbue;
extern int *nbxe;
extern int *nge;
extern double **hA;
extern double **hB;
extern double **hb;
extern double **hQ;
extern double **hR;
extern double **hS;
extern double **hq;
extern double **hr;
extern int **hidxbx;
extern double **hlbx;
extern double **hlbx_mask;
extern double **hubx;
extern double **hubx_mask;
extern int **hidxbu;
extern double **hlbu;
extern double **hlbu_mask;
extern double **hubu;
extern double **hubu_mask;
extern double **hC;
extern double **hD;
extern double **hlg;
extern double **hlg_mask;
extern double **hug;
extern double **hug_mask;
extern double **hZl;
extern double **hZu;
extern double **hzl;
extern double **hzu;
extern int **hidxs;
extern double **hlls;
extern double **hlls_mask;
extern double **hlus;
extern double **hlus_mask;
extern int **hidxe;
// arg
extern int mode;
extern int iter_max;
extern double alpha_min;
extern double mu0;
extern double tol_stat;
extern double tol_eq;
extern double tol_ineq;
extern double tol_comp;
extern double reg_prim;
extern int warm_start;
extern int pred_corr;
extern int ric_alg;
extern int split_step;



// main
int main()
	{

	int ii, jj;

	int hpipm_status;

	int rep, nrep=10;

	hpipm_timer timer;

/************************************************
* ocp qp dim
************************************************/

	hpipm_size_t dim_size = d_ocp_qp_dim_memsize(N);
	void *dim_mem = malloc(dim_size);

	struct d_ocp_qp_dim dim;
	d_ocp_qp_dim_create(N, &dim, dim_mem);

	// global setter
	d_ocp_qp_dim_set_all(nx, nu, nbx, nbu, ng, nsbx, nsbu, nsg, &dim);
	// individual setters
	d_ocp_qp_dim_set_nbxe(0, nbxe[0], &dim);

//	d_ocp_qp_dim_codegen("examples/c/data/test_d_ocp_data.c", "w", &dim);

/************************************************
* ocp qp
************************************************/

	hpipm_size_t qp_size = d_ocp_qp_memsize(&dim);
	void *qp_mem = malloc(qp_size);

	struct d_ocp_qp qp;
	d_ocp_qp_create(&dim, &qp, qp_mem);

	// global setter
	d_ocp_qp_set_all(hA, hB, hb, hQ, hS, hR, hq, hr, hidxbx, hlbx, hubx, hidxbu, hlbu, hubu, hC, hD, hlg, hug, hZl, hZu, hzl, hzu, hidxs, hlls, hlus, &qp);
	// individual setters
	d_ocp_qp_set_idxe(0, hidxe[0], &qp);

	// set inequality constraints mask
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_set_lbu_mask(ii, hlbu_mask[ii], &qp);
		d_ocp_qp_set_ubu_mask(ii, hubu_mask[ii], &qp);
		d_ocp_qp_set_lbx_mask(ii, hlbx_mask[ii], &qp);
		d_ocp_qp_set_ubx_mask(ii, hubx_mask[ii], &qp);
		d_ocp_qp_set_lg_mask(ii, hlg_mask[ii], &qp);
		d_ocp_qp_set_ug_mask(ii, hug_mask[ii], &qp);
		d_ocp_qp_set_lls_mask(ii, hlls_mask[ii], &qp);
		d_ocp_qp_set_lus_mask(ii, hlus_mask[ii], &qp);
		}

//	d_ocp_qp_codegen("examples/c/data/test_d_ocp_data.c", "a", &dim, &qp);

/************************************************
* ocp qp sol
************************************************/

	hpipm_size_t qp_sol_size = d_ocp_qp_sol_memsize(&dim);
	void *qp_sol_mem = malloc(qp_sol_size);

	struct d_ocp_qp_sol qp_sol;
	d_ocp_qp_sol_create(&dim, &qp_sol, qp_sol_mem);

/************************************************
* solver arg
************************************************/

	hpipm_size_t solver_arg_size = d_ocp_qp_solver_arg_memsize(&dim);
	void *solver_arg_mem = malloc(solver_arg_size);

	struct d_ocp_qp_solver_arg arg;
	d_ocp_qp_solver_arg_create(&dim, &arg, solver_arg_mem);

	d_ocp_qp_solver_arg_set_default(mode, &dim, &arg);

	//d_ocp_qp_solver_arg_set_iter_max(&iter_max, &arg);
	d_ocp_qp_solver_arg_set("iter_max", &iter_max, &arg);
	//d_ocp_qp_solver_arg_set_alpha_min(&alpha_min, &arg);
	d_ocp_qp_solver_arg_set("alpha_min", &alpha_min, &arg);
	//d_ocp_qp_solver_arg_set_mu0(&mu0, &arg);
	d_ocp_qp_solver_arg_set("mu0", &mu0, &arg);
	//d_ocp_qp_solver_arg_set_tol_stat(&tol_stat, &arg);
	d_ocp_qp_solver_arg_set("tol_stat", &tol_stat, &arg);
	//d_ocp_qp_solver_arg_set_tol_eq(&tol_eq, &arg);
	d_ocp_qp_solver_arg_set("tol_eq", &tol_eq, &arg);
	//d_ocp_qp_solver_arg_set_tol_ineq(&tol_ineq, &arg);
	d_ocp_qp_solver_arg_set("tol_ineq", &tol_ineq, &arg);
	//d_ocp_qp_solver_arg_set_tol_comp(&tol_comp, &arg);
	d_ocp_qp_solver_arg_set("tol_comp", &tol_comp, &arg);
	//d_ocp_qp_solver_arg_set_reg_prim(&reg_prim, &arg);
	d_ocp_qp_solver_arg_set("reg_prim", &reg_prim, &arg);
	//d_ocp_qp_solver_arg_set_warm_start(&warm_start, &arg);
	d_ocp_qp_solver_arg_set("warm_start", &warm_start, &arg);
	//d_ocp_qp_solver_arg_set_pred_corr(&pred_corr, &arg);
	d_ocp_qp_solver_arg_set("pred_corr", &pred_corr, &arg);
	//d_ocp_qp_solver_arg_set_ric_alg(&ric_alg, &arg);
	d_ocp_qp_solver_arg_set("ric_alg", &ric_alg, &arg);
	//d_ocp_qp_solver_arg_set_split_step(&split_step, &arg);
	d_ocp_qp_solver_arg_set("split_step", &split_step, &arg);

	int reduce_eq_dof = 1;
	d_ocp_qp_solver_arg_set("reduce_eq_dof", &reduce_eq_dof, &arg);

//	d_ocp_qp_ipm_arg_codegen("examples/c/data/test_d_ocp_data.c", "a", &dim, &arg);

/************************************************
* solver workspace
************************************************/

	hpipm_size_t solver_size = d_ocp_qp_solver_ws_memsize(&dim, &arg);
	void *solver_mem = malloc(solver_size);

	struct d_ocp_qp_solver_ws workspace;
	d_ocp_qp_solver_ws_create(&dim, &arg, &workspace, solver_mem);

/************************************************
* solver
************************************************/

	// overwrite selected args
	//int iter_max1 = 2;
	//double tol1 = 1e-3;
	//d_ocp_qp_solver_set("iter_max", &iter_max1, &workspace);
	//d_ocp_qp_solver_set_iter_max(&iter_max1, &workspace);
	//d_ocp_qp_solver_set_tol_stat(&tol1, &workspace);
	//d_ocp_qp_solver_set_tol_eq(&tol1, &workspace);
	//d_ocp_qp_solver_set_tol_ineq(&tol1, &workspace);
	//d_ocp_qp_solver_set_tol_comp(&tol1, &workspace);

	hpipm_tic(&timer);

	for(rep=0; rep<nrep; rep++)
		{
		// call solver
		d_ocp_qp_solver_solve(&qp, &qp_sol, &workspace);
		d_ocp_qp_solver_get_status(&workspace, &hpipm_status);
		//d_ocp_qp_solver_get("status", &workspace, &hpipm_status);
		}

	double time_ipm = hpipm_toc(&timer) / nrep;

/************************************************
* print solution info
************************************************/

    printf("\nHPIPM returned with flag %i.\n", hpipm_status);
    if(hpipm_status == 0)
		{
        printf("\n -> QP solved!\n");
		}
	else if(hpipm_status==1)
		{
        printf("\n -> Solver failed! Maximum number of iterations reached\n");
		}
	else if(hpipm_status==2)
		{
        printf("\n -> Solver failed! Minimum step lenght reached\n");
		}
	else if(hpipm_status==3)
		{
        printf("\n -> Solver failed! NaN in computations\n");
		}
	else
		{
        printf("\n -> Solver failed! Unknown return flag\n");
		}
    printf("\nAverage solution time over %i runs: %e [s]\n", nrep, time_ipm);
	printf("\n\n");

/************************************************
* extract and print solution
************************************************/

	// u

	int nu_max = nu[0];
	for(ii=1; ii<=N; ii++)
		if(nu[ii]>nu_max)
			nu_max = nu[ii];

	double *u = malloc(nu_max*sizeof(double));

	printf("\nu = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_sol_get_u(ii, &qp_sol, u);
		d_print_mat(1, nu[ii], u, 1);
		}


	// x

	int nx_max = nx[0];
	for(ii=1; ii<=N; ii++)
		if(nx[ii]>nx_max)
			nx_max = nx[ii];

	double *x = malloc(nx_max*sizeof(double));

	printf("\nx = \n");
	for(ii=0; ii<=N; ii++)
		{
		d_ocp_qp_sol_get_x(ii, &qp_sol, x);
		d_print_mat(1, nx[ii], x, 1);
		}


	// pi

	double *pi = malloc(nx_max*sizeof(double));

	printf("\npi = \n");
	for(ii=0; ii<N; ii++)
		{
		d_ocp_qp_sol_get_pi(ii, &qp_sol, pi);
		d_print_mat(1, nx[ii+1], pi, 1);
		}


	// all solution components at once

	double **u1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) u1[ii] = malloc((nu[ii])*sizeof(double));
	double **x1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) x1[ii] = malloc((nx[ii])*sizeof(double));
	double **ls1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) ls1[ii] = malloc((nsbu[ii]+nsbx[ii]+nsg[ii])*sizeof(double));
	double **us1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) us1[ii] = malloc((nsbu[ii]+nsbx[ii]+nsg[ii])*sizeof(double));
	double **pi1 = malloc((N)*sizeof(double *)); for(ii=0; ii<N; ii++) pi1[ii] = malloc((nx[ii+1])*sizeof(double));
	double **lam_lb1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) lam_lb1[ii] = malloc((nbu[ii]+nbx[ii])*sizeof(double));
	double **lam_ub1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) lam_ub1[ii] = malloc((nbu[ii]+nbx[ii])*sizeof(double));
	double **lam_lg1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) lam_lg1[ii] = malloc((ng[ii])*sizeof(double));
	double **lam_ug1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) lam_ug1[ii] = malloc((ng[ii])*sizeof(double));
	double **lam_ls1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) lam_ls1[ii] = malloc((nsbu[ii]+nsbx[ii]+nsg[ii])*sizeof(double));
	double **lam_us1 = malloc((N+1)*sizeof(double *)); for(ii=0; ii<=N; ii++) lam_us1[ii] = malloc((nsbu[ii]+nsbx[ii]+nsg[ii])*sizeof(double));

	d_ocp_qp_sol_get_all(&qp_sol, u1, x1, ls1, us1, pi1, lam_lb1, lam_ub1, lam_lg1, lam_ug1, lam_ls1, lam_us1);

//	d_ocp_qp_sol_print(&dim, &qp_sol);

/************************************************
* print ipm statistics
************************************************/

	//int iter; d_ocp_qp_solver_get_iter(&workspace, &iter);
	int iter; d_ocp_qp_solver_get("iter", &workspace, &iter);
	//double res_stat; d_ocp_qp_solver_get_max_res_stat(&workspace, &res_stat);
	double res_stat; d_ocp_qp_solver_get("max_res_stat", &workspace, &res_stat);
	//double res_eq; d_ocp_qp_solver_get_max_res_eq(&workspace, &res_eq);
	double res_eq; d_ocp_qp_solver_get("max_res_eq", &workspace, &res_eq);
	//double res_ineq; d_ocp_qp_solver_get_max_res_ineq(&workspace, &res_ineq);
	double res_ineq; d_ocp_qp_solver_get("max_res_ineq", &workspace, &res_ineq);
	//double res_comp; d_ocp_qp_solver_get_max_res_comp(&workspace, &res_comp);
	double res_comp; d_ocp_qp_solver_get("max_res_comp", &workspace, &res_comp);
	//double *stat; d_ocp_qp_solver_get_stat(&workspace, &stat);
	double *stat; d_ocp_qp_solver_get("stat", &workspace, &stat);
	//int stat_m; d_ocp_qp_solver_get_stat_m(&workspace, &stat_m);
	int stat_m; d_ocp_qp_solver_get("stat_m", &workspace, &stat_m);

	printf("\nipm return = %d\n", hpipm_status);
	printf("\nipm residuals max: res_g = %e, res_b = %e, res_d = %e, res_m = %e\n", res_stat, res_eq, res_ineq, res_comp);

	printf("\nipm iter = %d\n", iter);
	printf("\nalpha_aff\tmu_aff\t\tsigma\t\talpha_prim\talpha_dual\tmu\t\tres_stat\tres_eq\t\tres_ineq\tres_comp\tobj\t\tlq fact\t\titref pred\titref corr\tlin res stat\tlin res eq\tlin res ineq\tlin res comp\n");
	d_print_exp_tran_mat(stat_m, iter+1, stat, stat_m);

	printf("\nocp ipm time = %e [s]\n\n", time_ipm);

/************************************************
* get riccati matrices and vectors
************************************************/

#if 0
	printf("\nget Riccati recursion matrices and vectors\n");

	double *Lr0 = malloc(nu[0]*nu[0]*sizeof(double));
	double *Ls0 = malloc(nx[0]*nu[0]*sizeof(double));
	double *P0 = malloc(nx[0]*nx[0]*sizeof(double));
	double *lr0 = malloc(nu[0]*sizeof(double));
	double *p0 = malloc(nx[0]*sizeof(double));
	double *K0 = malloc(nu[0]*nx[0]*sizeof(double));
	double *k0 = malloc(nu[0]*sizeof(double));

	double *Lr1 = malloc(nu[1]*nu[1]*sizeof(double));
	double *Ls1 = malloc(nx[1]*nu[1]*sizeof(double));
	double *P1 = malloc(nx[1]*nx[1]*sizeof(double));
	double *lr1 = malloc(nu[1]*sizeof(double));
	double *p1 = malloc(nx[1]*sizeof(double));
	double *K1 = malloc(nu[1]*nx[1]*sizeof(double));
	double *k1 = malloc(nu[1]*sizeof(double));

	//
	d_ocp_qp_ipm_get_ric_Lr(&qp, &arg, &workspace, 0, Lr0);
	printf("\nLr0\n");
	d_print_exp_mat(nu[0], nu[0], Lr0, nu[0]);
	//
	d_ocp_qp_ipm_get_ric_Ls(&qp, &arg, &workspace, 0, Ls0);
	printf("\nLs0\n");
	d_print_exp_mat(nx[0], nu[0], Ls0, nx[0]);
	//
	d_ocp_qp_ipm_get_ric_P(&qp, &arg, &workspace, 0, P0);
	printf("\nP0\n");
	d_print_exp_mat(nx[0], nx[0], P0, nx[0]);
	//
	d_ocp_qp_ipm_get_ric_lr(&qp, &arg, &workspace, 0, lr0);
	printf("\nlr0\n");
	d_print_exp_mat(1, nu[0], lr0, 1);
	//
	d_ocp_qp_ipm_get_ric_p(&qp, &arg, &workspace, 0, p0);
	printf("\np0\n");
	d_print_exp_mat(1, nx[0], p0, 1);
	//
	d_ocp_qp_ipm_get_ric_K(&qp, &arg, &workspace, 0, K0);
	printf("\nK0\n");
	d_print_exp_mat(nu[0], nx[0], K0, nu[0]);
	//
	d_ocp_qp_ipm_get_ric_k(&qp, &arg, &workspace, 0, k0);
	printf("\nk0\n");
	d_print_exp_mat(1, nu[0], k0, 1);

	//
	d_ocp_qp_ipm_get_ric_Lr(&qp, &arg, &workspace, 1, Lr1);
	printf("\nLr1\n");
	d_print_exp_mat(nu[1], nu[1], Lr1, nu[1]);
	//
	d_ocp_qp_ipm_get_ric_Ls(&qp, &arg, &workspace, 1, Ls1);
	printf("\nLs1\n");
	d_print_exp_mat(nx[1], nu[1], Ls1, nx[1]);
	//
	d_ocp_qp_ipm_get_ric_P(&qp, &arg, &workspace, 1, P1);
	printf("\nP1\n");
	d_print_exp_mat(nx[1], nx[1], P1, nx[1]);
	//
	d_ocp_qp_ipm_get_ric_lr(&qp, &arg, &workspace, 1, lr1);
	printf("\nlr1\n");
	d_print_exp_mat(1, nu[1], lr1, 1);
	//
	d_ocp_qp_ipm_get_ric_p(&qp, &arg, &workspace, 1, p1);
	printf("\np1\n");
	d_print_exp_mat(1, nx[1], p1, 1);
	//
	d_ocp_qp_ipm_get_ric_K(&qp, &arg, &workspace, 1, K1);
	printf("\nK1\n");
	d_print_exp_mat(nu[1], nx[1], K1, nu[1]);
	//
	d_ocp_qp_ipm_get_ric_k(&qp, &arg, &workspace, 1, k1);
	printf("\nk1\n");
	d_print_exp_mat(1, nu[1], k1, 1);

	free(Lr0);
	free(Ls0);
	free(P0);
	free(lr0);
	free(p0);
	free(K0);
	free(k0);

	free(Lr1);
	free(Ls1);
	free(P1);
	free(lr1);
	free(p1);
	free(K1);
	free(k1);
#endif

/************************************************
* free memory and return
************************************************/

    free(dim_mem);
    free(qp_mem);
	free(qp_sol_mem);
	free(solver_arg_mem);
	free(solver_mem);

	free(u);
	free(x);
	free(pi);


	for(ii=0; ii<N; ii++)
		{
		free(u1[ii]);
		free(x1[ii]);
		free(ls1[ii]);
		free(us1[ii]);
		free(pi1[ii]);
		free(lam_lb1[ii]);
		free(lam_ub1[ii]);
		free(lam_lg1[ii]);
		free(lam_ug1[ii]);
		free(lam_ls1[ii]);
		free(lam_us1[ii]);
		}
	free(u1[ii]);
	free(x1[ii]);
	free(ls1[ii]);
	free(us1[ii]);
	free(lam_lb1[ii]);
	free(lam_ub1[ii]);
	free(lam_lg1[ii]);
	free(lam_ug1[ii]);
	free(lam_ls1[ii]);
	free(lam_us1[ii]);

	free(u1);
	free(x1);
	free(ls1);
	free(us1);
	free(lam_lb1);
	free(lam_ub1);
	free(lam_lg1);
	free(lam_ug1);
	free(lam_ls1);
	free(lam_us1);



	return 0;

	}



