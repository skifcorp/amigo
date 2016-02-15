
CREATE PROCEDURE commit_filestable ( IN var_name VARCHAR(100), IN cur_num INT  ) MODIFIES SQL DATA
BEGIN
  UPDATE FilesTable SET TEnd = UTC_TIMESTAMP() WHERE VName = var_name AND STName = cur_num;
END;

