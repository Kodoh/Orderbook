#include <ctime>
#include <map>
#include <deque>
#include <iostream>

class Order {
public:
    enum Type { BUY, SELL };
    Type type;
    double price;
    int quantity;
    std::time_t timestamp;
    int orderId;
};

class OrderBook {
private:
    std::map<double, std::deque<Order>> bids; // Buy orders
    std::map<double, std::deque<Order>> asks; // Sell orders

public:
    void addOrder(const Order& order) {
        if (order.type == Order::BUY) {
            bids[order.price].push_back(order);
        } else {
            asks[order.price].push_back(order);
        }
        matchOrders();
    }

    void removeOrder(const Order& order) {
        auto& book = (order.type == Order::BUY) ? bids : asks;
        auto it = book.find(order.price);
        if (it != book.end()) {
            auto& orders = it->second;
            orders.erase(std::remove(orders.begin(), orders.end(), order), orders.end());
            if (orders.empty()) {
                book.erase(it);
            }
        }
    }

    void matchOrders() {
        while (!bids.empty() && !asks.empty()) {
            auto highestBid = bids.rbegin(); // Highest bid price
            auto lowestAsk = asks.begin();   // Lowest ask price

            if (highestBid->first >= lowestAsk->first) {
                auto& bidOrders = highestBid->second;
                auto& askOrders = lowestAsk->second;
                Order& bidOrder = bidOrders.front();
                Order& askOrder = askOrders.front();

                int tradeQuantity = std::min(bidOrder.quantity, askOrder.quantity);
                executeTrade(bidOrder, askOrder, tradeQuantity);

                bidOrder.quantity -= tradeQuantity;
                askOrder.quantity -= tradeQuantity;

                if (bidOrder.quantity == 0) bidOrders.pop_front();
                if (askOrder.quantity == 0) askOrders.pop_front();

                if (bidOrders.empty()) bids.erase(highestBid->first);
                if (askOrders.empty()) asks.erase(lowestAsk->first);
            } else {
                break;
            }
        }
    }

    void executeTrade(Order& bidOrder, Order& askOrder, int quantity) {
        // Update the order book and generate trade records
        std::cout << "Trade executed: " << quantity << " @ " << askOrder.price << std::endl;
    }
};