#include "../../include/managers/AccountManager.h"

bool AccountManager::addAccount(const Account& account)
{
    // Reject duplicate account IDs
    if (accountExists(account.getAccountId()))
    { return false;
    }

    // Insert the account using its ID as the key
    accounts.emplace(account.getAccountId(), account);
    return true;
}

Account* AccountManager::findAccountById(
    const std::string& accountId)
{auto it = accounts.find(accountId);
if (it == accounts.end())
    {
        return nullptr;
    }
return &(it->second);
}

const Account* AccountManager::findAccountById(
    const std::string& accountId) const
{
    auto it = accounts.find(accountId);

    if (it == accounts.end())
    {
        return nullptr;
    }

    return &(it->second);
}

bool AccountManager::accountExists(
    const std::string& accountId) const
{
    return accounts.find(accountId) != accounts.end();
}

std::size_t AccountManager::getAccountCount() const
{
    return accounts.size();
}

bool AccountManager::isEmpty() const
{
    return accounts.empty();
}
