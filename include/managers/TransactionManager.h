#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <vector>
#include <string>
#include <unordered_set>

#include "../../src/models/Transaction.h"

class TransactionManager
{
private:
    std::vector<Transaction> transactions;
    std::unordered_set<std::string> transactionIds;

public:
    bool addTransaction(const Transaction& transaction);
    int findTransactionIndexById(const std::string& transactionId) const;
    const std::vector<Transaction>& getAllTransactions() const;
    std::size_t getTransactionCount() const;
    bool isEmpty() const;
    bool transactionExists(const std::string& transactionId) const;
    // added for the server: look up a stored transaction (so its status can be updated) and reset
    Transaction* findTransactionById(const std::string& transactionId);
    void clear();
};

#endif
