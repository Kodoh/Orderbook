import random
import struct

message_types = {
    "A": "add order",
    "F": "add order with mipid attribution",
    "E": "order executed",
    "X": "order executed with price message",
    "C": "order cancel",
    "D": "order delete",
    "U": "order replace"
}

def generate_message():
    message_type = random.choice(list(message_types.keys()))
    message_value = ord(message_type)  
    return message_type, message_value

def simulate_market_data(num_messages):
    for _ in range(num_messages):
        message_type, message_value = generate_message()
        print(f"{message_type}{hex(message_value)}")

simulate_market_data(10)            # change
