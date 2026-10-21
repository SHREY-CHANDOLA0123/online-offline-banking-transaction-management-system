#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <unordered_map>

#include "../../src/models/Account.h"

class AccountManager
{
private:
    std::unordered_map<std::string, Account> accounts;

public:
    // Add a new account
    bool addAccount(const Account& account);

    // Find an account using its ID
    Account* findAccountById(const std::string& accountId);

    // Read-only account lookup
    const Account* findAccountById(
        const std::string& accountId
    ) const;

    // Check whether an account exists
    bool accountExists(const std::string& accountId) const;

    // Get the total number of accounts
    std::size_t getAccountCount() const;

    // Check whether the manager contains no accounts
    bool isEmpty() const;
};

#endif
