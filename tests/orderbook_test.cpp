#include <gtest/gtest.h>
#include "../src/orderbook.h"

TEST(OrderBookTest, AddBuyOrder) {
    OrderBook orderBook("FIFO");
    Order order = { Order::BUY, Order::LIMIT, 10, 100.5 };
    orderBook.addOrder(order);

    ASSERT_EQ(orderBook.getHighestBidPrice(), 100.5);
    ASSERT_EQ(orderBook.getLowestAskPrice(), std::numeric_limits<double>::max());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
