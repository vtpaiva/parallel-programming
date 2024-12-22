### Pen Factory Simulation with Pthreads, Semaphores, and Condition Variables

This application simulates a pen factory system using threads in C with Pthreads, semaphores, and condition variables.
It operates in a Linux environment, with 6 threads performing various tasks related to manufacturing pens, storing raw materials, and handling purchases.

#### Overview
The application simulates a pen factory with the following tasks distributed across 6 threads:

1. **Creator (Rank 0)** - This thread manages the creation of other threads and controls the main execution flow of the program.
2. **Raw Material Storage (Rank 1)** - This thread is responsible for supplying raw materials to the manufacturing cell at regular intervals.
3. **Pen Manufacturing Cell (Rank 2)** - This thread handles the pen manufacturing process by converting raw materials into pens.
4. **Control (Rank 3)** - This thread ensures that production and supply processes occur only when certain conditions are met, such as space in the pen storage.
5. **Pen Storage (Rank 4)** - This thread manages the storage of manufactured pens, ensuring they are available for sale.
6. **Buyer (Rank 5)** - This thread simulates a buyer who requests pens from the storage in specified quantities.

#### Input Arguments
The program accepts 7 input arguments for configuring the execution:

1. **Quantity of raw materials in stock** (int)
2. **Quantity of raw materials sent to the pen manufacturing cell per interaction** (int)
3. **Time in seconds between each raw material delivery** (int)
4. **Time in seconds to manufacture a single pen** (int)
5. **Maximum capacity of pens in the storage** (int)
6. **Quantity of pens bought per interaction** (int)
7. **Time in seconds between each purchase request from the buyer** (int)

#### Program Execution Flow
The threads interact and are synchronized using semaphores and condition variables. The main function (creator thread) initializes the threads and manages the execution flow:

- **Raw Material Delivery**: The raw material storage sends materials to the pen manufacturing cell at regular intervals, with the quantity specified by the input parameters.
- **Pen Manufacturing**: The pen manufacturing cell processes raw materials into pens. The control thread ensures that manufacturing occurs only when raw materials are available.
- **Pen Storage**: The pen storage manages the pens and ensures that they are available for sale. If the storage is full, the production process is suspended until space is available.
- **Buyer Interaction**: The buyer requests pens from the storage. If the requested amount is available, the pens are delivered; if not, the buyer is informed that their request cannot be fulfilled.
- **Control Logic**: The control thread determines whether certain actions can occur based on the current state of the system (e.g., whether the storage has enough space for more pens).

#### Output
At the end of the program, the creator thread prints the total number of pens bought by the buyer. During the execution, the output allows monitoring the progress of raw material delivery, pen manufacturing, storage, and buying transactions.

#### Synchronization
The program uses **mutexes** to ensure exclusive access to shared resources such as raw materials, pens, and storage spaces. **Condition variables** are used to synchronize the execution of threads based on specific conditions, such as availability of storage space or raw materials.

#### Conclusion
This simulation provides an efficient way to model the process of manufacturing and selling pens in a factory, using Pthreads to represent concurrent processes and synchronizing them with semaphores and condition variables. The system allows for flexible interaction between the threads, ensuring proper execution of the production and sale process in a controlled environment.

#### Dependencies
- GCC or any standard C compiler
- Linux-based environment with Pthreads support
- Time-related functions provided by the standard C library.