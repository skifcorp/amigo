CREATE PROCEDURE update_scale_table ( IN var_name VARCHAR(100), IN min_datetime TIMESTAMP  ) MODIFIES SQL DATA
BEGIN

  DECLARE scale_name VARCHAR (100);
  DECLARE min_scale_time TIMESTAMP;

  SET scale_name = prepare_scale_name(var_name);

  SET @sql = CONCAT('CREATE TEMPORARY TABLE min_scale_time ENGINE=MEMORY SELECT MAX(Time) FROM ', scale_name, ' WHERE Time < "', CONVERT(min_datetime, CHAR), '"' );

  PREPARE q FROM @sql;
  EXECUTE q;
  DEALLOCATE PREPARE q;

  SELECT * from min_scale_time INTO min_scale_time;

  DROP TABLE min_scale_time;

  SET @sql1 = CONCAT('DELETE FROM ', scale_name , ' WHERE Time < "', CONVERT(min_scale_time, CHAR), '"' );
  PREPARE q1 FROM @sql1;
  EXECUTE q1;
  DEALLOCATE PREPARE q1;

  SET @sql2 = CONCAT('UPDATE ', scale_name, ' SET Time = "', CONVERT (min_datetime, CHAR) , '" WHERE Time = "', CONVERT(min_scale_time, CHAR), '"' );
  PREPARE q2 FROM @sql2;
  EXECUTE q2;
  DEALLOCATE PREPARE q2;
END ;

