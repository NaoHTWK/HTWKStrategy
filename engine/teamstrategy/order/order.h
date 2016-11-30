#ifndef _ORDER_H_
#define _ORDER_H_

#include <string>
#include <memory>

#include <worldmodel.pb.h>

class Order {
private:
    const std::string name;

public:
    explicit Order(const char* name) : name(name) {}
    virtual ~Order(){}

    Order(const Order& cpy) = delete;
    virtual Order& operator=(const Order& s) = delete;

    virtual const std::string& getClassName() const { return name; }

    virtual void toProtobuf(protobuf::worldmodel::Order* order) {
        order->set_name(getClassName());
    }
};

typedef std::shared_ptr<Order> OrderPtr;

#endif /* _STRATEGY_H_ */
