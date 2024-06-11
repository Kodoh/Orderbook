#include "orderbook.h"

// Constructor
OrderBook::OrderBook(const std::string& allocation) : allocation(allocation) {}

void OrderBook::addOrder(Order order) {
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

void OrderBook::removeOrder(const Order& order) {
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

void OrderBook::checkStopOrders() {
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

double OrderBook::getLowestAskPrice() const {
    return asks.empty() ? std::numeric_limits<double>::max() : asks.begin()->first;
}

double OrderBook::getHighestBidPrice() const {
    return bids.empty() ? 0.0 : bids.rbegin()->first;
}

void OrderBook::matchOrders() {
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

void OrderBook::matchOrdersFIFO(std::deque<Order>& bidOrders, std::deque<Order>& askOrders) {
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

void OrderBook::matchOrdersProRata(std::deque<Order>& bidOrders, std::deque<Order>& askOrders) {
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

void OrderBook::executeTrade(Order& bidOrder, Order& askOrder, int quantity) {
    std::cout << "Trade executed: " << quantity << " @ " << askOrder.price << " (Bid ID: " << bidOrder.orderId << ", Ask ID: " << askOrder.orderId << ")" << std::endl;
}

void OrderBook::dispBids() {
    for (const auto& bid : bids) {
        std::cout << "Price: " << bid.first << "\n";
        for (const auto& order : bid.second) {
            std::cout << "  Order ID: " << order.orderId << ", Quantity: " << order.quantity << ", Timestamp: " << order.timestamp << "\n";
        }
    }
}

void OrderBook::dispAsks() {
    for (const auto& ask : asks) {
        std::cout << "Price: " << ask.first << "\n";
        for (const auto& order : ask.second) {
            std::cout << "  Order ID: " << order.orderId << ", Quantity: " << order.quantity << ", Timestamp: " << order.timestamp << "\n";
        }
    }
}
