CREATE AGGREGATE FUNCTION stats_medain RETURNS REAL SONAME 'stats.so';
CREATE AGGREGATE FUNCTION stats_median_sorted RETURNS REAL SONAME 'stats.so';
CREATE AGGREGATE FUNCTION stats_quantile_sorted RETURNS REAL SONAME 'stats.so';
CREATE AGGREGATE FUNCTION stats_weight_avg RETURNS REAL SONAME 'state.so';
CREATE FUNCTION stats_higher_confidence RETURNS REAL SONAME 'stats.so';
CREATE FUNCTION stats_lower_confidence RETURNS REAL SONAME 'stats.so';
