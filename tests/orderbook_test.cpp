#include <gtest/gtest.h>
#include "../src/orderbook.h"

TEST(OrderBookTest, AddBuyOrder) {
    OrderBook orderBook("FIFO");
    Order order = { Order::BUY, Order::LIMIT, 10, 100.5 };
    orderBook.addOrder(order);

    ASSERT_EQ(orderBook.getHighestBidPrice(), 100.5);
    ASSERT_EQ(orderBook.getLowestAskPrice(), std::numeric_limits<double>::max());
}


TEST(OrderBookTest, AddAskOrder) {
    OrderBook orderBook("FIFO");
    Order order1 = { Order::SELL, Order::LIMIT, 10, 100.5 };
    Order order2 = { Order::SELL, Order::LIMIT, 10, 90.2 };
    Order order3 = { Order::SELL, Order::LIMIT, 10, 95.3 };
    orderBook.addOrder(order1);
    orderBook.addOrder(order2);
    orderBook.addOrder(order3);

    ASSERT_EQ(orderBook.getLowestAskPrice(), 90.2);
}

TEST(OrderBookTest, MatchProrata) {
    OrderBook orderBook("PRORATA");

    Order sellOrder1 = { Order::SELL, Order::LIMIT, 15, 100.5 };
    Order buyOrder1 = { Order::BUY, Order::LIMIT, 10, 101.5 };

    orderBook.addOrder(sellOrder1);
    orderBook.addOrder(buyOrder1);

    orderBook.dispAsks();
    const auto& bids = orderBook.getBids();
    const auto& asks = orderBook.getAsks();

    ASSERT_TRUE(bids.empty());

    ASSERT_EQ(asks.size(), 1);
    ASSERT_EQ(asks.begin()->first, 100.5); 
    ASSERT_EQ(asks.begin()->second.size(), 1);
    ASSERT_EQ(asks.begin()->second.front().quantity, 5);
    ASSERT_EQ(asks.begin()->second.front().orderId, 3);
}

TEST(OrderBookTest, LimitOrderFIFO) {
    OrderBook orderBook("FIFO");
    Order order1 = { Order::SELL, Order::LIMIT, 10, 100.5 };
    Order order2 = { Order::SELL, Order::LIMIT, 5, 90.2 };
    Order order3 = { Order::SELL, Order::LIMIT, 10, 95.3 };
    Order order4 = { Order::SELL, Order::LIMIT, 5, 100.5 };

    Order order5 = { Order::BUY, Order::LIMIT, 5, 100 };
    Order order6 = { Order::BUY, Order::LIMIT, 15, 50.4 };

    orderBook.addOrder(order1);
    orderBook.addOrder(order2);
    orderBook.addOrder(order3);
    orderBook.addOrder(order4);
    orderBook.addOrder(order5);
    orderBook.addOrder(order6);
    
    ASSERT_EQ(orderBook.getLowestAskPrice(), 95.3);
    ASSERT_EQ(orderBook.getHighestBidPrice(), 50.4);
}

TEST(OrderBookTest, MatchOrderMarket) {
    OrderBook orderBook("FIFO");
    Order order1 = { Order::SELL, Order::LIMIT, 10, 100.5 };
    Order order2 = { Order::SELL, Order::LIMIT, 10, 90.2 };
    Order order3 = { Order::SELL, Order::LIMIT, 10, 95.3 };
    Order order4 = { Order::BUY, Order::MARKET, 20};

    orderBook.addOrder(order4);
    ASSERT_EQ(orderBook.getHighestBidPrice(), std::numeric_limits<double>::max());
    
    orderBook.addOrder(order1);
    orderBook.addOrder(order2);
    orderBook.addOrder(order3);

    ASSERT_EQ(orderBook.getLowestAskPrice(), 95.3);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}