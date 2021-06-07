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
  bool udf_quantile_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void udf_quantile_sorted_deinit( UDF_INIT* initid );
  void udf_quantile_sorted_clear( UDF_INIT* initid, char* is_null, char* error );
  void udf_quantile_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void udf_quantile_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double udf_quantile_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
}

struct udf_quantile_sorted_data {
  unsigned long percentile;
  vector<double> data;
};

bool udf_quantile_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count < 1 || args->arg_count > 2) {
    strcpy(message, "Wrong number of arguments: UDF_QUANTILE_SORTED() requires one or two arguments");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    args->arg_type[0] = REAL_RESULT;
  }

  /* Default median percentage is 50% */
  unsigned long percentile = 50;
  if(args->arg_count==2 && (*((long*) args->args[1])>=0) && (*((long*) args->args[1])<=100)) {
    percentile = *((long*) args->args[1]);
  }

  initid->decimals = DECIMALS;

  udf_quantile_sorted_data* buffer = new udf_quantile_sorted_data;
  buffer->percentile = percentile;
  buffer->data.clear();

  initid->maybe_null = 1;
  initid->ptr = (char*) buffer;

  return 0;
}

void udf_quantile_sorted_deinit( UDF_INIT* initid )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;

  delete buffer;
}

void udf_quantile_sorted_clear( UDF_INIT* initid, char* is_null, char* error )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;
  
  buffer->data.clear();
}

void udf_quantile_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_quantile_sorted_clear(initid, is_null, error);
  udf_quantile_sorted_add(initid, args, is_null, error);
}

void udf_quantile_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;

  if(args->args[0] != NULL) {
    buffer->data.push_back(*(double*)args->args[0]);
  }
}

double udf_quantile_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;

  double* values = buffer->data.data();
  gsl_sort(values, 1, buffer->data.size());

  double quantile = gsl_stats_quantile_from_sorted_data(values, 1, buffer->data.size(), (double)buffer->percentile / 100.00);
  
  return quantile;
}

