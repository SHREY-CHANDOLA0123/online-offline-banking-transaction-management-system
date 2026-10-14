#include <iostream>

#include "../include/managers/AccountManager.h"

int main()
{
    AccountManager manager;

    std::cout << "Initial account count: "
              << manager.getAccountCount()
              << '\n';

    std::cout << "Is account manager empty? "
              << (manager.isEmpty() ? "Yes" : "No")
              << "\n\n";

    // Create account objects
    Account account1(
        "ACC1001",
        "Rahul Sharma",
        100000
    );

    Account account2(
        "ACC1002",
        "Priya Singh",
        200000
    );

    Account duplicateAccount(
        "ACC1001",
        "Amit Kumar",
        300000
    );

    // Test insertion
    if (manager.addAccount(account1))
    {
        std::cout << "Account 1 added successfully\n";
    }

    if (manager.addAccount(account2))
    {
        std::cout << "Account 2 added successfully\n";
    }

    // Test duplicate rejection
    if (!manager.addAccount(duplicateAccount))
    {
        std::cout << "Duplicate account ID rejected\n";
    }

    // Test successful lookup
    Account* account = manager.findAccountById("ACC1001");

    if (account != nullptr)
    {
        std::cout << "\nAccount found\n";
        std::cout << "Account ID: "
                  << account->getAccountId()
                  << '\n';

        std::cout << "Holder: "
                  << account->getHolderName()
                  << '\n';

        std::cout << "Balance: "
                  << account->getBalance()
                  << " paise\n";
    }

    // Test missing-account lookup
    if (manager.findAccountById("ACC9999") == nullptr)
    {
        std::cout << "\nNon-existent account correctly reported\n";
    }

    // Test account operations through the returned pointer
    if (account != nullptr && account->credit(50000))
    {
        std::cout << "Credit operation successful\n";
    }

    if (account != nullptr)
    {
        std::cout << "Updated balance: "
                  << account->getBalance()
                  << " paise\n";
    }

    std::cout << "\nTotal accounts: "
              << manager.getAccountCount()
              << '\n';

    return 0;
}
