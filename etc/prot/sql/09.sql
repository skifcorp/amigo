CREATE FUNCTION get_var_name ( var_name VARCHAR (255) ) RETURNS VARCHAR(100) NO SQL DETERMINISTIC
BEGIN
  DECLARE slash_pos INT;
  SET slash_pos = LOCATE('/', var_name );
  IF slash_pos > 0 THEN
    SET var_name = RIGHT(var_name, CHAR_LENGTH(var_name) - slash_pos );
  END IF;

  RETURN (var_name);
END ;

