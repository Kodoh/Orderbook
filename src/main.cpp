#include "orderbook.h"

int main() {
    // Example usage of the OrderBook
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
