# Orderbook
# High-Frequency Trading Order Book with FPGA Acceleration

![HFT](https://img.shields.io/badge/High--Frequency%20Trading-FPGA-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![License](https://img.shields.io/badge/License-MIT-green)

## Overview

This project implements a high-frequency trading (HFT) order book system utilizing FPGA acceleration to achieve low latency and high throughput. The order book is designed to handle real-time market data and execute trades efficiently. The FPGA processes latency-sensitive tasks such as market data handling and initial order validation, while the CPU handles complex order matching, risk checks, and trade execution.

## Features

- **High Performance**: Leveraging FPGA for real-time data processing.
- **Low Latency**: Optimized for high-frequency trading.
- **Scalable Architecture**: Efficient handling of high volume of orders and updates.
- **Risk Management**: Built-in risk checks and validations.
- **Real-time Analytics**: Monitoring system performance and market conditions.
- **Resilient Design**: State persistence and recovery mechanisms.

## Architecture

### Components

1. **Market Data Handler (FPGA)**
   - Parses and normalizes incoming market data.
   - Handles initial data filtering and preprocessing.

2. **Order Entry Handler (FPGA)**
   - Receives and pre-processes incoming orders.
   - Performs basic validation checks.

3. **Order Management System (CPU - C++)**
   - Maintains the state of the order book.
   - Processes and matches orders.
   - Executes trades and handles risk management.

4. **FPGA Communication Layer (CPU - C++)**
   - Manages data exchange between the CPU and FPGA via PCIe/DMA interface.

5. **Trade Execution and Reporting (CPU - C++)**
   - Executes matched trades and generates reports.

6. **Monitoring and Logging (CPU - C++)**
   - Monitors system performance.
   - Logs significant events and transactions.

### Data Flow Diagram

<img width="746" alt="image" src="https://github.com/Kodoh/Orderbook/assets/45899701/e8564ae6-0d79-41f2-94c5-28070c45c8ca">
