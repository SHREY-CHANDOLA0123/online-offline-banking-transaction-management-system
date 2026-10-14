#include "../../include/managers/TransactionManager.h"

bool TransactionManager::addTransaction(
    const Transaction& transaction)
{
    // Reject duplicate transaction IDs
    if (findTransactionIndexById(
            transaction.getTransactionId()) != -1)
    {
        return false;
    }

    // Add the transaction to the vector
    transactions.push_back(transaction);

    return true;
}

int TransactionManager::findTransactionIndexById(
    const std::string& transactionId) const
{
    // Searching for a transaction
    for (std::size_t i = 0; i < transactions.size(); i++)
    {
        if (transactions[i].getTransactionId() == transactionId)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

const std::vector<Transaction>&
TransactionManager::getAllTransactions() const
{
    return transactions;
}

std::size_t TransactionManager::getTransactionCount() const
{
    return transactions.size();
}

bool TransactionManager::isEmpty() const
{
    return transactions.empty();
}
