#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>

enum class AccountStatus
{   ACTIVE,
    BLOCKED
};

class Account
{   private:
        std::string accountId;
        std::string holderName;
        long long balance;   // Stored in paise
        AccountStatus status;
    public:
        // Constructor
        Account(const std::string &accountId, const std::string &holderName, long long balance = 0, AccountStatus status = AccountStatus::ACTIVE);

        // Getters
        std::string getAccountId() const;
        std::string getHolderName() const;
        long long getBalance() const;
        AccountStatus getStatus() const;

        // Setters / Account Operations
        void setStatus(AccountStatus newStatus);
        bool credit(long long amount);
        bool debit(long long amount);
        bool debitOffline(long long amount);   // pre-authorised offline payment: may overdraw the account
};

#endif