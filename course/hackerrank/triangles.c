#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct triangle
{
	int a;
	int b;
	int c;
};

typedef struct triangle triangle;

double calc_areas(triangle tr) {
    double p = (tr.a + tr.b + tr.c) / 2.0;
    // printf("%f\n", p);
    return (double) sqrt(p * (p - tr.a) * (p - tr.b) * (p - tr.c));
}

void swap_int(int *xp, int *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
}

// void swap_double(int *xp, int *yp) 
// { 
//     int temp = *xp; 
//     *xp = *yp; 
//     *yp = temp; 
// }

void sort_by_area(triangle* tr, int n) {
	/**
	* Sort an array a of the length n
	*/
    int i, j, min_pos;
    double areas[n], dtmp;
    triangle tri_tmp;
    for (i = 0; i < n; i++) {
        areas[i] = calc_areas(tr[i]);
    }

    // for (i = 0; i < n; i++) 
    // 	printf("%f\n", areas[i]);

    for (i = 0; i < n-1; i++) {
        min_pos = i;
        for (j = i+1; j < n; j++)
            if (areas[j] < areas[min_pos])
                min_pos = j;
        if (min_pos != i) {
            dtmp = areas[i];
            areas[i] = areas[min_pos];
            areas[min_pos] = dtmp;

            swap_int(&tr[i].a, &tr[min_pos].a);
            swap_int(&tr[i].b, &tr[min_pos].b);
            swap_int(&tr[i].c, &tr[min_pos].c);
        }
    }
    // printf("areas sort\n");
    // for (i = 0; i < n; i++) 
    // 	printf("%f\n", areas[i]);
}

int main()
{
	int n, i;
	scanf("%d", &n);
	triangle *tr = malloc(n * sizeof(triangle));
	for (i = 0; i < n; i++) {
		scanf("%d%d%d\n", &tr[i].a, &tr[i].b, &tr[i].c);
	}
	sort_by_area(tr, n);
	for (i = 0; i < n; i++) {
		printf("%d %d %d\n", tr[i].a, tr[i].b, tr[i].c);
	}
	return 0;
}