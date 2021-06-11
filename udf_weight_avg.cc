/*
  return the weighted average of values

  USAGE: 
    args[0]: data (real)
    args[1]: weight (real)

    output: weighted of the values (real)
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>
#include "cmath"

using namespace std;

#define DECIMALS 2

extern "C" {
  bool udf_weight_avg_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void udf_weight_avg_deinit( UDF_INIT* initid );
  void udf_weight_avg_clear( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void udf_weight_avg_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void udf_weight_avg_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double udf_weight_avg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
}

struct weight_avg_data {
  unsigned long count;
  double data_sum;
  double weight_sum;
};

bool udf_weight_avg_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count != 2) {
    strcpy(message, "Wrong number of arguments: UDF_WEIGHT_AVG() requires two arguments");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: UDF_WEIGHT_AVG() requires a real number as parameter 1");
    return 1;
  }

  if(args->arg_type[1] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: UDF_WEIGHT_AVG() requires a real number as parameter 2");
    return 1;
  }

  weight_avg_data* buffer = new weight_avg_data;
  buffer->count = 0;
  buffer->data_sum = 0.0;
  buffer->weight_sum = 0.0;

  initid->decimals = DECIMALS;
  initid->maybe_null	= 1;
  initid->max_length	= 32;
  initid->ptr = (char*) buffer;

  return 0;
}

void udf_weight_avg_deinit( UDF_INIT* initid )
{
  delete initid->ptr;
}

void udf_weight_avg_clear( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  weight_avg_data* buffer = (weight_avg_data*) initid->ptr;

  buffer->count = 0;
  buffer->data_sum = 0;
  buffer->weight_sum = 0;
  *is_null = 0;
  *error = 0;
}

void udf_weight_avg_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_weight_avg_clear( initid, args, is_null, error );
  udf_weight_avg_add( initid, args, is_null, error );
}

void udf_weight_avg_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{ 
  if(args->args[0] != NULL && args->args[1] != NULL) {
    weight_avg_data* buffer = (weight_avg_data*) initid->ptr;

    buffer->data_sum += ((double) *(double*) args->args[0]) * ((double) *(double*) args->args[1]);
    buffer->weight_sum += (double) *(double*) args->args[1];
    buffer->count ++;
  }
}

double udf_weight_avg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  weight_avg_data* buffer = (weight_avg_data*) initid->ptr;

  if (buffer->count == 0 || buffer->weight_sum == 0 || *error !=0) {
    *is_null = 1;
    return 0.0;
  }

  *is_null = 0;
  return (double) buffer->data_sum / buffer->weight_sum;
}
