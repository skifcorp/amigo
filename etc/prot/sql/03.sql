CREATE PROCEDURE add_new_table (IN var_name VARCHAR(100), IN cur_num INT, INOUT full_table_name VARCHAR(100)) MODIFIES SQL DATA
BEGIN
  DECLARE prepared_name VARCHAR(100);

  SET prepared_name = REPLACE(var_name, '.', '$');
  SET full_table_name = CONCAT(prepared_name, '_', CONVERT(cur_num, CHAR) );

  SET @sql = CONCAT('CREATE TABLE ', full_table_name, ' ( Time TIMESTAMP PRIMARY KEY  DEFAULT NOW(), IsDst TINYINT, Value FLOAT, NScale SMALLINT UNSIGNED ) ' );
  PREPARE cnt_query FROM @sql;
  EXECUTE cnt_query;
  DEALLOCATE PREPARE cnt_query;

  SET @gl_var_name = var_name;
  SET @gl_cur_num  = cur_num;
  SET @cur_date_time = UTC_TIMESTAMP();
  PREPARE cnt_query1 FROM 'INSERT INTO FilesTable (VName, STName, TBegin, TEnd ) VALUES (?,?,?, 0)';
  EXECUTE cnt_query1 USING @gl_var_name, @gl_cur_num, @cur_date_time;
  DEALLOCATE PREPARE cnt_query1;
END;
