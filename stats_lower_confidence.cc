/* stats_lower_conficence function
 *
 * Description:
 *    Return the lower confidence x-value given data mean, size, sd and confidence level
 *
 * Usage:
 *   LOWER_CONFIDENCE(probability, size, mean, sd)
 * 
 *   args[0]: confidence probability / level (int or real)
 *   args[1]: sample size (int)
 *   args[2]: arithmetic mean (int or real)
 *   args[3]: standard deviation (int or real)
 *
 * Return:
 *   lower confidence x-value : double (REAL)
 *  
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>
#include "cmath"

using namespace std;

#define DECIMALS 2

extern "C" {
  bool stats_lower_confidence_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  double stats_lower_confidence(UDF_INIT* init, UDF_ARGS* args, char* is_null, char* error );
}

bool stats_lower_confidence_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(!args->arg_count != 4) {
    strcpy(message, "Wrong number of arguments: STATS_LOWER_CONFIDENCE() requires four arguments");
    return 1;
  }

  if(args->arg_type[0] != INT_RESULT && args->arg_type[0] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: STATS_LOWER_CONFIDENCE() requires an integer or a real number as parameter 1");
    return 1;
  }

  if(args->arg_type[1] != INT_RESULT) {
    strcpy(message, "Wrong type of arguments: STATS_LOWER_CONFIDENCE() requires an interger as parameter 2");
    return 1;
  }

  if(*((long*)args->args[1]) < 1) {
    strcpy(message, "Wrong value of arguments: STATS_LOWER_CONFIDENCE() requires parameter 2 (sample size) at least 1");
    return 1;
  }

  if(args->arg_type[2] != INT_RESULT && args->arg_type[2] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: STATS_LOWER_CONFIDENCE() requires an integer or a real number as parameter 3");
    return 1;
  }

  if(args->arg_type[3] != INT_RESULT && args->arg_type[3] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: STATS_LOWER_CONFIDENCE() requires an integer or a real number as parameter 4");
    return 1;
  }

  initid->decimals = DECIMALS;
  initid->maybe_null    = 0; 
  initid->max_length    = 20;
  
  return 0;
}

static double invnormalp(double prob_low_end)
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

  if (prob_low_end < 0.5)  
  {
    conf_out = prob_low_end;
  } else
  {
    conf_out = 1 - prob_low_end;
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

  if (prob_low_end < 0.5)  
  {
    return -xp;
  } else
  {
    return  xp;
  }
}

double stats_lower_confidence(UDF_INIT* init, UDF_ARGS* args, char* is_null, char* error )
{
  double conf_prob;
  double sample_size;
  double mean;
  double std_dev;
  double t_dist;

  if(args->arg_type[0]==INT_RESULT) {
    conf_prob = (double) *((long long*) args->args[0]);
  } else if(args->arg_type[0]==REAL_RESULT) {
    conf_prob = (double) *((double*) args->args[0]);
  }

  if (args->arg_type[1]==INT_RESULT)
  {
    sample_size = (double) *((long long*) args->args[1]);
  } else if (args->arg_type[1]==REAL_RESULT)
  {
    sample_size = (double) *((double*) args->args[1]);
  }

  if (args->arg_type[2]==INT_RESULT)
  {
    mean = (double) *((long long*) args->args[2]);
  } else if (args->arg_type[2]==REAL_RESULT)
  {
    mean = (double) *((double*) args->args[2]);
  }

  if (args->arg_type[3]==INT_RESULT)
  {
    std_dev = (double) *((long long*) args->args[3]);
  } else if (args->arg_type[3]==REAL_RESULT)
  {
    std_dev = (double) *((double*) args->args[3]);
  }

  double prob_low_end = conf_prob * 0.5 + 0.5;
  t_dist = invnormalp(prob_low_end);
  if(t_dist < 0) {
    t_dist = -t_dist;
  }

  return 1.0 * mean - (std_dev * t_dist / sqrt(sample_size));
}