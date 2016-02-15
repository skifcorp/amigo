CREATE PROCEDURE check_depth () MODIFIES SQL DATA
BEGIN
  DECLARE var_name VARCHAR (255);
  DECLARE params VARCHAR (255);
  DECLARE conf_depth INT;
  DECLARE depth INT;
  DECLARE full_count INT;
  DECLARE done INT DEFAULT 0;
  DECLARE conf CURSOR FOR SELECT Names, Parametrs FROM Config_files.prot_conf WHERE NameDB=DATABASE();
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;
  
--  CREATE TABLE IF NOT EXISTS aaaa (ID INT);
  
  OPEN conf;

  REPEAT
    FETCH conf INTO var_name, params;
    IF NOT done THEN
      SET depth = get_depth(params)*86400; /*days to seconds*/
--      SET depth = get_depth(params);
      SET var_name = get_var_name (var_name);

      CALL get_full_rows_count (var_name, full_count);

      IF full_count > depth THEN
        CALL set_full_rows_count (var_name, full_count, depth);
      END IF;
    END IF;
  UNTIL done END REPEAT;

  CLOSE conf;

END ;





