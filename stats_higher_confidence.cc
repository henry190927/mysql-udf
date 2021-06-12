/* stats_higher_conficence function
 *
 * Description:
 *    Return the higher confidence x-value given data mean, size, sd and confidence level
 *
 * Usage:
 *   HIGHER_CONFIDENCE(X, probability)
 * 
 *   args[0]: input data
 *   args[1]: confidence probability / level (int or real)
 *
 * Return:
 *   higher confidence x-value : double (REAL)
 *  
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>
#include <vector>
#include <gsl/gsl_statistics.h>
#include "cmath"

using namespace std;

#define DECIMALS 6

extern "C" {
  bool stats_higher_confidence_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void stats_higher_confidence_deinit( UDF_INIT* initid );
  void stats_higher_confidence_clear( UDF_INIT* initid, char* is_null, char* error );
  void stats_higher_confidence_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void stats_higher_confidence_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double stats_higher_confidence( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
}

struct stats_confidence_data {
  vector<double> data;
};

bool stats_higher_confidence_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count != 2) {
    strcpy(message, "Wrong number of arguments: STATS_HIGHER_CONFIDENCE() requires four arguments");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    args->arg_type[0] = REAL_RESULT;
  }

  if(args->arg_type[1] != INT_RESULT && args->arg_type[0] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: STATS_HIGHER_CONFIDENCE() requires an integer or a real number as parameter 1");
    return 1;
  }

  initid->decimals = DECIMALS;
  initid->maybe_null    = 0; 
  initid->max_length    = 20;

  stats_confidence_data* buffer = new stats_confidence_data;
  buffer->data.clear();
  initid->ptr = (char*) buffer;
  
  return 0;
}

void stats_higher_confidence_deinit( UDF_INIT* initid )
{
  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  delete buffer;
}

void stats_higher_confidence_clear( UDF_INIT* initid, char* is_null, char* error )
{
  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  buffer->data.clear();
}

void stats_higher_confidence_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  stats_higher_confidence_clear( initid, is_null, error );
  stats_higher_confidence_add( initid, args, is_null, error );
}

void stats_higher_confidence_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  if (args->args[0] != NULL) {
    buffer->data.push_back(*(double*)args->args[0]);
  }
}

static double invnormalp(double prob_high_end)
{
  const double p0 = -0.322232431088;
  const double p1 = -1.0;
  const double p2 = -0.342242088547;
  const double p3 = -0.0204231210245;
  const double p4 = -0.453642210148E-4;
  const double q0 =  0.0993484626060;
  const double q1 =  0.588581570495;
  const double q2 =  0.531103462366;
  const double q3 =  0.103537752850;
  const double q4 =  0.38560700634E-2;
  
  double conf_out, y, xp;

  if (prob_high_end < 0.5)  
  {
    conf_out = prob_high_end;
  } else
  {
    conf_out = 1 - prob_high_end;
  }

  if (conf_out < 1E-12)
  {
    xp = 99;
  } else
  {
    y = sqrt(log(1/(conf_out*conf_out)));
    xp = y + ((((y * p4 + p3) * y + p2) * y + p1) * y + p0) /
             ((((y * q4 + q3) * y + q2) * y + q1) * y + q0);
  }

  if (prob_high_end < 0.5)  
  {
    return -xp;
  } else
  {
    return  xp;
  }
}

double stats_higher_confidence( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  double conf_prob;
  double sample_size;
  double mean;
  double std_dev;
  double t_dist;

  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  if(args->arg_type[1]==INT_RESULT) {
    conf_prob = (double) *((long long*) args->args[1]);
  } else if(args->arg_type[1]==REAL_RESULT) {
    conf_prob = (double) *((double*) args->args[1]);
  }

  double* values = buffer->data.data();
  sample_size = buffer->data.size();
  mean = gsl_stats_mean(values, 1, buffer->data.size());
  std_dev = gsl_stats_sd(values, 1, buffer->data.size());

  double prob_high_end = conf_prob * 0.5 + 0.5;
  t_dist = invnormalp(prob_high_end);
  if(t_dist < 0) {
    t_dist = -t_dist;
  }

  return 1.0 * mean + (std_dev * t_dist / sqrt(sample_size)); 
}