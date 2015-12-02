#include <stdio.h>
#include <stdlib.h>

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

void p_fusion(int* T, int p1, int r1, int p2, int r2, int* A, int p3){
	int n1 = r1 - p1 + 1;
	int n2 = r2 - p2 + 1;
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
		p_fusion(T, p1, q1-1, p2, q2-1, A, p3);
		p_fusion(T, q1+1, r1, q2, r2, A, q3+1);
	}
}

int p_fusion_pthread(int* T, int p1, int r1, int p2, int r2, int* A, int p3) {
	int* params = (int*) malloc(sizeof(int)*7);
	params[0] = T;
	params[1] = p1;
	params[2] = r1;
	params[3] = p2;
	params[4] = r2;
	params[5] = A;
	params[6] = p3;
	int rc = pthread_create(&threads[i*N+j], &attr, p_fusion, (void*) params);
	if ( rc ){
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
	return rc;
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
	int* T = (int*)malloc(sizeof(int)*2*n);
	int* R = (int*)malloc(sizeof(int)*2*n);
	int i;
	for ( i = 0 ; i < 2*n ; i++ ) {
		scanf("%d", T+i);
	}
	
	for ( i = 0 ; i < 2*n ; i++ ){
		printf("%d ", T[i]);
	} printf("\n---\n");
	p_fusion_pthread(T, 0, n-1, n, 2*n-1, R, 0);
	for ( i = 0 ; i < 2*n ; i++ ){
		printf("%d ", R[i]);
	} printf("\n");
	
	free(T);
	
	return 0;
}

