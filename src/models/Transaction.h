#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <string>
using namespace std;

enum class TransactionStatus
{   PENDING,
    SUCCESSFUL,
    FAILED,
    CANCELLED,
    REVERSED
};

class Transaction
{   private:
        string transactionId;
        string senderAccountId;
        string receiverAccountId;
        long long amount;          
        TransactionStatus status;
        string createdAt;
        bool canTransitionTo(TransactionStatus newStatus) const;

    public:
        // Constructor
        Transaction(const string &transactionId, const string &senderAccountId, const string &receiverAccountId, long long amount, const string &createdAt);

        // Getters
        string getTransactionId() const;
        string getSenderAccountId() const;
        string getReceiverAccountId() const;
        long long getAmount() const;
        TransactionStatus getStatus() const;
        string getCreatedAt() const;

        // Transaction Status Management
        bool updateStatus(TransactionStatus newStatus);
};

#endif