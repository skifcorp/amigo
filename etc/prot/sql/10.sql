CREATE FUNCTION prepare_table_name (var_name VARCHAR (100), num INT) RETURNS VARCHAR(100) NO SQL
BEGIN
  RETURN CONCAT( REPLACE (var_name, '.', '$'), '_', CONVERT(num, CHAR) );
END ;
