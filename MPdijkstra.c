# include <stdlib.h>
# include <stdio.h>
# include <time.h>
# include <omp.h>
# include <sys/time.h>
# include <math.h>

# define NV 1000
# define ITER 100

int actual_size; 

int main ( int argc, char **argv );
int *dijkstra_distance ( int ohd[NV][NV] );
void find_nearest ( int s, int e, int mind[NV], int connected[NV], int *d, 
  int *v );
void init ( int ohd[NV][NV] );
void timestamp ( void );
void update_mind ( int s, int e, int mv, int connected[NV], int ohd[NV][NV], 
  int mind[NV] );

long getMicrotime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}
/******************************************************************************/
int main ( int argc, char **argv )

{
  for(actual_size = 10; actual_size <= 1000; actual_size *= 10) {
    printf("Number of vertices: %d\n", actual_size);
    int *mind;
    int ohd[NV][NV];
    int k;
    double mean = 0, std = 0;
    int times[ITER]; 
    srand(3);
    
    for(k = 0; k < ITER; k++) 
    {
      long start = getMicrotime();
      init ( ohd );
      mind = dijkstra_distance ( ohd );  
      free ( mind );
      long lasted = getMicrotime() - start;
      times[k] = lasted;
      //printf("Iteration %d lasted %ld us", k, lasted);
    }
    for (k = 0; k < ITER; k++)
      mean += times[k];
    mean /= ITER;
    for (k = 0; k < ITER; k++) 
      std += pow((times[k] - mean), 2.0);
    std /= ITER; 
    std = sqrt(std);
    printf("After %d iterations mean; %.3f and std: %.3f\n", ITER, mean, std);
  }
  return 0;
}
/******************************************************************************/



int *dijkstra_distance ( int ohd[NV][NV]  )
{
  int *connected;
  int i;
  int i4_huge = 2147483647;
  int md;
  int *mind;
  int mv;
  int my_first;
  int my_id;
  int my_last;
  int my_md;
  int my_mv;
  int my_step;
  int nth;
/*
  Start out with only node 0 connected to the tree.
*/
  connected = ( int * ) malloc ( actual_size * sizeof ( int ) );

  connected[0] = 1;
  for ( i = 1; i < actual_size; i++ )
  {
    connected[i] = 0;
  }
/*
  Initial estimate of minimum distance is the 1-step distance.
*/
  mind = ( int * ) malloc ( actual_size * sizeof ( int ) );

  for ( i = 0; i < actual_size; i++ )
  {
    mind[i] = ohd[0][i];
  }
  # pragma omp parallel private ( my_first, my_id, my_last, my_md, my_mv, my_step ) \
  shared ( connected, md, mind, mv, nth, ohd )
  {
    my_id = omp_get_thread_num ( );
    nth = omp_get_num_threads ( ); 
    my_first =   (   my_id       * actual_size ) / nth;
    my_last  =   ( ( my_id + 1 ) * actual_size ) / nth - 1;
    # pragma omp single
    {
      
    }
    
    for ( my_step = 1; my_step < actual_size; my_step++ )
    {
      # pragma omp single 
      {
        md = i4_huge;
        mv = -1; 
      }
      find_nearest ( my_first, my_last, mind, connected, &my_md, &my_mv );
      # pragma omp critical
      {
        if ( my_md < md )  
        {
          md = my_md;
          mv = my_mv;
        }
      }
      # pragma omp barrier
      # pragma omp single 
      {
        if ( mv != - 1 )
        {
          connected[mv] = 1;
          //printf ( "  P%d: Connecting node %d.\n", my_id, mv );
        }
      }
      # pragma omp barrier
      if ( mv != -1 )
      {
        update_mind ( my_first, my_last, mv, connected, ohd, mind );
      }
      #pragma omp barrier
    }
  }

  free ( connected );

  return mind;
}
/******************************************************************************/

void find_nearest ( int s, int e, int mind[NV], int connected[NV], int *d, 
  int *v )

{
  int i;
  int i4_huge = 2147483647;

  *d = i4_huge;
  *v = -1;

  for ( i = s; i <= e; i++ )
  {
    if ( !connected[i] && ( mind[i] < *d ) )
    {
      *d = mind[i];
      *v = i;
    }
  }
  return;
}

void init ( int ohd[NV][NV] )

{
  int i;
  int i4_huge = 2147483647;
  int j;

  for ( i = 0; i < actual_size; i++ )  
  {
    for ( j = 0; j < actual_size; j++ )
    {
      if ( i == j ) 
      {
        ohd[i][i] = 0;
      }
      else
      {
        
        ohd[i][j] = rand() % 100 +1;
      }
    }
  }

  return;
}
/******************************************************************************/

void timestamp ( void )

{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  printf ( "%s\n", time_buffer );

  return;
# undef TIME_SIZE
}
/******************************************************************************/

void update_mind ( int s, int e, int mv, int connected[NV], int ohd[NV][NV],
  int mind[NV] )

{
  int i;
  int i4_huge = 2147483647;

  for ( i = s; i <= e; i++ )
  {
    if ( !connected[i] )
    {
      if ( ohd[mv][i] < i4_huge )
      {
        if ( mind[mv] + ohd[mv][i] < mind[i] )  
        {
          mind[i] = mind[mv] + ohd[mv][i];
        }
      }
    }
  }
  return;
}