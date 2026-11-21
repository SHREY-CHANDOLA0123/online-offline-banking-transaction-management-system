#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cstddef>

#include "../../src/models/Account.h"

class AccountManager
{
private:
    std::unordered_map<std::string, Account> accounts;

public:
    bool addAccount(const Account& account);
    Account* findAccountById(const std::string& accountId);
    const Account* findAccountById(const std::string& accountId) const;
    bool accountExists(const std::string& accountId) const;
    std::size_t getAccountCount() const;
    bool isEmpty() const;
    // added for the server: all accounts sorted by ID, and a reset
    std::vector<const Account*> getAllAccounts() const;
    void clear();
};

#endif
