CREATE PROCEDURE get_min_datetime(IN prepared_table_name VARCHAR(100), OUT min_datetime TIMESTAMP ) MODIFIES SQL DATA
BEGIN
  SET @sql1 = CONCAT('CREATE TEMPORARY TABLE min_date_time ENGINE=MEMORY SELECT MIN(Time) FROM ', prepared_table_name );
  PREPARE q1 FROM @sql1;
  EXECUTE q1;
  DEALLOCATE PREPARE q1;

  SELECT * FROM min_date_time INTO min_datetime;

  DROP TABLE min_date_time;
END ;
