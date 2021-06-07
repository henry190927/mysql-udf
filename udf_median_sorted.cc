/*
    
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>

#include <vector>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_sort.h>

using namespace std;

#define DECIMALS 2

extern "C" {
  bool udf_median_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void udf_median_sorted_deinit( UDF_INIT* initid );
  void udf_median_sorted_clear( UDF_INIT* initid, char* is_null, char* error );
  void udf_median_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void udf_median_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double udf_median_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error  );
}

struct udf_median_sorted_data {
  vector<double> data;
};


bool udf_median_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count != 1) {
    strcpy(message, "Wrong number of arguments: UDF_MEDIAN_SORTED() requires one argument");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    args->arg_type[0] = REAL_RESULT;
  }

  initid->decimals = DECIMALS;
  
  udf_median_sorted_data* buffer = new udf_median_sorted_data;
  buffer->data.clear();

  initid->maybe_null = 1;
  initid->ptr = (char*) buffer;

  return 0;
}

void udf_median_sorted_deinit( UDF_INIT* initid )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;

  delete buffer;
}

void udf_median_sorted_clear( UDF_INIT* initid, char* is_null, char* error )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;
  
  buffer->data.clear();
}

void udf_median_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_median_sorted_clear(initid, is_null, error);
  udf_median_sorted_add( initid, args, is_null, error );
}

void udf_median_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;

  if(args->args[0] != NULL){
    buffer->data.push_back(*(double*)args->args[0]);
  }
}

double udf_median_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error  )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;

  double* values = buffer->data.data();
  gsl_sort(values, 1, buffer->data.size());

  double median = gsl_stats_median(values, 1, buffer->data.size());

  return median;
}