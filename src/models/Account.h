#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
using namespace std;
enum class AccountStatus
{
    ACTIVE,
    BLOCKED
};

class Account
{
private:
    string accountId;
    string holderName;
    long long balance;          // Stored in paise
    AccountStatus status;

public:
    // Constructor
    Account(const string& accountId,
            const string& holderName,
            long long balance = 0,
            AccountStatus status = AccountStatus::ACTIVE);

    // Getters
    string getAccountId() const;
    string getHolderName() const;
    long long getBalance() const;
    AccountStatus getStatus() const;

    // Setters / account operations
    void setStatus(AccountStatus newStatus);

    bool credit(long long amount);
    bool debit(long long amount);
};

#endif