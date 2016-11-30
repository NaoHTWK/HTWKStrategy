#include "teamstrategycontroller.h"

#include <dcm.h>
#include <envinfo.h>
#include <gamecontroller_client.h>
#include <iostream>
#include <orderfactory.h>
#include <QIntValidator>
#include <QDoubleValidator>
#include <teamstrategybuilder.h>
#include <worldmodelsocket.h>
#include <worldmodel.pb.h>

namespace wm = protobuf::worldmodel;

TeamStrategyController::TeamStrategyController(std::string teamStrategyName, QWidget *window) {
  outer = new QGridLayout(window);
  outer->setContentsMargins(0, 0, 0, 0);
  outer->setSpacing(0);
  right = new QGridLayout;
  outer->addLayout(right, 0, 1, Qt::AlignTop | Qt::AlignLeft);
  run_ts = new QPushButton("Run Team Strategy");
  right->addWidget(run_ts, 0, 0, 1, 2);
  connect(run_ts, SIGNAL(released()), this, SLOT(runTeamStrategy()));
  dcm_time = new QLineEdit("0");
  dcm_time->setValidator(new QIntValidator(dcm_time));
  right->addWidget(new QLabel("DCM-Time"));
  right->addWidget(dcm_time, 1, 1);

  sf = new QSoccerFieldWidget;
  outer->addWidget(sf, 0, 0, Qt::AlignTop | Qt::AlignLeft);
  ball = new QBallWidget(sf);
  connect(ball, SIGNAL(moved()), this, SLOT(runTeamStrategy()));
  gamestate = new QComboBox(window);
  gamestate->addItems(QStringList({QString("INITIAL"),QString("READY"),
                                   QString("SET"),QString("PLAY"),QString("FINISHED")}));
  right->addWidget(gamestate, 2, 0, 1, 2);

  TeamStrategyBuilder teamStrategyBuilder;

  for (int i = 0; i < NUM_PLAYERS; i++) {
    robot[i] = new QRobotWidget(i + 1, sf);
    connect(robot[i], SIGNAL(moved()), this, SLOT(runTeamStrategy()));
    right->addWidget(new QLabel(QString::fromStdString("Robot " + std::to_string(i+1))), i*3+3,0,1,2);
    penalized[i] = new QCheckBox(QString("penalized"), window);
    right->addWidget(penalized[i], i*3 + 4, 0);
    alive[i] = new QCheckBox(QString("alive"), window);
    alive[i]->setCheckState(Qt::Checked);
    right->addWidget(alive[i], i*3+4, 1);
    ballPrecision[i] = new QComboBox();
    ballPrecision[i]->addItems({"NONE", "LONG_INTERPOLATION", "INTERPOLATION", "NORMAL",
                                "FOOT_INTERPOLATION", "FOOT_MEASUREMENT"});
    ballPrecision[i]->setCurrentIndex(2);
    right->addWidget(new QLabel("Ball precision"), 3 * i + 5, 0);
    right->addWidget(ballPrecision[i], 3 * i + 5, 1);

    teamStrategyBuilder.startAsThread(teamStrategyName, 13, i);
    worldInfoSocket[i] = nullMQ::connect(
          WorldModelSocket::WORLD_INFO_SOCKET_NAME + std::to_string(i),sizeof(WorldInfo));
    ownOrderSocket[i] = nullMQ::connect("ownorder" + std::to_string(i), sysconf(_SC_PAGESIZE));
  }
  kickoff = new QCheckBox(QString("kickoff"));
  right->addWidget(kickoff, 3 * NUM_PLAYERS + 6, 0, 1, 2);
  teamBallQuali = new QLineEdit("0.8");
  teamBallQuali->setValidator(new QDoubleValidator(teamBallQuali));
  right->addWidget(new QLabel("Team ball quali"), 3 * NUM_PLAYERS + 7, 0);
  right->addWidget(teamBallQuali, 3 * NUM_PLAYERS + 7, 1);

  gcInfoSocket = nullMQ::connect(Gamecontroller::GC_INFO_SOCK_NAME, sizeof(GameInfo));
  dcmTimeSocket = nullMQ::connect(DCM_HEARTBEAT_NMQ_SOCK_NAME, sizeof(naohtwk_timestamp_t));

  if(posix_memalign((void**)&strategyBuffer, sysconf(_SC_PAGESIZE), sysconf(_SC_PAGESIZE))) {
    fprintf(stderr, "AgentCommunication::AgentCommunication posix_memalign failed\n");
    exit(1);
  }

  strategyBufferSize = sysconf(_SC_PAGESIZE);
}

#define LENGTH_PREFIX sizeof(size_t)
bool TeamStrategyController::recvTeamStrategy(std::shared_ptr<Order> *ts, bool nonblock, uint8_t ownIdx) {

  memset(strategyBuffer, 0, sysconf(_SC_PAGESIZE));
  bool newData = ownOrderSocket[ownIdx].recv(strategyBuffer, nonblock);

  if(newData == false)
    return newData;

  size_t bufSize;
  memcpy(&bufSize, strategyBuffer, LENGTH_PREFIX);

  if(bufSize > strategyBufferSize - LENGTH_PREFIX) {
    fprintf(stderr, "Received Order size is unplausible! %s %d\n", __FILE__, __LINE__);
    return false;
  }

  wm::Order wmOrder;
  if(wmOrder.ParseFromArray(strategyBuffer + LENGTH_PREFIX, bufSize) == false) {
    fprintf(stderr, "Received Order ProtoBuf returned false! %s %d\n", __FILE__, __LINE__);
    return false;
  }

  ts->reset(OrderFactory::createOrder(wmOrder));
  return newData;
}

void TeamStrategyController::runTeamStrategy() {
  GameInfo game_info;
  game_info.gamestate = (Gamecontroller::GameState)gamestate->currentIndex();
  for (int i = 0; i < NUM_PLAYERS; i++) {
    game_info.isPenalizedOwn[i] = penalized[i]->isChecked();
  }
  game_info.kickoffTeam = kickoff->isChecked() ? 13 : 10;
  gcInfoSocket.send(&game_info);

  naohtwk_timestamp_t time;
  time = dcm_time->text().toInt();
  dcmTimeSocket.send(&time);

  for(int i = 0; i < NUM_PLAYERS; i++) {
    WorldInfo world;
    for (int j = 0; j < NUM_PLAYERS; j++) {
      world.robots[j].jerseyNr = j+1;
      world.robots[j].is_alive = alive[j]->isChecked();
      world.robots[j].pos = robot[j]->getPos();
      world.robots[j].ball.precision = (ball_precision)ballPrecision[j]->currentIndex();
    }
    world.ownIdx = i;
    //world.robots[world.ownIdx].ball.pos = ball->getPos();
    world.teamBallQuali = teamBallQuali->text().toFloat();
    world.teamBall = ball->getPos();
    worldInfoSocket[i].send(&world);
  }
  for(int i = 0; i < NUM_PLAYERS; i++) {
    std::shared_ptr<Order> order;
    int sl = 0;
    while(!recvTeamStrategy(&order, true, i)) {
      usleep(10000);
      sl++;
      if (sl == 25) {
        std::cerr<<"Team strategy timeout!\n";
        break;
      }
    }
    if (sl == 25) continue;
    auto p = robot[i]->getPos();
    sf->setOrder(i, p, ball->getPos(), order);
  }
}
