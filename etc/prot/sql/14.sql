CREATE PROCEDURE delete_rows ( IN prepared_table_name VARCHAR(100), IN num_rows INT )
BEGIN
  SET @sql = CONCAT('DELETE FROM ', prepared_table_name, ' ORDER BY Time LIMIT ', CONVERT(num_rows, CHAR) );

  PREPARE q FROM @sql;
  EXECUTE q;
  DEALLOCATE PREPARE q;
END ;


