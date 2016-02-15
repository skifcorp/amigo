CREATE PROCEDURE prepare_table ( IN var_name VARCHAR(100) ) MODIFIES SQL DATA
BEGIN
--  DECLARE prepared_name VARCHAR(30);
  DECLARE cur_num INT;
  DECLARE full_table_name VARCHAR(100);
  DECLARE rows_count INT;
  DECLARE scale_exists INT;
  DECLARE scale_table_name VARCHAR(100);

  SET scale_table_name = prepare_scale_name(var_name);

  SELECT count(*) FROM information_schema.TABLES WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME=scale_table_name INTO scale_exists;

  IF ( scale_exists = 0 ) THEN
    CALL create_scale_table (scale_table_name);
  END IF;

  SELECT MAX(STName) FROM FilesTable WHERE VName = var_name AND TEnd = 0 INTO cur_num;

  IF cur_num >= 0 THEN
--    SET full_table_name = CONCAT(prepared_name, '_', CONVERT (cur_num, CHAR) );
    SET full_table_name = prepare_table_name (var_name, cur_num );
    CALL get_rows_count (full_table_name, rows_count);
--    create table if not exists aaa (count INT);
--    insert into aaa values (rows_count);
    IF rows_count > 1000000 THEN
      call commit_filestable  ( var_name, cur_num );
      call add_new_table ( var_name, cur_num + 1, full_table_name );
    END IF;
  ELSE
    call add_new_table ( var_name, 0, full_table_name );
  END IF;

  SELECT full_table_name;
END;
