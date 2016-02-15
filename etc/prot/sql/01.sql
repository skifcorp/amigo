CREATE PROCEDURE get_rows_count (IN full_table_name VARCHAR(100), OUT rows_count INT ) MODIFIES SQL DATA
BEGIN
  SET @sql = CONCAT( 'CREATE TEMPORARY TABLE count_tbl ENGINE=MEMORY SELECT count(*) FROM ', full_table_name );

  PREPARE cnt_query FROM @sql;
  EXECUTE cnt_query;

  SELECT * FROM count_tbl INTO rows_count;

  DEALLOCATE PREPARE cnt_query;
  DROP TEMPORARY TABLE IF EXISTS count_tbl;
END;

