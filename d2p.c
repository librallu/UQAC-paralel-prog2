#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>

int CUTOFF = 1000;


int max(int a, int b){ return a<b?b:a; }

int find(int x, int* T, int p, int r){
	int inf = p;
	int sup = max(p, r+1);
	int mil;
	while ( inf < sup ){
		mil = (inf+sup)/2;
		if ( x <= T[mil] ){
			sup = mil;
		} else {
			inf = mil+1;
		}
	}
	return sup;
}

void swap(int* a, int* b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void* create_params(int* T, int p1, int r1, int p2, int r2, int* A, int p3){
	long* params = malloc(7*sizeof(long));
	params[0] = (long) T;
	params[1] = (long) p1;
	params[2] = (long) r1;
	params[3] = (long) p2;
	params[4] = (long) r2;
	params[5] = (long) A;
	params[6] = (long) p3;
	return params;
}

static void* fn_fusion(void* data);

void p_fusion_seq(int* T, int p1, int r1, int p2, int r2, int* A, int p3){
	int i=p1, j=p2, k;
	int n1 = r1 - p1 + 1;
	int n2 = r2 - p2 + 1;
	for (k=0 ; k<n1+n2 ; k++) {
		if ( i == p1+n1 ) {
			A[k] = T[j++];
		} else if ( j == p2+n2 ) {
			A[k] = T[i++];
		} else {
			A[k] = (T[i] <= T[j]) ? T[i++] : T[j++];
		}		
	}
}

void p_fusion(int* T, int p1, int r1, int p2, int r2, int* A, int p3){
	int n1 = r1 - p1 + 1;
	int n2 = r2 - p2 + 1;
	if ( n1 < CUTOFF ) {
		return p_fusion_seq(T,p1,r1,p2,r2,A,p3);
	}
	if ( n1 < n2 ){
		swap(&p1, &p2);
		swap(&r1, &r2);
		swap(&n1, &n2);
	}
	if ( n1 == 0 ) {
		return;
	}
	else {
		int q1 = (p1+r1)/2;
		int q2 = find(T[q1], T, p2, r2);
		int q3 = p3 + (q1-p1) + (q2-p2);
		A[q3] = T[q1];
		
		//~ spawn p_fusion(T, p1, q1-1, p2, q2-1, A, p3);
		void* params = create_params(T, p1, q1-1, p2, q2-1, A, p3);
		pthread_t th_id;
		int rc = pthread_create(&th_id, NULL, fn_fusion, params);
		if (rc) {
            fprintf (stderr, "error %d", rc);
        }
        
        // second call to fusion
		p_fusion(T, q1+1, r1, q2, r2, A, q3+1);
		
		// join
		pthread_join(th_id, NULL);
		
	}
}




static void* fn_fusion(void* data) {
	long* d = (long*) data;
	p_fusion((int*)(d[0]), (int)(d[1]), (int)(d[2]), (int)(d[3]), 
		(int)(d[4]), (int*)(d[5]), (int)(d[6]) );
	free(data);
	return NULL;
}



int main() {
	//~ int U[] = {3, 4, 7, 10, 14, 25};
	//~ int res = find(8, U, 0, 5);
	//~ printf("pos: %d\n", res);
	//~ 
	//~ int a = 2;
	//~ int b = 3;
	//~ printf("a:%d, b:%d\n",a,b);
	//~ swap(&a,&b);
	//~ printf("a:%d, b:%d\n",a,b);
	//~ 
	//~ int I[] = {5,6,7,8, 1,2,3,4};
	//~ int R[] = {0,0,0,0, 0,0,0,0};
	//~ int k;
	//~ for ( k = 0 ; k < 8 ; k++ ){
		//~ printf("%d ", I[k]);
	//~ } printf("\n");
	//~ p_fusion(I, 0, 3, 4, 7, R, 0);
	//~ for ( k = 0 ; k < 8 ; k++ ){
		//~ printf("%d ", R[k]);
	//~ } printf("\n");
	//~ 
	int n;
	scanf("%d", &n);
	CUTOFF = n/omp_get_num_threads();
	printf("CUTOFF: %d\n", CUTOFF);
	int* T = (int*)malloc(sizeof(int)*2*n);
	int* R = (int*)malloc(sizeof(int)*2*n);
	int i;
	for ( i = 0 ; i < 2*n ; i++ ) {
		scanf("%d", T+i);
	}
	
	//~ for ( i = 0 ; i < 2*n ; i++ ){
		//~ printf("%d ", T[i]);
	//~ } printf("\n---\n");
	double start = omp_get_wtime();
	p_fusion(T, 0, n-1, n, 2*n-1, R, 0);
	double stop = omp_get_wtime();
	//~ for ( i = 0 ; i < 2*n ; i++ ){
		//~ printf("%d ", R[i]);
	//~ } printf("\n");
	
	free(T);
	printf("execution time of merge: %lf\n", stop-start);
	
	return 0;
}
