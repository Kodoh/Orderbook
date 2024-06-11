#include <chrono> // milliseconds better than seconds 
#include <map>
#include <limits.h>
#include <stdio.h>
#include <deque>
#include <iostream>
#include <algorithm> // for std::remove

static int sNextId = 0;

class Order {
public:
    enum Type { BUY, SELL };
    enum OrderType { MARKET, LIMIT, STOP};
    Type type;
    OrderType orderType { LIMIT };
    int quantity;
    double price { };
    double stopPrice { };
    uint64_t timestamp = timeSinceEpochMillisec();
    int orderId = sNextId++;
    bool operator==(const Order& other) const {
        return orderId == other.orderId; // for std::remove (using ID as comparator)
    }

    static uint64_t timeSinceEpochMillisec() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
};

class OrderBook {
private:
    std::string allocation; // FIFO / PRORATA
    std::map<double, std::deque<Order>> bids; // Buy orders
    std::map<double, std::deque<Order>> asks; // Sell orders
    std::deque<Order> stopOrders; // Queue for stop orders

public:
    OrderBook(const std::string& allocation) : allocation(allocation) {}

    void addOrder(Order order) {
        if (order.orderType == Order::STOP) {
            stopOrders.push_back(order);
        }
        if (order.type == Order::BUY && order.orderType == Order::MARKET) {
            order.price = (!asks.empty()) ? asks.begin()->first : std::numeric_limits<double>::max();
        } else if (order.type == Order::SELL && order.orderType == Order::MARKET) {
            order.price = (!bids.empty()) ? bids.rbegin()->first : std::numeric_limits<double>::lowest();
        } 
        if (order.type == Order::BUY) {
            bids[order.price].push_back(order);
        } else {
            asks[order.price].push_back(order);
        }
        checkStopOrders();
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

    void checkStopOrders() {
        for (auto it = stopOrders.begin(); it != stopOrders.end();) {
            if ((it->type == Order::BUY && it->stopPrice <= getLowestAskPrice()) ||
                (it->type == Order::SELL && it->stopPrice >= getHighestBidPrice())) {
                it->orderType = Order::MARKET;
                addOrder(*it);
                it = stopOrders.erase(it);
            } else {
                ++it;
            }
        }
    }

    double getLowestAskPrice() const {
        return asks.empty() ? std::numeric_limits<double>::max() : asks.begin()->first;
    }

    double getHighestBidPrice() const {
        return bids.empty() ? 0.0 : bids.rbegin()->first;
    }

    void matchOrders() {
        while (!bids.empty() && !asks.empty()) {
            auto highestBid = bids.rbegin(); // Highest bid price
            auto lowestAsk = asks.begin();   // Lowest ask price

            if (highestBid->first >= lowestAsk->first) {
                if (allocation == "PRORATA") {
                    matchOrdersProRata(highestBid->second, lowestAsk->second);
                } else {
                    matchOrdersFIFO(highestBid->second, lowestAsk->second);
                }

                if (highestBid->second.empty()) bids.erase(highestBid->first);
                if (lowestAsk->second.empty()) asks.erase(lowestAsk->first);
            } else {
                break;
            }
        }
    }

    void matchOrdersFIFO(std::deque<Order>& bidOrders, std::deque<Order>& askOrders) {
        while (!bidOrders.empty() && !askOrders.empty()) {
            Order& bidOrder = bidOrders.front();
            Order& askOrder = askOrders.front();

            int tradeQuantity = std::min(bidOrder.quantity, askOrder.quantity);
            executeTrade(bidOrder, askOrder, tradeQuantity);

            bidOrder.quantity -= tradeQuantity;
            askOrder.quantity -= tradeQuantity;

            if (bidOrder.quantity == 0) bidOrders.pop_front();
            if (askOrder.quantity == 0) askOrders.pop_front();
        }
    }

    void matchOrdersProRata(std::deque<Order>& bidOrders, std::deque<Order>& askOrders) {
        int totalBidQuantity = 0;
        for (const auto& bid : bidOrders) {
            totalBidQuantity += bid.quantity;
        }

        int totalAskQuantity = 0;
        for (const auto& ask : askOrders) {
            totalAskQuantity += ask.quantity;
        }

        int tradeQuantity = std::min(totalBidQuantity, totalAskQuantity);

        auto allocateQuantity = [&](std::deque<Order>& orders, int totalQuantity) {
            for (auto& order : orders) {
                int allocation = (order.quantity * tradeQuantity) / totalQuantity;
                if (allocation > 0) {
                    order.quantity -= allocation;
                    executeTrade(order, orders.front(), allocation); // Temporary order as the counterparty for now
                    if (order.quantity == 0) {
                        orders.pop_front();
                    }
                }
            }
        };

        allocateQuantity(bidOrders, totalBidQuantity);
        allocateQuantity(askOrders, totalAskQuantity);
    }

    void executeTrade(Order& bidOrder, Order& askOrder, int quantity) {
        std::cout << "Trade executed: " << quantity << " @ " << askOrder.price << " (Bid ID: " << bidOrder.orderId << ", Ask ID: " << askOrder.orderId << ")" << std::endl;
    }

    void dispBids() {
        for (const auto& bid : bids) {
            std::cout << "Price: " << bid.first << "\n";
            for (const auto& order : bid.second) {
                std::cout << "  Order ID: " << order.orderId << ", Quantity: " << order.quantity << ", Timestamp: " << order.timestamp << "\n";
            }
        }
    }

    void dispAsks() {
        for (const auto& ask : asks) {
            std::cout << "Price: " << ask.first << "\n";
            for (const auto& order : ask.second) {
                std::cout << "  Order ID: " << order.orderId << ", Quantity: " << order.quantity << ", Timestamp: " << order.timestamp << "\n";
            }
        }
    }
};

int main() {
    // Testing

    OrderBook orderBook("PRORATA");

    Order order1 = { Order::SELL, Order::LIMIT, 5, 100.5 };
    Order order2 = { Order::SELL, Order::LIMIT, 15, 100.5 };
    Order order3 = { Order::SELL, Order::LIMIT, 10, 100.5 };
    Order order4 = { Order::BUY, Order::MARKET, 20 };

    orderBook.addOrder(order1);
    orderBook.addOrder(order2);
    orderBook.addOrder(order3);
    orderBook.addOrder(order4);

    std::cout << "Bids:\n";
    orderBook.dispBids();

    std::cout << "\nAsks:\n";
    orderBook.dispAsks();

    return 0;
}
