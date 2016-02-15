#ifndef __VIS_FS_H_
#define __VIS_FS_H_

#include <QString>
#include <QDir>
#include <string>
//#include "amigofs.h"

using std::string;

class QtProjFs
{
public:
  QtProjFs (const string& bp ) : base_path(bp)
  {
    initFs();
  }
  
  QtProjFs (const QtProjFs& ) = delete;
  
  ~QtProjFs()
  {
  }
  
  QString vis() const
  {
    return QString::fromStdString(base_path) + QDir::separator() + "visual" + QDir::separator();
  }
  QString visPictures() const 
  {
    return  vis() + "Pictures" + QDir::separator();
  }
  QString visXml() const 
  {
    return  vis() + "xml" + QDir::separator();
  }
private:
  const string& base_path;
  void initFs() const 
  {
    QDir base_dir(QString::fromStdString(base_path));
    
    if (!base_dir.exists("visual")) {
//qDebug () << "visual not exists\n";
      base_dir.mkdir("visual");
    }
    else {
///qDebug () << "visual exists exists\n";      
    }
    
    base_dir.cd("visual");
    if (!base_dir.exists("Pictures"))
      base_dir.mkdir("Pictures");

    if (!base_dir.exists("xml"))
      base_dir.mkdir("xml");
  }
};

#endif
