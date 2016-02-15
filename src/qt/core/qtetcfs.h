#ifndef __QTETCFS_H_
#define __QTETCFS_H_

#include "amigofs.h"
#include <QString>
#include <QDir>

namespace a {
  namespace fs {
    inline QString etcPath()
    {
      return QString::fromStdString( etc_path() );
    }
    inline QString langPath()
    {
      //return QString::fromStdString( (initial_path().parent_path() / "etc" / "lang").string() );
      return etcPath() + QDir::separator() + "lang";
    }
    inline QString visualPath()
    {
      return etcPath() + QDir::separator() + "visual";
    }
    inline QString visPredefPwPath()
    {
      return visualPath() + QDir::separator() + "predefpws";
    }
    inline QString visPictures()
    {
      return visualPath() + QDir::separator() + "pict";
    }
  }
};


#endif
