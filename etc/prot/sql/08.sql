CREATE PROCEDURE get_full_rows_count (IN var_name VARCHAR(100), OUT rows_count INT )
BEGIN
  DECLARE num INT;
  DECLARE cur_num INT;
  DECLARE done INT DEFAULT 0;
  DECLARE prepared_name VARCHAR(100);
  DECLARE cursor_var_tables CURSOR FOR SELECT STName FROM FilesTable WHERE VName = var_name;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;

  SET rows_count = 0;


  OPEN cursor_var_tables;

  REPEAT
    FETCH cursor_var_tables INTO cur_num;
    IF NOT done THEN
      SET prepared_name = prepare_table_name(var_name, cur_num);
      CALL get_rows_count (prepared_name, num);
      SET rows_count = rows_count + num;
    END IF;
  UNTIL done END REPEAT;

  CLOSE cursor_var_tables;
END ;