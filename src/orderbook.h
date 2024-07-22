#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <chrono>
#include <map>
#include <limits>
#include <deque>
#include <algorithm>
#include <iostream>

static int sNextId = 0;

class Order {
public:
    enum Type { BUY, SELL };
    enum OrderType { MARKET, LIMIT, STOP };

    Type type;
    OrderType orderType { LIMIT };
    int quantity;
    double price { };
    double stopPrice { };
    uint64_t timestamp = timeSinceEpochMillisec();
    int orderId = sNextId++;

    bool operator==(const Order& other) const {
        return orderId == other.orderId;
    }

    static uint64_t timeSinceEpochMillisec() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
};

class OrderBook {
private:
    std::string allocation; // Allocation strategy: FIFO / PRORATA
    std::map<double, std::deque<Order>> bids; // Buy orders
    std::map<double, std::deque<Order>> asks; // Sell orders
    std::deque<Order> stopOrders; // Queue for stop orders

    // Private helper functions
    void matchOrdersFIFO(std::deque<Order>& bidOrders, std::deque<Order>& askOrders);
    void matchOrdersProRata(std::deque<Order>& bidOrders, std::deque<Order>& askOrders);
    void executeTrade(Order& bidOrder, Order& askOrder, int quantity);
    void checkStopOrders();



public:
    // Constructor
    OrderBook(const std::string& allocation);

    // Public member functions
    void addOrder(Order order);
    int getQuantity(const Order &order);
    void removeOrder(const Order& order);
    void matchOrders();
    double getLowestAskPrice() const;
    double getHighestBidPrice() const;
    void dispBids();
    void dispAsks();
    const std::map<double, std::deque<Order>>& getBids() const { return bids; }
    const std::map<double, std::deque<Order>>& getAsks() const { return asks; }
};

#endif 
