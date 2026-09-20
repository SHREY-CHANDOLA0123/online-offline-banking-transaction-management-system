#include <iostream>
#include "../src/models/Transaction.h"
using namespace std;

int main()
{
    Transaction transaction(
        "TXN1001",
        "ACC1001",
        "ACC1002",
        50000,
        "2026-09-20 22:30:00"
    );

    cout << "Transaction ID: "
              << transaction.getTransactionId()
              << '\n';

    cout << "Sender: "
              << transaction.getSenderAccountId()
              << '\n';

    cout << "Receiver: "
              << transaction.getReceiverAccountId()
              << '\n';

    cout << "Amount: "
              << transaction.getAmount()
              << " paise\n";

    // PENDING -> SUCCESSFUL
    if (transaction.updateStatus(TransactionStatus::SUCCESSFUL))
    {
        cout << "Transaction marked successful\n";
    }

    // SUCCESSFUL -> CANCELLED
    if (!transaction.updateStatus(TransactionStatus::CANCELLED))
    {
        cout << "Invalid cancellation rejected\n";
    }

    // SUCCESSFUL -> REVERSED
    if (transaction.updateStatus(TransactionStatus::REVERSED))
    {
        cout << "Transaction reversed successfully\n";
    }

    return 0;
}

// Compile:

// g++ -std=c++17 tests/test_transaction.cpp src/models/Transaction.cpp -o test_transaction.exe

// Run:

// .\test_transaction.exe