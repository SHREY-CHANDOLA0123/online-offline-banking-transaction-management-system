#include <iostream>

#include "../include/managers/TransactionManager.h"

int main()
{
    TransactionManager manager;

    std::cout << "Initial transaction count: "
              << manager.getTransactionCount()
              << '\n';

    std::cout << "Is history empty? "
              << (manager.isEmpty() ? "Yes" : "No")
              << "\n\n";

    Transaction transaction1(
        "TXN1001",
        "ACC1001",
        "ACC1002",
        50000,
        "2026-10-14 14:00:00"
    );

    Transaction transaction2(
        "TXN1002",
        "ACC1001",
        "ACC1003",
        20000,
        "2026-10-14 14:05:00"
    );

    Transaction duplicateTransaction(
        "TXN1001",
        "ACC1002",
        "ACC1003",
        10000,
        "2026-10-14 14:10:00"
    );

    // Test insertion
    if (manager.addTransaction(transaction1))
    {
        std::cout << "Transaction 1 added successfully\n";
    }

    if (manager.addTransaction(transaction2))
    {
        std::cout << "Transaction 2 added successfully\n";
    }

    // Test duplicate rejection
    if (!manager.addTransaction(duplicateTransaction))
    {
        std::cout << "Duplicate transaction ID rejected\n";
    }

    // Test searching
    int index = manager.findTransactionIndexById("TXN1002");

    if (index != -1)
    {
        std::cout << "\nTXN1002 found at index: "
                  << index << '\n';
    }
    else
    {
        std::cout << "\nTransaction not found\n";
    }

    // Test searching for a missing ID
    if (manager.findTransactionIndexById("TXN9999") == -1)
    {
        std::cout << "Missing transaction correctly reported\n";
    }

    // Display transaction history
    std::cout << "\nTransaction History\n";
    std::cout << "-------------------\n";

    for (const Transaction& transaction : manager.getAllTransactions())
    {
        std::cout << "Transaction ID: "
                  << transaction.getTransactionId() << '\n';

        std::cout << "Sender: "
                  << transaction.getSenderAccountId() << '\n';

        std::cout << "Receiver: "
                  << transaction.getReceiverAccountId() << '\n';

        std::cout << "Amount: "
                  << transaction.getAmount()
                  << " paise\n";

        std::cout << "-------------------\n";
    }

    std::cout << "Total transactions: "
              << manager.getTransactionCount()
              << '\n';

    return 0;
}
