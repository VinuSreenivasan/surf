
/*
* Compile using the command:
* `cc 27Stencil.c -o oa -fopenmp -lm`
*/

#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _OPENACC
#include <openacc.h>
#endif

#define DEFAULT_DATASIZE 1048576  /* Default datasize. */
#define DEFAULT_REPS 10           /* Default repetitions. */
#define CONF95 1.96

#define ITERATIONS 10
#define FAC (1./26)
#define TOLERANCE 1.0e-15

extern int reps;              /* Repetitions. */
extern double *times;         /* Array to store results in. */
extern int flag;              /* Flag to set CPU or GPU invocation. */
extern unsigned int datasize; /* Datasize passed to benchmark functions. */

unsigned int datasize = -1;       /* Datasize for tests in bytes. */
int reps = -1;                    /* Repetitions. */


double *times;           /* Array of doubles storing the benchmark times in microseconds. */
double testtime;         /* The average test time in microseconds for reps runs. */
double testsd;           /* The standard deviation in the test time in microseconds for reps runs. */
int flag = 0;            /* 0 indicates CPU. */


/*
 * Function prototypes for common functions.
 */
void init(int argc, char **argv);
void finalisetest(char *);
void finalise(void);
void benchmark(char *, double (*test)(void));
void print_results(char *, double, double);


/* Forward Declarations of utility functions*/
double max_diff(double *, double *, int);

void wul();


void usage(char *argv[]) {
  printf("Usage: %s \n"
         "\t--reps <repetitions> (default %d)\n"
         "\t--datasize <datasize> (default %d bytes)\n",
         argv[0],
         DEFAULT_REPS, DEFAULT_DATASIZE);
}

/*
 * This function parses the parameters from the command line.
 */
void parse_args(int argc, char *argv[]) {
  
  int arg;
  for (arg = 1; arg < argc; arg++) {

    if (strcmp(argv[arg], "--reps") == 0) {
      reps = atoi(argv[++arg]);
      if (reps == 0) {
        printf("Invalid integer:--reps: %s\n", argv[arg]);
        usage(argv);
        exit(EXIT_FAILURE);
      }

    } else if (strcmp(argv[arg], "--datasize") == 0) {
      datasize = atoi(argv[++arg]);
      if (datasize == 0) {
        printf("Invalid integer:--datasize: %s\n", argv[arg]);
        usage(argv);
        exit(EXIT_FAILURE);
      }

    } else if (strcmp(argv[arg], "-h") == 0) {
      usage(argv);
      exit(EXIT_SUCCESS);

    } else {
      printf("Invalid parameters: %s\n", argv[arg]);
      usage(argv);
      exit(EXIT_FAILURE);
    }
  }
}

void stats(double *mtp, double *sdp) {

  double meantime, totaltime, sumsq, mintime, maxtime, sd;
  int i, good_reps;


  mintime = 1.0e10;
  maxtime = 0.;
  totaltime = 0.;
  good_reps = 0;

  for (i = 0; i < reps; i++) {
    /* Skip entries where times is 0, this indicates an error occured */
    if (times[i] != 0){
      mintime = (mintime < times[i]) ? mintime : times[i];
      maxtime = (maxtime > times[i]) ? maxtime : times[i];
      totaltime += times[i];
      good_reps++;
    }
  }

  meantime = totaltime / good_reps;
  sumsq = 0;

  for (i = 0; i < reps; i++) {
    if (times[i] != 0){
      sumsq += (times[i] - meantime) * (times[i] - meantime);
    }
  }
  sd = sqrt(sumsq / good_reps);

  *mtp = meantime;
  *sdp = sd;

}

/*
 * This function prints the results of the tests.
 * If you use a compiler which sets a different preprocessor flag
 * you may wish to add it here.
 */
void print_results(char *name, double testtime, double testsd) {

  char compiler[20];

  /* Set default compiler idetifier. */
  sprintf(compiler, "COMPILER");

  /* Set compiler identifier based on known preprocessor flags. */
#ifdef __PGI
  sprintf(compiler, "PGI");
#endif

#ifdef __HMPP
  sprintf(compiler, "CAPS");
#endif

printf("%s  %s  %d %f %f\n", compiler, name, datasize, testtime*1e6, CONF95*testsd*1e6);

}

/*
 * This function initialises the storage for the test results and set the defaults.
 */
void init(int argc, char **argv)
{

  parse_args(argc, argv);

  if (reps == -1) {
    reps = DEFAULT_REPS;
  }

  if (datasize == (unsigned int)-1) {
    datasize = DEFAULT_DATASIZE;
  }


  times = (double *)malloc((reps) * sizeof(double));

  /*
#ifdef __PGI
  acc_init(acc_device_nvidia);
  //  printf("PGI INIT\n");
#endif

#ifdef __HMPP
  int a[5] = {1,2,3,4,5};
#pragma acc data copyin(a[0:5])
{}
#endif

#ifdef _CRAYC
  int a[5] = {1,2,3,4,5};
#pragma acc data copyin(a[0:5])
{}
#endif

  */

}

void finalise(void) {
  free(times);
}


/* 
 * This function runs the benchmark specified.
 */
void benchmark(char *name, double (*test)(void))
{
  int i = 0;
  double tmp = 0;


  for (i=0; i<reps; i++) {
    tmp = test();
    if (tmp == -10000){
      printf("Memory allocation failure in %s\n", name);
      times[i] = 0;
    }
    else if (tmp == -11000){
      printf("CPU/GPU mismatch in %s\n", name);
      times[i] = 0;
    }
    else{
      times[i] = tmp;
    }
  }

  stats(&testtime, &testsd);
  //printf("in benchmark\n");
  print_results(name, testtime, testsd);
 //printf("printed result\n");

}

double stencil()
{
  extern unsigned int datasize;
  int sz = cbrt((datasize/sizeof(double))/2);
  int i, j, k, iter;
  int n = sz-2;
  double fac = FAC;
  double t1, t2;
  double md;
//printf("size = %d\n", sz);
  /* Work buffers, with halos */
  double *a0 = (double*)malloc(sizeof(double)*sz*sz*sz);
  double *device_result = (double*)malloc(sizeof(double)*sz*sz*sz);
  double *a1 = (double*)malloc(sizeof(double)*sz*sz*sz);
  double *host_result = (double*)malloc(sizeof(double)*sz*sz*sz);
  double *a0_init = (double*)malloc(sizeof(double)*sz*sz*sz);

  if(a0==NULL||device_result==NULL||a1==NULL||host_result==NULL||a0_init==NULL){
    /* Something went wrong in the memory allocation here, fail gracefully */
    return(-10000);
  }


  /* initialize input array a0 */

  /* zero all of array (including halos) */
  //printf("size = %d\n", sz);
  for (i = 0; i < sz; i++) {
    for (j = 0; j < sz; j++) {
      for (k = 0; k < sz; k++) {
        a0[i*sz*sz+j*sz+k] = 0.0;
	//printf("%d\t", (i*sz*sz+j*sz+k));
      }
    }
 }
  //printf("\n");
//int size_of_a0 = sizeof(a0) / sizeof(*a0);
//printf("size of a0 = %d\n", size_of_a0);

  /* use random numbers to fill interior */
  for (i = 1; i < n+1; i++) {
    for (j = 1; j < n+1; j++) {
      for (k = 1; k < n+1; k++) {
        a0[i*sz*sz+j*sz+k] = (double) rand()/ (double)(1.0 + RAND_MAX);
      }
    }
  }

  /* memcpy(&a0_init[0], &a0[0], sizeof(double)*sz*sz*sz); */

  /* save initial input array for later GPU run */
  for (i = 0; i < sz; i++) {
    for (j = 0; j < sz; j++) {
      for (k = 0; k < sz; k++) {
        a0_init[i*sz*sz+j*sz+k] = a0[i*sz*sz+j*sz+k];
      }
    }
  }

 //printf("Host computation\n");
  /* run main computation on host */

  for (iter = 0; iter < ITERATIONS; iter++) {

    for (i = 1; i < n+1; i++) {
      for (j = 1; j < n+1; j++) {
        for (k = 1; k < n+1; k++) {
          a1[i*sz*sz+j*sz+k] = (
                         a0[i*sz*sz+(j-1)*sz+k] + a0[i*sz*sz+(j+1)*sz+k] +
                         a0[(i-1)*sz*sz+j*sz+k] + a0[(i+1)*sz*sz+j*sz+k] +
                         a0[(i-1)*sz*sz+(j-1)*sz+k] + a0[(i-1)*sz*sz+(j+1)*sz+k] +
                         a0[(i+1)*sz*sz+(j-1)*sz+k] + a0[(i+1)*sz*sz+(j+1)*sz+k] +
                         a0[i*sz*sz+(j-1)*sz+(k-1)] + a0[i*sz*sz+(j+1)*sz+(k-1)] +
                         a0[(i-1)*sz*sz+j*sz+(k-1)] + a0[(i+1)*sz*sz+j*sz+(k-1)] +
                         a0[(i-1)*sz*sz+(j-1)*sz+(k-1)] + a0[(i-1)*sz*sz+(j+1)*sz+(k-1)] +
                         a0[(i+1)*sz*sz+(j-1)*sz+(k-1)] + a0[(i+1)*sz*sz+(j+1)*sz+(k-1)] +
                         a0[i*sz*sz+(j-1)*sz+(k+1)] + a0[i*sz*sz+(j+1)*sz+(k+1)] +
                         a0[(i-1)*sz*sz+j*sz+(k+1)] + a0[(i+1)*sz*sz+j*sz+(k+1)] +
                         a0[(i-1)*sz*sz+(j-1)*sz+(k+1)] + a0[(i-1)*sz*sz+(j+1)*sz+(k+1)] +
                         a0[(i+1)*sz*sz+(j-1)*sz+(k+1)] + a0[(i+1)*sz*sz+(j+1)*sz+(k+1)] +
                         a0[i*sz*sz+j*sz+(k-1)] + a0[i*sz*sz+j*sz+(k+1)]
                         ) * fac;
        }
      }
    }


    for (i = 1; i < n+1; i++) {
      for (j = 1; j < n+1; j++) {
        for (k = 1; k < n+1; k++) {
          a0[i*sz*sz+j*sz+k] = a1[i*sz*sz+j*sz+k];
        }
      }
    }





  } /* end iteration loop */

  /* save result */
  /* memcpy(&host_result[0], &a0[0], sizeof(double)*sz*sz*sz); */
  for (i = 0; i < sz; i++) {
    for (j = 0; j < sz; j++) {
      for (k = 0; k < sz; k++) {
        host_result[i*sz*sz+j*sz+k] = a0[i*sz*sz+j*sz+k];
//	printf("%lf\t", a0[i*sz*sz+j*sz+k]);
      }
    }
  }
   
  //int size = sizeof(host_result)/sizeof(host_result[0]);

  //for(i = 0; i < size; i++) {
  //	printf("%lf\t", host_result[i]);
  //}
  //printf("\n");

  /* copy initial array back to a0 */
  /* memcpy(&a0[0], &a0_init[0], sizeof(double)*sz*sz*sz); */
  for (i = 0; i < sz; i++) {
    for (j = 0; j < sz; j++) {
      for (k = 0; k < sz; k++) {
        a0[i*sz*sz+j*sz+k] = a0_init[i*sz*sz+j*sz+k];
      }
    }
  }

//printf("Starting acc pragma code\n");
  t1 = omp_get_wtime();
#pragma acc data copy(a0[0:sz*sz*sz]), create(a1[0:sz*sz*sz], i,j,k,iter), copyin(sz,fac,n)
  {

    for (iter = 0; iter < ITERATIONS; iter++) {
#LOOP1
      for (i = 1; i < n+1; i++) {
#LOOP2
        for (j = 1; j < n+1; j++) {
#LOOP3
          for (k = 1; k < n+1; k++) {
            a1[i*sz*sz+j*sz+k] = (
                         a0[i*sz*sz+(j-1)*sz+k] + a0[i*sz*sz+(j+1)*sz+k] +
                         a0[(i-1)*sz*sz+j*sz+k] + a0[(i+1)*sz*sz+j*sz+k] +
                         a0[(i-1)*sz*sz+(j-1)*sz+k] + a0[(i-1)*sz*sz+(j+1)*sz+k] +
                         a0[(i+1)*sz*sz+(j-1)*sz+k] + a0[(i+1)*sz*sz+(j+1)*sz+k] +
                         a0[i*sz*sz+(j-1)*sz+(k-1)] + a0[i*sz*sz+(j+1)*sz+(k-1)] +
                         a0[(i-1)*sz*sz+j*sz+(k-1)] + a0[(i+1)*sz*sz+j*sz+(k-1)] +
                         a0[(i-1)*sz*sz+(j-1)*sz+(k-1)] + a0[(i-1)*sz*sz+(j+1)*sz+(k-1)] +
                         a0[(i+1)*sz*sz+(j-1)*sz+(k-1)] + a0[(i+1)*sz*sz+(j+1)*sz+(k-1)] +
                         a0[i*sz*sz+(j-1)*sz+(k+1)] + a0[i*sz*sz+(j+1)*sz+(k+1)] +
                         a0[(i-1)*sz*sz+j*sz+(k+1)] + a0[(i+1)*sz*sz+j*sz+(k+1)] +
                         a0[(i-1)*sz*sz+(j-1)*sz+(k+1)] + a0[(i-1)*sz*sz+(j+1)*sz+(k+1)] +
                         a0[(i+1)*sz*sz+(j-1)*sz+(k+1)] + a0[(i+1)*sz*sz+(j+1)*sz+(k+1)] +
                         a0[i*sz*sz+j*sz+(k-1)] + a0[i*sz*sz+j*sz+(k+1)]
                           ) * fac;
          }
        }
      }

#pragma acc parallel loop
      for (i = 1; i < n+1; i++) {
#pragma acc loop
        for (j = 1; j < n+1; j++) {
#pragma acc loop
          for (k = 1; k < n+1; k++) {
            a0[i*sz*sz+j*sz+k] = a1[i*sz*sz+j*sz+k];
          }
        }
      }


    } /* end iteration loop */

  } /* end data region */
#pragma acc wait
  t2 = omp_get_wtime();

  memcpy(&device_result[0], &a0[0], sizeof(double)*sz*sz*sz);
  md = max_diff(&host_result[0],&device_result[0], sz);

    /* Free malloc'd memory to prevent leaks */
  free(a0);
  free(a0_init);
  free(a1);
  free(host_result);
  free(device_result);

  //printf("md: %lf \t tolerance: %lf", md, TOLERANCE);
  if (md < TOLERANCE ){
   //printf ("GPU matches host to within tolerance of %1.1e\n\n", TOLERANCE); 
    return(t2 - t1);
  }
  else{
   // printf ("WARNING: GPU does not match to within tolerance of %1.1e\nIt is %lf\n", TOLERANCE, md); 
    return(-11000);
  }
}

/* Utility Functions */

double max_diff(double *array1,double *array2, int sz)
{
  double tmpdiff, diff;
  int i,j,k;
  int n = sz-2;
  diff=0.0;

  for (i = 1; i < n+1; i++) {
    for (j = 1; j < n+1; j++) {
      for (k = 1; k < n+1; k++) {
        tmpdiff = fabs(array1[i*sz*sz+j*sz+k] - array2[i*sz*sz+j*sz+k]);
	//printf("diff: %lf", tmpdiff);
        if (tmpdiff > diff) diff = tmpdiff;

      }
    }
  }
  return diff;
}


/*
 * This function ensures the device is awake.
 * It is more portable than acc_init().
 */
void wul(){

  int data = 8192;
  double *arr_a = (double *)malloc(sizeof(double) * data);
  double *arr_b = (double *)malloc(sizeof(double) * data);
  int i = 0;

  if (arr_a==NULL||arr_b==NULL) {
      printf("Unable to allocate memory in wul.\n");
  }

  for (i=0;i<data;i++){
    arr_a[i] = (double) (rand()/(1.0+RAND_MAX));
  }

#pragma acc data copy(arr_b[0:data]), copyin(arr_a[0:data])
  {
#pragma acc parallel loop
    for (i=0;i<data;i++){
      arr_b[i] = arr_a[i] * 2;
    }
  }

  if (arr_a[0] < 0){
    printf("Error in WUL\n");
    /*
     * This should never be called as rands should be in the range (0,1].
     * This stops clever optimizers.
     */
  }

  free(arr_a);
  free(arr_b);
}


int main(int argc, char **argv) {
	char testName[32];

	printf("compiler name datasize testtime*1e6 CONF95*testsd*1e6\n");
	
	/* Initialise storage for test results & parse input arguements. */
  	init(argc, argv);

  	/* Ensure device is awake. */
  	wul();

	sprintf(testName, "27S");
	benchmark(testName, &stencil);
	/* Print results & free results storage */
	finalise();

	return EXIT_SUCCESS;
}

