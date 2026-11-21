#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <string>

enum class TransactionStatus
{   PENDING,
    SUCCESSFUL,
    FAILED,
    CANCELLED,
    REVERSED
};

class Transaction
{   private:
        std::string transactionId;
        std::string senderAccountId;
        std::string receiverAccountId;
        long long amount;          
        TransactionStatus status;
        std::string createdAt;
        std::string mode = "ONLINE", reason, settledAt;   // filled in by the server on settlement
        bool canTransitionTo(TransactionStatus newStatus) const;

    public:
        // Constructor
        Transaction(const std::string &transactionId, const std::string &senderAccountId, const std::string &receiverAccountId, long long amount, const std::string &createdAt);

        // Getters
        std::string getTransactionId() const;
        std::string getSenderAccountId() const;
        std::string getReceiverAccountId() const;
        long long getAmount() const;
        TransactionStatus getStatus() const;
        std::string getCreatedAt() const;
        std::string getMode() const { return mode; }
        std::string getReason() const { return reason; }
        std::string getSettledAt() const { return settledAt; }
        void setSettlement(const std::string &m, const std::string &r, const std::string &at) { mode = m; reason = r; settledAt = at; }

        // Transaction Status Management
        bool updateStatus(TransactionStatus newStatus);
};

#endif