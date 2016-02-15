CREATE PROCEDURE delete_table ( IN prepared_table_name VARCHAR(100) )
BEGIN
  SET @sql = CONCAT('DROP TABLE ', prepared_table_name);
  PREPARE q FROM @sql;
  EXECUTE q;
  DEALLOCATE PREPARE q;
END ;
