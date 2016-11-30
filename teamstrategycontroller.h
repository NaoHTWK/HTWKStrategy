#ifndef TEAMSTRATEGYCONTROLLER_H
#define TEAMSTRATEGYCONTROLLER_H

#include <memory>

#include <nullmq.h>
#include <order.h>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include "qballwidget.h"
#include "qrobotwidget.h"
#include "qsoccerfieldwidget.h"
#include "soccerfield.h"

#define NUM_PLAYERS 5

class TeamStrategyController : public QObject {
  Q_OBJECT
 public:
  TeamStrategyController(std::string teamStrategyName, QWidget *window);
  void run();

 private slots:
  void runTeamStrategy();

 private:
  bool recvTeamStrategy(std::shared_ptr<Order> *ts, bool nonblock, uint8_t ownIdx);

  QRobotWidget *robot[NUM_PLAYERS];
  QCheckBox *penalized[NUM_PLAYERS];
  QCheckBox *alive[NUM_PLAYERS];
  QCheckBox *kickoff;
  QComboBox *gamestate;
  QComboBox *ballPrecision[NUM_PLAYERS];
  QLineEdit *teamBallQuali;
  QSoccerFieldWidget *sf;
  QBallWidget *ball;
  QGridLayout *outer;
  QGridLayout *right;
  QPushButton *run_ts;
  QLineEdit *dcm_time;
  nullMQ::socket_t gcInfoSocket;
  nullMQ::socket_t dcmTimeSocket;
  nullMQ::socket_t worldInfoSocket[NUM_PLAYERS];
  nullMQ::socket_t ownOrderSocket[NUM_PLAYERS];
  char* strategyBuffer;
  size_t strategyBufferSize;
};

#endif  // TEAMSTRATEGYCONTROLLER_H
