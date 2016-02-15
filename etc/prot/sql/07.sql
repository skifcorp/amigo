CREATE FUNCTION get_depth(params VARCHAR(255)) RETURNS INT NO SQL DETERMINISTIC
BEGIN
  DECLARE val_beg INT;
  DECLARE val_end INT;
  DECLARE ret INT;

  SET val_beg = LOCATE('depth=', params) ;
  IF val_beg=0 THEN
    SET ret = 300;
  ELSE
    SET val_beg = val_beg + 6;
    SET val_end = LOCATE(' ', params, val_beg);
    IF val_end = 0 THEN
      SET val_end = CHAR_LENGTH(params) + 1;
    END IF;

      SET ret = CONVERT(SUBSTR(params, val_beg, val_end - val_beg ), SIGNED );
  END IF;

  RETURN(ret);
END ;


