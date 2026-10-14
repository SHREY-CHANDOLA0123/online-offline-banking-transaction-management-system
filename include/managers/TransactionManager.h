#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <vector>
#include <string>

#include "../../src/models/Transaction.h"

class TransactionManager
{
private:
    std::vector<Transaction> transactions;

public:
    // Add a transaction to the collection
    bool addTransaction(const Transaction& transaction);

    // Find a transaction and return its index
    int findTransactionIndexById(
        const std::string& transactionId
    ) const;

    // Access the transaction history
    const std::vector<Transaction>& getAllTransactions() const;

    // Get the number of stored transactions
    std::size_t getTransactionCount() const;

    // Check whether the history is empty
    bool isEmpty() const;
};

#endif
