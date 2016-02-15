#include "dialog.h"

#include <QApplication>
#include <QtXml>
#include <QLibrary>
#include <QFile>
#include <QDir>
#include <QString>
#include <QPointer>

#include "../../core/tag.h"
#include "../../core/visiface.h"
#include "amigofs.h"

#include <functional>
#include <list>
#include <string>
#include <memory>

using std::function;
using std::list;
using std::string;
using std::shared_ptr;

#include <boost/bind.hpp>

using std::bind;

#if 0
typedef list<pair<QString, function<void()>> > StopFuncs;


void stop(QApplication& app, StopFuncs & ef)
{
  qDebug () <<"start stopping qtapp";

  for (auto p : ef ) {
    qDebug () << "stoppping: "<<p.first;
    p.second();
  }
  
  app.quit();
  
  qDebug () <<"stopping qtapp";
}


list<QString> parse_conf (const string& prj_path)
{
  QFile f( QString::fromStdString(prj_path) + QDir::separator()  + "qtapp.xml");
  
  QDomDocument doc;
  if ( !doc.setContent(&f) ) {
    qWarning() << "qtapp: setContent of: "<<f.fileName()<<" FAILED!";
    return list<QString>();
  }
  

  
  list<QString> mp;
  
  QDomNode n = doc.firstChild().firstChild();
  
  //qDebug() << "qtapp: "<<doc.nodeName()<<"n: "<<n.nodeName();
  
  while (!n.isNull()) {
    mp.push_back( n.nodeName() );
    qDebug () << "qtapp: parsing: "<< n.nodeName();
    n = n.nextSibling();
  }
  
  return std::move(mp);
}

typedef void (*vis_func_type)(tags_t& , function<void()>*, QApplication& );

extern "C" void executeModule( tags_t& tags, function<void()> stop_module_func )
{
  Q_ASSERT_X(false, __FUNC__, "you should not use this function");
#if 0
  qDebug () << "QtApp started";
  
  QApplication app( tags.argc(), tags.argv() );
 
  list<QString> modules_paths = parse_conf( tags.projectFolder() );
  list<shared_ptr<QLibrary>> libs;
  StopFuncs stop_funcs;
  
  for (auto p : modules_paths) {
    QString nam = QString::fromStdString (a::fs::lib_path()) + QDir::separator() + "qt"+ QDir::separator() + a::fs::lib_name(p);
    
    libs.push_back( shared_ptr<QLibrary>( new QLibrary(nam) ) );
    if (!libs.back()->load() ) {
      qWarning() << "qtapp: cant load library: "<<nam; continue;
    }
    
    stop_funcs.push_back( pair<QString, function<void()>>(nam, function<void()>() ) );
    
    vis_func_type f = reinterpret_cast<vis_func_type>(libs.back()->resolve("executeModule"));
    if (!f) {
      qWarning() << "qtapp: cant resolve func executeModule for lib: "<<p; libs.back()->unload(); continue;
    }
    f(tags, &stop_funcs.back().second, app);
  }
  
  stop_module_func = bind (stop, std::ref(app), std::ref(stop_funcs));
  
  app.exec();
#endif

  
}

#endif


class QtApp : public VisIface 
{
public:
  typedef void (*vis_func_type)(tags_t& , function<void()>*, QApplication& );
  typedef list<pair<QString, function<void()>> > StopFuncs;
  
  QtApp(tags_t & tgs) : tags(tgs), app(tags.argc(), tags.argv()), exec(false)
  {
    //app.exec();
  }

  ~QtApp()
  {
    stop();
  }
  
  virtual void showProjWindow()
  {
    if (dialog) {
      dialog->activateWindow(); return;
    }
    
    dialog = new Dialog(tags);
    dialog->show();
  }
  
  virtual void hideProjWindow()
  {
    delete dialog;
  }
  
  
  virtual void executeModule()
  {
    modules_paths = parse_conf( tags.projectFolder() );
  
    for (auto p : modules_paths) {
      QString nam = QString::fromStdString (a::fs::lib_path()) + QDir::separator() + "qt"+ QDir::separator() + a::fs::lib_name(p);
    
      libs.push_back( shared_ptr<QLibrary>( new QLibrary(nam) ) );
      if (!libs.back()->load() ) {
        qWarning() << "qtapp: cant load library: "<<nam; continue;
      }
    
      stop_funcs.push_back( pair<QString, function<void()>>(nam, function<void()>() ) );
     
      vis_func_type f = reinterpret_cast<vis_func_type>(libs.back()->resolve("executeModule"));
      if (!f) {
        qWarning() << "qtapp: cant resolve func executeModule for lib: "<<p; libs.back()->unload(); continue;
      }
      f(tags, &stop_funcs.back().second, app);
    }
  }
  
  virtual void start ()
  {
    if (!exec) {
      app.exec();
      exec = true;
    }
  }
  
  virtual void stop()
  {
    qDebug () <<"start stopping qtapp";

    for (auto p : stop_funcs ) {
      qDebug () << "stopping: "<<p.first;
      p.second();
    }
    app.quit();
//    app.quit();
  
//    qDebug () <<"stopping qtapp";
  }
private:

  tags_t & tags;
  QApplication app;
  QPointer<Dialog> dialog;
  bool exec;
  
  list<QString> modules_paths;
  list<shared_ptr<QLibrary>> libs;
  StopFuncs stop_funcs;
  
  list<QString> parse_conf (const string& prj_path)
  {
    QFile f( QString::fromStdString(prj_path) + QDir::separator()  + "qtapp.xml" );
  
    QDomDocument doc;
    if ( !doc.setContent(&f) ) {
      qWarning() << "qtapp: setContent of: "<<f.fileName()<<" FAILED!";
      return list<QString>();
    }

    list<QString> mp;
  
    QDomNode n = doc.firstChild().firstChild();
    
    while (!n.isNull()) {
      mp.push_back( n.nodeName() );
      qDebug () << "qtapp: parsing: "<< n.nodeName();
      n = n.nextSibling();
    }
  
    return std::move(mp);
  }
};



extern "C" shared_ptr<VisIface> executeModule(tags_t & tags)
{
  qDebug () << "visIface started";
  
  return shared_ptr<VisIface>( new QtApp (tags) );
}


