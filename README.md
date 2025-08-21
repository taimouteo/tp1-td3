# School Project N°4: Abstract Data Types and Computational Complexity

A prototype implementation of a virtual wallet that runs on a rudimentary blockchain is available. The prototype has full functionality but suffers from severe performance issues.  

In view of a production release, the complexity requirements that the application must meet to provide a top-level user experience have been determined, and the students have been entrusted with the task of adapting the current implementation to meet these requirements.  

In this system, a blockchain (an instance of the `Blockchain` class) is used, which has various wallets registered. The functionality of the blockchain consists of:  
- storing transactions  
- opening new wallets  
- performing transactions between registered wallets  

The process of creating a wallet is carried out through the method `blockchain.abrir_billetera()`, which is responsible for:  
1. Creating the wallet and adding it to the internal registry  
2. Performing a “seed” transaction of 100 currency units  
3. Returning a pointer to the created wallet for further use  

This class provides a very rudimentary interface, not intended to be used directly by end users. The `Billetera` class is responsible for providing advanced functionality that will later be used to program the mobile application for clients:  
- obtaining the current balance  
- obtaining the latest transactions performed  
- obtaining the historical balance at the end of any day  
- obtaining a list of wallets to which money is most frequently sent
