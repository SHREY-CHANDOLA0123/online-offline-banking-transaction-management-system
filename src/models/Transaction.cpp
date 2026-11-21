#include "Transaction.h"
using namespace std;
Transaction::Transaction(const string &transactionId, const string &senderAccountId, const string &receiverAccountId, long long amount, const string &createdAt)
{   this->transactionId = transactionId;
    this->senderAccountId = senderAccountId;
    this->receiverAccountId = receiverAccountId;
    this->amount = amount;
    this->status = TransactionStatus::PENDING;
    this->createdAt = createdAt;
}

string Transaction::getTransactionId() const
{   return transactionId;
}

string Transaction::getSenderAccountId() const
{   return senderAccountId;
}

string Transaction::getReceiverAccountId() const
{   return receiverAccountId;
}      

long long Transaction::getAmount() const
{   return amount;
}

TransactionStatus Transaction::getStatus() const
{   return status;
}

string Transaction::getCreatedAt() const
{   return createdAt;
}

bool Transaction::canTransitionTo(TransactionStatus newStatus) const
{   switch (status)
    {
        case TransactionStatus::PENDING:
            return newStatus == TransactionStatus::SUCCESSFUL ||
                   newStatus == TransactionStatus::FAILED ||
                   newStatus == TransactionStatus::CANCELLED;

        case TransactionStatus::SUCCESSFUL:
            return newStatus == TransactionStatus::REVERSED;

        case TransactionStatus::FAILED:
            return false;

        case TransactionStatus::CANCELLED:
            return false;

        case TransactionStatus::REVERSED:
            return false;
    }

    return false;
}

bool Transaction::updateStatus(TransactionStatus newStatus)
{   if(!(canTransitionTo(newStatus)))
        {return false;
        }
    status = newStatus;
    return true;
}