#ifndef __DIALOG_H_
#define __DIALOG_H_

#include "../../core/tag.h"

#include "ui_projman.h"

#include <QDialog>

class Dialog : public QDialog,
               public Ui::Dialog
               
{
  Q_OBJECT
public:
  Dialog(tags_t & , QWidget * parent = 0, Qt::WindowFlags f = 0 );
  ~Dialog();
public slots: 
  void onOpenProj();
  void onRunProj();
  void onStopProj();
  void onCloseProj();
  void onBtnConf();
  void onBtnExit();
  
private:
  tags_t & tags;
  
  void updateButtons();
  
/*  enum proj_state {
    proj_closed, proj_opened, proj_running, proj_stopped
  } _proj_state;*/
};


#endif
