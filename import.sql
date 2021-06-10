CREATE AGGREGATE FUNCTION udf_medain RETURNS REAL SONAME 'udf_stats.so';
CREATE AGGREGATE FUNCTION udf_median_sorted RETURNS REAL SONAME 'udf_stats.so';
CREATE AGGREGATE FUNCTION udf_quantile_sorted RETURNS REAL SONAME 'udf_stats.so';
CREATE FUNCTION udf_higher_confidence RETURNS REAL SONAME 'udf_stats.so';
CREATE FUNCTION udf_lower_confidence RETURNS REAL SONAME 'udf_stats.so';
