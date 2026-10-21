#include "../../include/managers/TransactionManager.h"

bool TransactionManager::addTransaction(
    const Transaction& transaction)
{
    const std::string& transactionId =
        transaction.getTransactionId();

    // Reject duplicate transaction IDs
    if (transactionExists(transactionId))
    {
        return false;
    }

    // Add the transaction record
    transactions.push_back(transaction);

    // Record the ID for future duplicate checks
    transactionIds.insert(transactionId);

    return true;
}

int TransactionManager::findTransactionIndexById(
    const std::string& transactionId) const
{
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

bool TransactionManager::transactionExists(
    const std::string& transactionId) const
{
    auto it = transactionIds.find(transactionId);

    if (it != transactionIds.end())
    {
        return true;
    }

    return false;
}
