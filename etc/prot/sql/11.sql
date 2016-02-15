CREATE PROCEDURE set_full_rows_count (IN var_name VARCHAR(100), IN full_count INT, IN count INT ) MODIFIES SQL DATA
BEGIN
  DECLARE done INT DEFAULT 0;
  DECLARE delta INT;
  DECLARE cur_num INT;
  DECLARE rows_count INT;
  DECLARE prepared_table_name VARCHAR(100);
  DECLARE min_time TIMESTAMP;

  DECLARE cursor_var_tables CURSOR FOR SELECT STName FROM FilesTable WHERE VName = var_name ORDER BY TBegin;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 0;

  SET delta = full_count - count;

  OPEN cursor_var_tables;

  REPEAT
    FETCH cursor_var_tables INTO cur_num;
    IF NOT done THEN
      SET prepared_table_name = prepare_table_name (var_name, cur_num);
      CALL get_rows_count ( prepared_table_name, rows_count );

      IF rows_count <= delta THEN
        CALL delete_table (prepared_table_name);
        SET delta = delta - rows_count;

        DELETE FROM FilesTable WHERE VName = var_name AND STName = cur_num;
        IF delta = 0 THEN 
          -- getting next table_name
          FETCH cursor_var_tables INTO cur_num;
          IF NOT done THEN
            SET prepared_table_name = prepare_table_name (var_name, cur_num);
            CALL get_min_datetime (prepared_table_name, min_time);
            CALL update_scale_table (var_name, min_time);
          ELSE 
            -- if we here it means that all tables with data for this variable deleted so we delete scale_table_also
            CALL delete_table (prepare_scale_name (var_name));
          END IF;
        END IF;
      ELSE
        CALL delete_rows (prepared_table_name, delta);
        SET delta = 0;
        SELECT MIN(STName) FROM FilesTable WHERE VName = var_name INTO cur_num;
        CALL get_min_datetime (prepated_table_name, min_time);
        UPDATE FilesTable SET TBegin = min_time WHERE VName = var_name AND STName = cur_num;
        CALL update_scale_table (var_name, min_time);
      END IF;
    END IF;
  UNTIL done OR delta <= 0  END REPEAT;

  CLOSE cursor_var_tables;
END ;
