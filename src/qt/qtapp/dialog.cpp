#include "dialog.h"
#include <QDebug>
#include <QFileDialog>


Dialog::Dialog(tags_t & t, QWidget * parent, Qt::WindowFlags f ) : QDialog(parent, f), tags(t)
{
  setupUi(this);

  connect(btnOpen , SIGNAL(clicked()), this, SLOT(onOpenProj()) );
  connect(btnRun  , SIGNAL(clicked()), this, SLOT(onRunProj())  );
  connect(btnStop , SIGNAL(clicked()), this, SLOT(onStopProj()) );
  connect(btnClose, SIGNAL(clicked()), this, SLOT(onCloseProj()));
  connect(btnConf , SIGNAL(clicked()), this, SLOT(onBtnConf())  );
  connect(btnExit , SIGNAL(clicked()), this, SLOT(onBtnExit())  ); 
  
  updateButtons();
  
}

Dialog::~Dialog()
{
  
}

void Dialog::updateButtons()
{
  btnOpen->setEnabled(tags.projState()==tags_t::proj_closed);
  btnRun->setEnabled(tags.projState()==tags_t::proj_opened || tags.projState()==tags_t::proj_stopped);
  btnStop->setEnabled(tags.projState()==tags_t::proj_running);
  btnClose->setEnabled(tags.projState()==tags_t::proj_stopped || tags.projState()==tags_t::proj_opened);
}

void Dialog::onOpenProj()
{
  
  QString path = QFileDialog::getExistingDirectory(this, "Select project folder");
  if ( !path.isEmpty() ) {
    tags.setProjectFolder( (path + QDir::separator()).toStdString() );
  }
  
  updateButtons();
}


void Dialog::onRunProj()
{
  tags.run();
  updateButtons();
}

void Dialog::onStopProj()
{
  tags.stop();
  updateButtons();
}

void Dialog::onCloseProj()
{
  tags.closeProject();
  updateButtons();
}

void Dialog::onBtnConf()
{
  //hide();
}

void Dialog::onBtnExit()
{
//  tags.stop();
//  qApp->exit(); //????
}

