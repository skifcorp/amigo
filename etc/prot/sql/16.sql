CREATE FUNCTION prepare_scale_name ( var_name VARCHAR(100) ) RETURNS VARCHAR (100) NO SQL
BEGIN
  RETURN CONCAT( 'Scale_', REPLACE(var_name, '.', '$') );
END ;