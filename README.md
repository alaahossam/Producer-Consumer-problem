# Producer-Consumer-problem
### Producers
Each producer is supposed to continuously declare the price of one commodity. For simplicity, we assume that each commodity price follows a normal distribution with parameters (𝜇,𝜎2). Therefore, producers will generate a new random price, share it with the consumer, and then sleep for an interval before producing the next price.
All producers are processes running the same codebase. Producers are to be run concurrently, either in separate terminals, or in the background. While running a producer, you will specify the following command line arguments:
• Commodity name (e.g., GOLD – Assume no more than 10 characters.)
• Commodity Price Mean; 𝜇 – a double value.
• Commodity Price Standard Deviation; 𝜎 – a double value.
• Length of the sleep interval in milliseconds; T – an integer.
• Bounded-Buffer Size (number of entries); N – an integer.
### Consumer
The consumer is to print the current price of each commodity, along the average of the current and past 4 readings. An Up/Down arrow to show whether the current Price (AvgPrice) got increased or decreased from the prior one. Until you receive current prices, use 0.00 for the current price of any commodity.
For simplicity, let’s limit the commodities to GOLD, SILVER, CRUDEOIL, NATURALGAS, ALUMINIUM, COPPER, NICKEL, LEAD, ZINC, MENTHAOIL, and COTTON. Show the commodities in alphabetical order. While running the consumer, you will specify the following command line argument:
• Bounded-Buffer Size (number of entries); N – an integer.
